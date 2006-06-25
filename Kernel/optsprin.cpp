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
// Implementation of the print tabs of the options dialog box

/*
*/

#include "camtypes.h"

//	 WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#include "appprefs.h"
#include "optsprin.h"
#include "optsmsgs.h"
//#include "printdlg.h"
#include "princomp.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "isetres.h"
#include "colplate.h"
#include "printmsg.h"
//#include "markn.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "printdlg.h"
#include "prdlgctl.h"
//#include "barsdlgs.h"
#include "camelot.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "prnmks.h"
#include "prnmkcom.h"
#include "colmsg.h"
#include "colormgr.h"
#include "bubbleid.h"

// for testing printing
#ifdef _DEBUG
#include "noderect.h"
#include "layer.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sglayer.h"
#include "keypress.h"
//#include "printprg.h"
#endif

//#include "will2.h"
//#include "customlist.h"

CC_IMPLEMENT_DYNAMIC(PrintBaseTab,  		OptionsTabs)
CC_IMPLEMENT_DYNAMIC(PrintGeneralTab, 		PrintBaseTab)
CC_IMPLEMENT_DYNAMIC(PrintLayoutTab,  		PrintBaseTab)
CC_IMPLEMENT_DYNAMIC(PrintSepsTab,  		PrintBaseTab)
CC_IMPLEMENT_DYNAMIC(PrintImagesetterTab,  	PrintBaseTab)
CC_IMPLEMENT_DYNCREATE(OpShowPrintBorders, 	Operation)
CC_IMPLEMENT_DYNCREATE(SepsDlg,				DialogOp);


// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

//------------------------------------------------

static BOOL IgnoreTextChangedMsgs = FALSE;


// This local PrintControl contains a copy of the associated document's print control object.
// The local copy is copied from the document when InitSection() and UpdateSection() is called.
// The local copy is copied into the document when CommitSection() is called.
// NOTE that we have a static print control because it must be SHARED by all the different tabs!
PrintControl PrintBaseTab::LocalPrintControl;
Document *PrintBaseTab::pLastPrintControlDocument = NULL;

static CGadgetID ScreenIDs[] =	// NULL terminated list of Screen control IDs
{
	//Screen types
	_R(SCREEN_TYPE_BASE),
	_R(IDS_SCRTYPE_SPOT1),
	_R(IDS_SCRTYPE_SPOT2),
	_R(IDS_SCRTYPE_TRIPLESPOT1),
	_R(IDS_SCRTYPE_TRIPLESPOT2),
	_R(IDS_SCRTYPE_ELLIPTICAL),
	_R(IDS_SCRTYPE_LINE),
	_R(IDS_SCRTYPE_CROSSHATCH),
	_R(IDS_SCRTYPE_MEZZOTINT),
	_R(IDS_SCRTYPE_SQUARE),
	_R(IDS_SCRTYPE_DITHER),
	0
};


/********************************************************************************************

>	PrintBaseTab::PrintBaseTab() 


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintBaseTab constructor.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PrintBaseTab::PrintBaseTab()
{   
}        



/********************************************************************************************

>	PrintBaseTab::~PrintBaseTab()


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintBaseTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
PrintBaseTab::~PrintBaseTab()
{   
	// Vape the cached print control info
	pLastPrintControlDocument = NULL;

	// Clear out the entire print control to sensible defaults. This includes wiping the
	// plate list, which ensures there are no IndexedColours left "in use" when we exit
	PrintControl BlankPC;
	LocalPrintControl = BlankPC;
}        



/******************************************************************************************

>	BOOL PrintBaseTab::IsDocumentOption()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a document option.	  
	Purpose:	Allows the document options dialog code to determine if this tab belongs
				to the group of document options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL PrintBaseTab::IsDocumentOption()
{
	return FALSE;	// This tab is not a document option. Changed from True 3/3/97.
}



/******************************************************************************************

>	BOOL PrintBaseTab::IsProgramOption()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the program options dialog code to determine if this tab belongs
				to the group of program options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL PrintBaseTab::IsProgramOption()
{
	return FALSE;	// This tab is not a program option. Changed from True 3/3/97.
}



/******************************************************************************************

>	BOOL PrintBaseTab::IsPrintingOption()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a printing option.	  
	Purpose:	Allows the printing options dialog code to determine if this tab belongs
				to the group of printing options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL PrintBaseTab::IsPrintingOption()
{
	return TRUE;	// This tab is a printing option 
}



/********************************************************************************************

>	BOOL PrintBaseTab::TalkToPage()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This calls pPrefsDlg->TalkToPage() with the ID returned from GetPageID()
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PrintBaseTab::TalkToPage()
{
	if (pPrefsDlg != NULL)
		return (pPrefsDlg->TalkToPage(GetPageID()));

	return FALSE;
}



/********************************************************************************************

>	void PrintBaseTab::UpdateDocName()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This displays the doc name returned from GetDocumentName() in the tab
				If it can't do it for some reason (i.e. NULL dlg ptr) it just returns
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PrintBaseTab::UpdateDocName()
{
	if (pPrefsDlg != NULL && TalkToPage())
		pPrefsDlg->SetStringGadgetValue(_R(IDC_PRINT_DOCNAME),*GetDocumentName());
}



/********************************************************************************************

>	BOOL PrintBaseTab::GreySection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	PrintBaseTab::UngreySection;

********************************************************************************************/

BOOL PrintBaseTab::GreySection()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintBaseTab::GreySection called with no dialog pointer");

	// Vape the cached print control info
	pLastPrintControlDocument = NULL;

	// Clear out the entire print control to sensible defaults. This includes wiping the
	// plate list, which ensures there are no IndexedColours left "in use" when we exit
	PrintControl BlankPC;
	LocalPrintControl = BlankPC;

	if (TalkToPage())
	{
		// Make sure the information field displaying the name of the current document
		// is correct.
		UpdateDocName();

		AllGreyed = TRUE;
		EnableControls();	// Call our central greying/ungreying function
	}

	return TRUE;
} 



/********************************************************************************************

>	BOOL PrintBaseTab::UngreySection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	PrintBaseTab::UngreySection;

********************************************************************************************/

BOOL PrintBaseTab::UngreySection()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintBaseTab::UngreySection called with no dialog pointer");

	// Vape the cached print control info
	pLastPrintControlDocument = NULL;
 
	// Clear out the entire print control to sensible defaults. This includes wiping the
	// plate list, which ensures there are no IndexedColours left "in use" when we exit
	PrintControl BlankPC;
	LocalPrintControl = BlankPC;

	if (TalkToPage())
	{
		// Make sure the information field displaying the name of the current document
		// is correct.
		UpdateDocName();

		AllGreyed = FALSE;

		ShowDetails();		// Make sure all details have been updated to show the new document
		EnableControls();	// Call our central greying/ungreying function
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL PrintBaseTab::UpdateSection(String_256 *DocumentName)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
	Errors:		-
	SeeAlso:	PrintBaseTab::GreySection; PrintBaseTab::UngreySection; PrintBaseTab::InitSection;

********************************************************************************************/

BOOL PrintBaseTab::UpdateSection(String_256 *DocumentName)
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintBaseTab::UpdateSection called with no dialog pointer");

	if (!TalkToPage())
		return TRUE;	// Just return if TalkToPage() fails

	// Call the normal init section code
	return InitSection();
}



/******************************************************************************************

>	PrintControl* PrintBaseTab::GetPrintControl()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to the associated doc's print control object or NULL if it fails
	Purpose:	Access function to the doc's print control object

				This has to find the doc's print component and ask it for the print
				control object
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

PrintControl* PrintBaseTab::GetPrintControl()
{
	// Return NULL if we don't have a document ptr
	if (pDocument == NULL)
		return NULL;

	// Find the doc's print component
	PrintComponent* pPrComp = (PrintComponent*)pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
	ERROR2IF(pPrComp == NULL,NULL,"Can't find the associated doc's print component");

	// Find a ptr to the print component's print control object
	PrintControl* pPrCtrl = pPrComp->GetPrintControl();
	ERROR2IF(pPrCtrl == NULL,NULL,"The doc's print component gave me a NULL print control object");

	return pPrCtrl;
}



/******************************************************************************************

>	void PrintBaseTab::CopyLocalPrintControlFromDoc(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96

	Purpose:	Copies the current document print control into our locl print control copy
				(i.e. copies the current document configuration into our local copy, either
				initialising the values that the dialogue should be using, or effectively
				"cancelling" any user changes made in the dialogue. Note that you'll have
				to call updatesection to update the dialogue to use the new settings.

				The print tabs all work *exclusively* on the copy, and then the base tab
				will update the real document print control from the local one as 
				appropriate when the dialogue values are committed.

	Notes:		This caches the print control information for the options dialogue document.
				Subsequent calls don't bother doing anything, on the grounds that due to the
				random times at which our tabs are initialised, they can try to copy the doc
				information into the local control when doing so will overwrite changed
				but non-committed values for this document. (e.g. without this caching
				lock on it, clicking the "do separations" button doesn't auto-update
				the "output printers marks" option in the other tab until you've "seen"
				that tab!

******************************************************************************************/

void PrintBaseTab::CopyLocalPrintControlFromDoc(void)
{
	// If cached for this document, don't copy it again, because this could lose settings
	// which have been changed in the dialogue but not yet committed.
	if (pLastPrintControlDocument == pDocument)
		return;

	PrintControl *PrintCtrl = GetPrintControl();
	if (PrintCtrl != NULL)
	{
		LocalPrintControl = *PrintCtrl;
		LocalPrintControl.SetUp(pDocument->GetSelectedSpread());

		pLastPrintControlDocument = pDocument;
	}
}


/******************************************************************************************

>	static void PrintBaseTab::DocumentSettingsUpdated(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/96

	Purpose:	Copies the current document print control into our locl print control copy
				Called by the AppPrefs base dialogue when we soft-commit with the Apply button

******************************************************************************************/

void PrintBaseTab::DocumentSettingsUpdated(void)
{
	Document *pDoc = Document::GetSelected();
	if (pDoc != NULL)
	{
		PrintComponent* pPrComp = (PrintComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
		if (pPrComp != NULL)	
		{
			PrintControl* pPrCtrl = pPrComp->GetPrintControl();
			if (pPrCtrl != NULL)
			{
				LocalPrintControl = *pPrCtrl;
				LocalPrintControl.SetUp(pDoc->GetSelectedSpread());

				pLastPrintControlDocument = pDoc;
			}
		}
	}
}



/******************************************************************************************

>	void PrintBaseTab::CopyLocalPrintControlToDoc(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96

	Purpose:	Copies our local print control copy into the current document
				(i.e. commits the local dialogue values into the document)

				The print tabs all work *exclusively* on the copy, and then the base tab
				will update the real document print control from the local one as 
				appropriate when the dialogue values are committed.

******************************************************************************************/

void PrintBaseTab::CopyLocalPrintControlToDoc(void)
{
	PrintControl *PrintCtrl = GetPrintControl();
	if (PrintCtrl != NULL)
	{
		*PrintCtrl = LocalPrintControl;
		if (pDocument)						// If we have just changed document settings
			pDocument->SetModified(TRUE);	// Then mark the document modified!!!
	}
}


/******************************************************************************************

>	BOOL PrintBaseTab::InitSection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the print general section of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PrintBaseTab::InitSection()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintBaseTab::InitSection called with no dialog pointer");

	if (pDocument == NULL)
		return GreySection();

	AllGreyed = FALSE;	// we are ungreyed by default

	// Make sure the information field displaying the name of the current document
	// is correct.
	UpdateDocName();

	// Copy the doc's print control data into our local copy
	CopyLocalPrintControlFromDoc();

	if (!TalkToPage())
		return TRUE;	// Just return if TalkToPage() fails

	// Display the print data in our tab
	//return ShowDetails(); // I've commented this out as it should be done in the actual tab's init function - Adrian 
	return TRUE;
}



/********************************************************************************************

>	virtual BOOL PrintBaseTab::PageSizeHasChanged()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		-
	Outputs:	-
	Returns:	True if handled ok, False if not. 
	Purpose:	Allows a tab to update any page size items being displayed. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PrintBaseTab::PageSizeHasChanged()
{
	// Sent when the page size has been changed
	LocalPrintControl.SetUp(pDocument->GetSelectedSpread());

	if (!TalkToPage())
		return TRUE;	// Just return if TalkToPage() fails

	return ShowDetails();
}

/******************************************************************************************

>	void PrintBaseTab::EnableControl(CGadgetID Gadget, BOOL Enabled)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		Gadget	= gadget ID
				Enabled = TRUE to enable, FALSE to disable gadget
	Outputs:	-
	Returns:	-
	Purpose:	This will enable the gadget given if Enabled is TRUE AND AllGreyed is FALSE,
				i.e. it is a central control-greying func that takes the AllGreyed state of the tab
				into account.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void PrintBaseTab::EnableControl(CGadgetID Gadget, BOOL Enabled)
{

// Code to comment out non-functional aspects of the print dlg (18/4/95)
/*
if (Gadget != _R(IDC_GROUP_PRINTMETHOD) &&
	Gadget != _R(IDC_PRINTMETHOD_NORMAL) &&
	Gadget != _R(IDC_PRINTMETHOD_BITMAP) &&
	Gadget != _R(IDC_PRINTMETHOD_AABITMAP) &&

	Gadget != _R(IDC_GROUP_DPI) &&
	Gadget != _R(IDC_DPIAUTO) &&
	Gadget != _R(IDC_DPIMANUAL) &&
	Gadget != _R(IDC_DPIEDIT) &&
	Gadget != _R(IDC_DPIEDITTEXT))
{
	Enabled = FALSE;
}
*/
	if (pPrefsDlg != NULL)
		pPrefsDlg->EnableGadget(Gadget,Enabled && !AllGreyed);
}

/******************************************************************************************

>	void PrintBaseTab::PrintSettingsChanged()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called by AppPrefsDlg::Message() when it received a PrintMsg::SETTINGSCHANGED message.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void PrintBaseTab::PrintSettingsChanged()
{
	if (IsAllGreyed())
		return;

	// Set up the print again so that it takes the new settings on board
	LocalPrintControl.SetUp(pDocument->GetSelectedSpread());

	if (TalkToPage())
		ShowDetails();
}


/********************************************************************************************

>	BOOL PrintBaseTab::HandleMsg(DialogMsg* Msg)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/96
	Inputs:		Msg - The dialogue message to be handled
	Returns:	TRUE

	Purpose:	Handles any print options tabs messages that the derived class is not
				interested in handlign themselves. Should be called by the derived class
				for any events they don't handle

				Currently handles:
				DIM_CREATE - Calls InitSection() and then ShowDetails()
				
				DIM_CANCEL - vapes the cached PrintControl info, so that cancel really
							 does cancel!

********************************************************************************************/

BOOL PrintBaseTab::HandleMsg(DialogMsg* Msg)
{
	ERROR2IF(Msg == NULL,FALSE,"PrintGeneralTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintGeneralTab::HandleMsg called with no dialog pointer");

	BOOL ok = TalkToPage(); // The print general tab identifier
	if (!ok)
		return TRUE;		// page not present

	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
			ok = InitSection();
			if (!ok)
				InformError();
			else
				ShowDetails();	// The base class InitSection() no longer does this by default
			break;


		case DIM_CANCEL:
		{
			// Vape the cached print control info
			pLastPrintControlDocument = NULL;

			// Clear out the entire print control to sensible defaults. This includes wiping the
			// plate list, which ensures there are no IndexedColours left "in use" when we exit
			PrintControl BlankPC;
			LocalPrintControl = BlankPC;
			break;
		}

		default:
			break;
	}

	return TRUE;
}  



/******************************************************************************************

>	BOOL PrintBaseTab::CommitSection()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/96
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Commits values from the print dialogue tabs to the document.
				Should be called by all derived classes
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintBaseTab::CommitSection()
{
	if (!IsAllGreyed() && pDocument != NULL)
		CopyLocalPrintControlToDoc();

	pLastPrintControlDocument = NULL;

	return(TRUE);
}



//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------


//------------------------------------------------

/********************************************************************************************

>	PrintGeneralTab::PrintGeneralTab() 


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintGeneralTab constructor.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PrintGeneralTab::PrintGeneralTab()
{
}        

/********************************************************************************************

>	PrintGeneralTab::~PrintGeneralTab()


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintGeneralTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
PrintGeneralTab::~PrintGeneralTab()
{   
}        

CDlgResID PrintGeneralTab::GetPageID()	{ return _R(IDD_OPTSTAB_PRINTGENERAL); }

/********************************************************************************************

>	BOOL PrintGeneralTab::HandleMsg(DialogMsg* Msg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the print general options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PrintGeneralTab::HandleMsg(DialogMsg* Msg)
{
	ERROR2IF(Msg == NULL,FALSE,"PrintGeneralTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintGeneralTab::HandleMsg called with no dialog pointer");

	BOOL ok = TalkToPage(); // The print general tab identifier
	if (!ok)
		return TRUE;		// page not present

	switch(Msg->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
			OptionsTabs::SetApplyNowState(TRUE);
			if (FALSE) {}
			// Print layers
			else if (Msg->GadgetID == _R(IDC_ALLFOREGROUND))
			{
				LocalPrintControl.SetPrintLayers(PRINTLAYERS_ALLFOREGROUND);
			}
			else if (Msg->GadgetID == _R(IDC_VISIBLEFOREGROUND))
			{
				LocalPrintControl.SetPrintLayers(PRINTLAYERS_VISIBLEFOREGROUND);
			}
#if 0
			// Job 10463: remove PS Level bits - default to Level 2
			// Postscript language level
			else if (Msg->GadgetID == _R(IDC_PSLEVELAUTO))
			{
				LocalPrintControl.SetPSLevel(PSLEVEL_AUTO);
			}
			else if (Msg->GadgetID == _R(IDC_PSLEVEL1))
			{
				LocalPrintControl.SetPSLevel(PSLEVEL_1);
			}
			else if (Msg->GadgetID == _R(IDC_PSLEVEL2))
			{
				LocalPrintControl.SetPSLevel(PSLEVEL_2);
			}
#endif
			// Print method
			else if (Msg->GadgetID == _R(IDC_PRINTMETHOD_NORMAL))
			{
				LocalPrintControl.SetPrintMethod(PRINTMETHOD_NORMAL);
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_PRINTMETHOD_BITMAP))
			{
				LocalPrintControl.SetPrintMethod(PRINTMETHOD_BITMAP);
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_PRINTMETHOD_AABITMAP))
			{
				LocalPrintControl.SetPrintMethod(PRINTMETHOD_AABITMAP);
				ShowDetails();
			}

			// Bitmap DPI
			else if (Msg->GadgetID == _R(IDC_DPIAUTO))
			{
				LocalPrintControl.SetBitmapResMethod(BITMAPRES_AUTO);
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_DPIMANUAL))
			{
				LocalPrintControl.SetBitmapResMethod(BITMAPRES_MANUAL);
				ShowDetails();
			}
			// Text options
			else if (Msg->GadgetID == _R(IDC_ALLTEXTASSHAPES))
			{
				PrintTextOptions pto = LocalPrintControl.GetTextOptions();
				pto = (pto == PRINTTEXTOPTIONS_NORMAL) ? PRINTTEXTOPTIONS_ALLTEXTASSHAPES
														: PRINTTEXTOPTIONS_NORMAL;
				LocalPrintControl.SetTextOptions(pto);
				ShowDetails();
			}

			break; 

		case DIM_SELECTION_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			if (Msg->GadgetID == _R(IDC_FILLQUALLIST))
			{
				WORD Index;
				pPrefsDlg->GetValueIndex(_R(IDC_FILLQUALLIST),&Index); 
				LocalPrintControl.SetFillQuality(PrintFillQuality(Index));
				ShowDetails();
			}
			break;

		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			if (Msg->GadgetID == _R(IDC_DPIEDIT))
			{
				INT32 DPI = pPrefsDlg->GetLongGadgetValue(_R(IDC_DPIEDIT),0,0);
				LocalPrintControl.SetDotsPerInch(DPI);
			}
			break;

		default:
			return(PrintBaseTab::HandleMsg(Msg));
	}

	return TRUE;
}  

/******************************************************************************************

>	BOOL PrintGeneralTab::CommitSection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Print General section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintGeneralTab::CommitSection()
{
	if (IsAllGreyed() || pDocument == NULL)
		return(PrintBaseTab::CommitSection());

	ERROR3IF(pPrefsDlg == NULL, "PrintGeneralTab::CommitSection called with no dialog pointer");

	if (!TalkToPage())
		return TRUE;	// Just return if TalkToPage() fails

/*
	//---
	// RANGE CHECK ALL VALUES TO MAKE SURE THEY ARE SENSIBLE
	//---
	
	PrintControl* pPrCtrl = GetPrintControl();
	ERROR2IF(pPrCtrl == NULL,TRUE,"The doc's print component gave me a NULL print control object");

	// We only want to change the values of the doc print control as if we are importing the values
	// from a file, i.e. we don't want other values changing as a result of these calls
	pPrCtrl->StartImport();

	// Only set the members this tab has changed.
|*	pPrCtrl->SetPrintLayers(	LocalPrintControl.GetPrintLayers());
	pPrCtrl->SetPSLevel(		LocalPrintControl.GetPSLevel());
	pPrCtrl->SetPrintMethod(	LocalPrintControl.GetPrintMethod());
	pPrCtrl->SetBitmapResMethod(LocalPrintControl.GetBitmapResMethod());
	pPrCtrl->SetDotsPerInch(	LocalPrintControl.GetDotsPerInch());
	pPrCtrl->SetFillQuality(	LocalPrintControl.GetFillQuality());
	pPrCtrl->SetTextOptions(	LocalPrintControl.GetTextOptions());
*|

	CGadgetID LayerGadgets[] = { _R(IDC_ALLFOREGROUND), _R(IDC_VISIBLEFOREGROUND), NULL };
	BOOL Result = (pPrefsDlg->GetRadioGroupSelected(LayerGadgets) == _R(IDC_ALLFOREGROUND));
	pPrCtrl->SetPrintLayers((Result) ? PRINTLAYERS_ALLFOREGROUND : PRINTLAYERS_VISIBLEFOREGROUND);

	CGadgetID PSGadgets[] = { _R(IDC_PSLEVELAUTO), _R(IDC_PSLEVEL1), _R(IDC_PSLEVEL2), NULL };
	ResourceID PSRadio = pPrefsDlg->GetRadioGroupSelected(PSGadgets);
	if (PSRadio == _R(IDC_PSLEVELAUTO))
		pPrCtrl->SetPSLevel(PSLEVEL_AUTO);
	else if (PSRadio == _R(IDC_PSLEVEL1))
		pPrCtrl->SetPSLevel(PSLEVEL_1);
	else if (PSRadio == _R(IDC_PSLEVEL2))
		pPrCtrl->SetPSLevel(PSLEVEL_2);

	CGadgetID MethodGadgets[] = { _R(IDC_PRINTMETHOD_NORMAL), _R(IDC_PRINTMETHOD_BITMAP), _R(IDC_PRINTMETHOD_AABITMAP), NULL };
	ResourceID MethodRadio = GetRadioGroupSelected(MethodGadgets));
	if (MethodRadio == _R(IDC_PRINTMETHOD_NORMAL))
		pPrCtrl->SetPrintMethod(PRINTMETHOD_NORMAL);
	else if (MethodRadio == _R(IDC_PRINTMETHOD_BITMAP))
		pPrCtrl->SetPrintMethod(PRINTMETHOD_BITMAP);
	else if (MethodRadio == _R(IDC_PRINTMETHOD_AABITMAP))
		pPrCtrl->SetPrintMethod(PRINTMETHOD_AABITMAP);

	CGadgetID DPIGadgets[] = { _R(IDC_DPIAUTO), _R(IDC_DPIMANUAL), NULL };
	Result = (pPrefsDlg->GetRadioGroupSelected(DPIGadgets) == _R(IDC_DPIAUTO));
	pPrCtrl->SetBitmapResMethod((Result) ? BITMAPRES_AUTO : BITMAPRES_MANUAL);

	Result = pPrefsDlg->GetBoolGadgetSelected(_R(IDC_ALLTEXTASSHAPES));
	pPrCtrl->SetTextOptions((Result) ? PRINTTEXTOPTIONS_ALLTEXTASSHAPES : PRINTTEXTOPTIONS_NORMAL);

	WORD Index;
	pPrefsDlg->GetValueIndex(_R(IDC_FILLQUALLIST), &Index); 
	pPrCtrl->SetFillQuality(PrintFillQuality(Index));

	INT32 DPI = pPrefsDlg->GetLongGadgetValue(_R(IDC_DPIEDIT),0,0);
	pPrCtrl->SetDotsPerInch(DPI);
 
	// Finished making the changes.
	pPrCtrl->EndImport();
*/
	return(PrintBaseTab::CommitSection());
}


/******************************************************************************************

>	BOOL PrintGeneralTab::ShowDetails()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets values for the print general section of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PrintGeneralTab::ShowDetails()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintGeneralTab::ShowDetailscalled with no dialog pointer");

	String_256 Str;

	// The print layers group
	PrintLayers	pl = LocalPrintControl.GetPrintLayers();
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_ALLFOREGROUND),	pl == PRINTLAYERS_ALLFOREGROUND);
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_VISIBLEFOREGROUND),pl == PRINTLAYERS_VISIBLEFOREGROUND);

	/*
	// Job 10463: remove PS Level bits - default to Level 2
	// Postscript language level
	PSLevel	PSLvl = LocalPrintControl.GetPSLevel();
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_PSLEVELAUTO),PSLvl == PSLEVEL_AUTO);
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_PSLEVEL1),	 PSLvl == PSLEVEL_1);
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_PSLEVEL2),	 PSLvl == PSLEVEL_2);
	*/

	// Print method
	PrintMethodType	pm = LocalPrintControl.GetPrintMethod();
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_PRINTMETHOD_NORMAL),	pm == PRINTMETHOD_NORMAL);
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_PRINTMETHOD_BITMAP),	pm == PRINTMETHOD_BITMAP);
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_PRINTMETHOD_AABITMAP),	pm == PRINTMETHOD_AABITMAP);

	if (CCPrintDialog::IsPostscript())
		Str.Load(_R(IDS_K_OPTSPRIN_POSTSCIPT));
	else
		Str.Load(_R(IDS_K_OPTSPRIN_NORMAL));

	pPrefsDlg->SetStringGadgetValue(_R(IDC_PRINTMETHOD_NORMAL),Str);

	// Bitmap resolution
	BitmapResMethod brm = LocalPrintControl.GetBitmapResMethod();
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_DPIAUTO),  brm == BITMAPRES_AUTO);
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_DPIMANUAL),brm == BITMAPRES_MANUAL);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_DPIEDIT), LocalPrintControl.GetDotsPerInch());

	// The bitmap resolution group text
	if (pm == PRINTMETHOD_NORMAL) 	Str.Load(_R(IDS_K_OPTSPRIN_TRANSRES));
	if (pm == PRINTMETHOD_BITMAP) 	Str.Load(_R(IDS_K_OPTSPRIN_BITMAPRES));
	if (pm == PRINTMETHOD_AABITMAP) Str.Load(_R(IDS_K_OPTSPRIN_AABITMAPRES));
	pPrefsDlg->SetStringGadgetValue(_R(IDC_GROUP_DPI),Str);

	//  Fill quality group
	pPrefsDlg->SetSelectedValueIndex(_R(IDC_FILLQUALLIST),INT32(LocalPrintControl.GetFillQuality()));

	// Text options
	PrintTextOptions pto = LocalPrintControl.GetTextOptions();
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_ALLTEXTASSHAPES), pto != PRINTTEXTOPTIONS_NORMAL);

	// All done!
	return EnableControls();
}

/******************************************************************************************

>	BOOL PrintGeneralTab::EnableControls()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all went well
	Purpose:	The enables and disables the various controls depending on the values held
				in the given print control object
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintGeneralTab::EnableControls()
{
	if (pPrefsDlg == NULL) return TRUE;

	if (!TalkToPage())
		return(TRUE);

	// General controls
	EnableControl(_R(IDC_PRINT_DOCNAME),	 TRUE);
	EnableControl(_R(IDC_OUTPUT_DOCNAMETEXT),TRUE);

	// The layers group
	EnableControl(_R(IDC_GROUP_PRINTLAYERS),TRUE);
	EnableControl(_R(IDC_ALLFOREGROUND), 	TRUE);
	EnableControl(_R(IDC_VISIBLEFOREGROUND),TRUE);

	// postscript level group
	BOOL IsPostcript = CCPrintDialog::IsPostscript();
	/*
	// Job 10463: remove PS Level bits - default to Level 2
	EnableControl(_R(IDC_GROUP_PSLEVEL),IsPostcript);
	EnableControl(_R(IDC_PSLEVELAUTO),	IsPostcript);
	EnableControl(_R(IDC_PSLEVEL1), 	IsPostcript);
	EnableControl(_R(IDC_PSLEVEL2), 	IsPostcript);
	*/

	// print method group (print as...)
	EnableControl(_R(IDC_GROUP_PRINTMETHOD),	TRUE);
	EnableControl(_R(IDC_PRINTMETHOD_NORMAL),	TRUE);
	EnableControl(_R(IDC_PRINTMETHOD_BITMAP),	TRUE);
	EnableControl(_R(IDC_PRINTMETHOD_AABITMAP),	TRUE);

	// Bmp DPI group
	EnableControl(_R(IDC_GROUP_DPI),	TRUE);
	EnableControl(_R(IDC_DPIAUTO),		TRUE);
	EnableControl(_R(IDC_DPIMANUAL),	TRUE);
	EnableControl(_R(IDC_DPIEDIT),		(LocalPrintControl.GetBitmapResMethod() == BITMAPRES_MANUAL));
	EnableControl(_R(IDC_DPIEDITTEXT),	(LocalPrintControl.GetBitmapResMethod() == BITMAPRES_MANUAL));

	// Fill quality group
	BOOL Normal = (LocalPrintControl.GetPrintMethod() == PRINTMETHOD_NORMAL);
	EnableControl(_R(IDC_GROUP_FILLQUAL),	Normal && (!IsPostcript));
	EnableControl(_R(IDC_FILLQUALTEXT),		Normal && (!IsPostcript));
	EnableControl(_R(IDC_FILLQUALLIST),		Normal && (!IsPostcript));

	// Text options
	EnableControl(_R(IDC_GROUP_TEXTOPTIONS), TRUE);
	EnableControl(_R(IDC_ALLTEXTASSHAPES),   TRUE);

	return TRUE;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

/********************************************************************************************

>	PrintLayoutTab::PrintLayoutTab() 


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintLayoutTab constructor.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PrintLayoutTab::PrintLayoutTab()
{   
}        

/********************************************************************************************

>	PrintLayoutTab::~PrintLayoutTab()


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintLayoutTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
PrintLayoutTab::~PrintLayoutTab()
{   
}        

CDlgResID PrintLayoutTab::GetPageID()	{ return _R(IDD_OPTSTAB_PRINTLAYOUT); }

/********************************************************************************************

>	BOOL PrintLayoutTab::HandleMsg(DialogMsg* Msg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the print layout options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PrintLayoutTab::HandleMsg(DialogMsg* Msg)
{
	ERROR2IF(Msg == NULL,FALSE,"PrintLayoutTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintLayoutTab::HandleMsg called with no dialog pointer");

	BOOL ok = TalkToPage();	// The print layout tab identifier
	if (!ok)
		return TRUE;		// page not present

	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
			ok = InitSection();
			if (!ok)
				InformError();
			break;

		case DIM_LFT_BN_CLICKED:
			OptionsTabs::SetApplyNowState(TRUE);

			if (FALSE) {}
			// Orientation
			else if (Msg->GadgetID == _R(IDC_BTN_UPRIGHT))
			{
				LocalPrintControl.SetPrintOrient(PRINTORIENTATION_UPRIGHT);
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_BTN_SIDEWAYS))
			{
				LocalPrintControl.SetPrintOrient(PRINTORIENTATION_SIDEWAYS);
				ShowDetails();
			}
			// DPS buttons
			else if (Msg->GadgetID == _R(IDC_DPS_INDIVIDUALPAGES))
			{
				LocalPrintControl.SetWholeSpread(FALSE);
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_DPS_WHOLESPREAD))
			{
				LocalPrintControl.SetWholeSpread(TRUE);
				ShowDetails();
			}
			// Fit type
			else if (Msg->GadgetID == _R(IDC_BESTFIT))
			{
				LocalPrintControl.SetFitType(PRINTFIT_BEST);
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_CUSTOMFIT))
			{
				LocalPrintControl.SetFitType(PRINTFIT_CUSTOM);
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_MULTIPLEFIT))
			{
				LocalPrintControl.SetFitType(PRINTFIT_MULTIPLE);
				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_BESTFITPAPER))
			{
				LocalPrintControl.SetFitType(PRINTFIT_BESTPAPER);
				ShowDetails();
			}
			break;

		case DIM_SELECTION_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			break;

		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);

			if (!IgnoreTextChangedMsgs)
			{
				if (FALSE) {}
				// Scale ed field
				else if (Msg->GadgetID == _R(IDC_SCALEEDIT))
				{
					double Scale = pPrefsDlg->GetDoubleGadgetValue(_R(IDC_SCALEEDIT),0,0);
					// We need to range check the value before converting it to a Fixed 16
					if (Scale > 32000.0) Scale = 32000.0;
					if (Scale < 0)		 Scale = 0.0;
					LocalPrintControl.SetScale(FIXED16(Scale));
					ShowDetails(NULL,_R(IDC_SCALEEDIT));
				}
				// Custom fit ed fields
				else if (Msg->GadgetID == _R(IDC_TOPEDIT))
				{
					LocalPrintControl.SetTopMargin(pPrefsDlg->GetUnitGadgetValue(_R(IDC_TOPEDIT),CurrentPageUnits,0,0));
					ShowDetails(NULL,_R(IDC_TOPEDIT));
				}
				else if (Msg->GadgetID == _R(IDC_LEFTEDIT))
				{
					LocalPrintControl.SetLeftMargin(pPrefsDlg->GetUnitGadgetValue(_R(IDC_LEFTEDIT),CurrentPageUnits,0,0));
					ShowDetails(NULL,_R(IDC_LEFTEDIT));
				}
				else if (Msg->GadgetID == _R(IDC_WIDTHEDIT))
				{
					LocalPrintControl.SetWidth(pPrefsDlg->GetUnitGadgetValue(_R(IDC_WIDTHEDIT),CurrentPageUnits,0,0));
					ShowDetails(NULL,_R(IDC_WIDTHEDIT));
				}
				else if (Msg->GadgetID == _R(IDC_HEIGHTEDIT))
				{
					LocalPrintControl.SetHeight(pPrefsDlg->GetUnitGadgetValue(_R(IDC_HEIGHTEDIT),CurrentPageUnits,0,0));
					ShowDetails(NULL,_R(IDC_HEIGHTEDIT));
				}
				// Multiple fit ed fields
				else if (Msg->GadgetID == _R(IDC_ROWSEDIT))
				{
					LocalPrintControl.SetRows(pPrefsDlg->GetLongGadgetValue(_R(IDC_ROWSEDIT),0,0));
					ShowDetails(NULL,_R(IDC_ROWSEDIT));
				}
				else if (Msg->GadgetID == _R(IDC_COLUMNSEDIT))
				{
					LocalPrintControl.SetColumns(pPrefsDlg->GetLongGadgetValue(_R(IDC_COLUMNSEDIT),0,0));
					ShowDetails(NULL,_R(IDC_COLUMNSEDIT));
				}
				else if (Msg->GadgetID == _R(IDC_GUTTEREDIT))
				{
					LocalPrintControl.SetGutter(pPrefsDlg->GetUnitGadgetValue(_R(IDC_GUTTEREDIT),CurrentPageUnits,0,0));
					ShowDetails(NULL,_R(IDC_GUTTEREDIT));
				}
			}
			break;

		case DIM_KILL_FOCUS:
			{
				CGadgetID List[] = { 0, 0 };
				// Edit fields
				if ((Msg->GadgetID == _R(IDC_SCALEEDIT)) ||
					(Msg->GadgetID == _R(IDC_ROWSEDIT)) ||
					(Msg->GadgetID == _R(IDC_COLUMNSEDIT)) ||
					(Msg->GadgetID == _R(IDC_GUTTEREDIT)) ||
					(Msg->GadgetID == _R(IDC_TOPEDIT)) ||
					(Msg->GadgetID == _R(IDC_LEFTEDIT)) ||
					(Msg->GadgetID == _R(IDC_WIDTHEDIT)) ||
					(Msg->GadgetID == _R(IDC_HEIGHTEDIT)) )
				{
					List[0] = Msg->GadgetID;
					ShowDetails(List, 0);
				}
			}
			break;

		default:
			return(PrintBaseTab::HandleMsg(Msg));
	}		

#ifdef _DEBUG
	if (Msg->DlgMsg == DIM_LFT_BN_CLICKED &&
		((Msg->GadgetID == _R(IDC_BESTFIT)) || (Msg->GadgetID == _R(IDC_CUSTOMFIT)) || (Msg->GadgetID == _R(IDC_MULTIPLEFIT))) &&
		KeyPress::IsConstrainPressed())
	{
		PrintLayoutTab::TestPrinting(&LocalPrintControl);
	}
#endif

	return TRUE;
}



/******************************************************************************************

>	BOOL PrintLayoutTab::CommitSection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Print Layout section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintLayoutTab::CommitSection()
{
	if (IsAllGreyed() || pDocument == NULL)
		return(PrintBaseTab::CommitSection());

	ERROR3IF(pPrefsDlg == NULL, "PrintLayoutTab::CommitSection called with no dialog pointer");

	if (!TalkToPage())
		return TRUE;	// Just return if TalkToPage() fails

/*

	//---
	// RANGE CHECK ALL VALUES TO MAKE SURE THEY ARE SENSIBLE
	//---
	
	PrintControl* pPrCtrl = GetPrintControl();
	ERROR2IF(pPrCtrl == NULL,TRUE,"The doc's print component gave me a NULL print control object");

	// We only want to change the values of the doc print control as if we are importing the values
	// from a file, i.e. we don't want other values changing as a result of these calls
	pPrCtrl->StartImport();
|*
	// Only set the members this tab has changed.
	pPrCtrl->SetPrintOrient(LocalPrintControl.GetPrintOrient());
	pPrCtrl->SetWholeSpread(LocalPrintControl.IsWholeSpread());
	pPrCtrl->SetFitType(	LocalPrintControl.GetFitType());
	pPrCtrl->SetScale(		LocalPrintControl.GetScale());
	pPrCtrl->SetTopMargin(	LocalPrintControl.GetTopMargin());
	pPrCtrl->SetLeftMargin(	LocalPrintControl.GetLeftMargin());
	pPrCtrl->SetWidth(		LocalPrintControl.GetWidth());
	pPrCtrl->SetHeight(		LocalPrintControl.GetHeight());
	pPrCtrl->SetRows(		LocalPrintControl.GetRows());
	pPrCtrl->SetColumns(	LocalPrintControl.GetColumns());
	pPrCtrl->SetGutter(		LocalPrintControl.GetGutter());
*|

	// Read the docuemnt vaules directly out of the dialogue
	CGadgetID OrientGadgets[] = { _R(IDC_BTN_UPRIGHT), _R(IDC_BTN_SIDEWAYS), NULL };
	BOOL Result = (pPrefsDlg->GetRadioGroupSelected(OrientGadgets) == _R(IDC_BTN_UPRIGHT));
	pPrCtrl->SetPrintOrient((Result) ? PRINTORIENTATION_UPRIGHT : PRINTORIENTATION_SIDEWAYS);

	CGadgetID DPSGadgets[] = { _R(IDC_DPS_INDIVIDUALPAGES), _R(IDC_DPS_WHOLESPREAD), NULL };
	Result = (pPrefsDlg->GetRadioGroupSelected(DPSGadgets) == _R(IDC_DPS_WHOLESPREAD));
	pPrCtrl->SetWholeSpread(Result);

	CGadgetID FitGadgets[] = { _R(IDC_BESTFIT), _R(IDC_CUSTOMFIT), _R(IDC_MULTIPLEFIT), NULL };
	ResourceID FitRadio = GetRadioGroupSelected(FitGadgets))
	if (FitRadio == _R(IDC_BESTFIT))
		pPrCtrl->SetFitType(PRINTFIT_BEST);
	else if (FitRadio == _R(IDC_CUSTOMFIT))
		pPrCtrl->SetFitType(PRINTFIT_CUSTOM);
	else if (FitRadio == _R(IDC_MULTIPLEFIT))
		pPrCtrl->SetFitType(PRINTFIT_MULTIPLE);
	
	double Scale = pPrefsDlg->GetDoubleGadgetValue(_R(IDC_SCALEEDIT),0,0);
	// We need to range check the value before converting it to a Fixed 16
	if (Scale > 32000.0) Scale = 32000.0;
	if (Scale < 0)		 Scale = 0.0;
	pPrCtrl->SetScale(FIXED16(Scale));

	pPrCtrl->SetTopMargin(pPrefsDlg->GetUnitGadgetValue(_R(IDC_TOPEDIT),CurrentPageUnits,0,0));
	pPrCtrl->SetLeftMargin(pPrefsDlg->GetUnitGadgetValue(_R(IDC_LEFTEDIT),CurrentPageUnits,0,0));
	pPrCtrl->SetWidth(pPrefsDlg->GetUnitGadgetValue(_R(IDC_WIDTHEDIT),CurrentPageUnits,0,0));
	pPrCtrl->SetHeight(pPrefsDlg->GetUnitGadgetValue(_R(IDC_HEIGHTEDIT),CurrentPageUnits,0,0));
	pPrCtrl->SetRows(pPrefsDlg->GetLongGadgetValue(_R(IDC_ROWSEDIT),0,0));
	pPrCtrl->SetColumns(pPrefsDlg->GetLongGadgetValue(_R(IDC_COLUMNSEDIT),0,0));
	pPrCtrl->SetGutter(pPrefsDlg->GetUnitGadgetValue(_R(IDC_GUTTEREDIT),CurrentPageUnits,0,0));

	// Finished making the changes.
	pPrCtrl->EndImport();

	// Set up the print again so that it takes the new settings on board
	pPrCtrl->SetUp(pDocument->GetSelectedSpread());
*/

	// Redraw all views that are showing their print borders
	FRDocViewFunc pFunc = PrintControl::RedrawPrintableArea;
	pDocument->ForceRedraw(pFunc);

	return(PrintBaseTab::CommitSection());
}


/******************************************************************************************

>	BOOL PrintLayoutTab::InitSection()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the print layout section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PrintLayoutTab::InitSection()
{
	if (PrintBaseTab::InitSection())
	{
		ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintLayoutTab::InitSection called with no dialog pointer");

		pPrefsDlg->SetGadgetBitmaps(_R(IDC_BTN_SIDEWAYS),0,0);
		pPrefsDlg->SetGadgetBitmaps(_R(IDC_BTN_UPRIGHT),0,0);

		// Set up the units that are used in the currently selected document
		// All units work off the selected document and so we must do nothing if there is no
		// selected document.
		if (!SetUpDocUnits())
		{
			// If returns False then we are in an invalid situation and so should make sure that
			// the dialog box is greyed.
			GreySection();
			return (TRUE);
		}

		return ShowDetails();
	}
	else
		return FALSE;
}

/******************************************************************************************

>	BOOL PrintLayoutTab::ShowDetails()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		pGadgetIDList = ptr to gadgets to show details in (NULL means all gadgets)
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets values for the print layout section of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs; ShowDetails(CGadgetID* pGadgetIDList,CGadgetID Exclude)

******************************************************************************************/

BOOL PrintLayoutTab::ShowDetails()
{
	return ShowDetails(NULL, 0);
}

/******************************************************************************************

>	BOOL PrintLayoutTab::ShowDetails(CGadgetID* pGadgetIDList,CGadgetID Exclude)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pGadgetIDList 	= ptr to gadgets to show details in (NULL means all gadgets)
				Exclude			= gadget to exclude in list
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets values for the print layout section of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PrintLayoutTab::ShowDetails(CGadgetID* pGadgetIDList, CGadgetID Exclude)
{
	if (Document::GetSelected() == NULL)	// No document around! Eek!
		return(FALSE);

	IgnoreTextChangedMsgs = TRUE;

	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintLayoutTab::ShowDetailscalled with no dialog pointer");

	// A list of all the gadgets in which we display details in
	static CGadgetID pAllGadgets[] ={	_R(IDC_LAYOUTINFO),_R(IDC_PAPERSIZEINFO),_R(IDC_DPS_WHOLESPREAD),
										_R(IDC_BTN_UPRIGHT),_R(IDC_SCALEEDIT),_R(IDC_BESTFIT),
										_R(IDC_TOPEDIT),_R(IDC_LEFTEDIT),_R(IDC_WIDTHEDIT),_R(IDC_HEIGHTEDIT),
										_R(IDC_ROWSEDIT),_R(IDC_COLUMNSEDIT),_R(IDC_GUTTEREDIT),
										0 };

	// If no gadget list is given, fall back to the list of all gadgets
	if (pGadgetIDList == NULL)
		pGadgetIDList = pAllGadgets;

	BOOL ShowMargins = 	(	(LocalPrintControl.GetFitType() != PRINTFIT_MULTIPLE) || 
							((LocalPrintControl.GetRows() == 1) && (LocalPrintControl.GetColumns() == 1))
					   	);

	String_256 Str;
	String_256 Dash = "-";
	
	for (INT32 i=0; pGadgetIDList[i]; i++)
	{
		// Ignore the the exclude gadget
		if (pGadgetIDList[i] == Exclude)
			continue;

		if (FALSE) {}
		else if (pGadgetIDList[i] == _R(IDC_LAYOUTINFO))
		{
			// Display that print info
			Str = LocalPrintControl.BuildPrintInfoStr();
			pPrefsDlg->SetStringGadgetValue(_R(IDC_LAYOUTINFO),Str);
		}
		else if (pGadgetIDList[i] == _R(IDC_PAPERSIZEINFO))
		{
			// Display the paper size
			Str = LocalPrintControl.BuildPaperSizeStr();
			pPrefsDlg->SetStringGadgetValue(_R(IDC_PAPERSIZEINFO),Str);
		}
		else if (pGadgetIDList[i] == _R(IDC_DPS_WHOLESPREAD))
		{
			// Reflect the Whole Spread flag
			BOOL WholeSpread = LocalPrintControl.IsWholeSpread();
			pPrefsDlg->SetBoolGadgetSelected(_R(IDC_DPS_WHOLESPREAD),WholeSpread);
			pPrefsDlg->SetBoolGadgetSelected(_R(IDC_DPS_INDIVIDUALPAGES),!WholeSpread);
		}
		else if (pGadgetIDList[i] == _R(IDC_BTN_UPRIGHT))
		{
			// Set the orientation bitmap buttons
			BOOL Upright = (LocalPrintControl.GetPrintOrient() == PRINTORIENTATION_UPRIGHT);
			pPrefsDlg->SetBoolGadgetSelected(_R(IDC_BTN_UPRIGHT),Upright);
			pPrefsDlg->SetBoolGadgetSelected(_R(IDC_BTN_SIDEWAYS),!Upright);
		}
		else if (pGadgetIDList[i] == _R(IDC_SCALEEDIT))	
		{
			// Set the scale factor field
			FIXED16 Scale = LocalPrintControl.GetScale();
			pPrefsDlg->SetDoubleGadgetValue(_R(IDC_SCALEEDIT),Scale.MakeDouble());
		}
		else if (pGadgetIDList[i] == _R(IDC_BESTFIT))
		{
			// The fit radio buttons
			CGadgetID FitList[] = { _R(IDC_BESTFIT),_R(IDC_CUSTOMFIT),_R(IDC_MULTIPLEFIT),_R(IDC_BESTFITPAPER), 0 };

			switch (LocalPrintControl.GetFitType())
			{
				case PRINTFIT_BEST:		pPrefsDlg->SetRadioGroupSelected(FitList,_R(IDC_BESTFIT));		break;
				case PRINTFIT_CUSTOM:	pPrefsDlg->SetRadioGroupSelected(FitList,_R(IDC_CUSTOMFIT));	break;
				case PRINTFIT_MULTIPLE:	pPrefsDlg->SetRadioGroupSelected(FitList,_R(IDC_MULTIPLEFIT));	break;
				case PRINTFIT_BESTPAPER:pPrefsDlg->SetRadioGroupSelected(FitList,_R(IDC_BESTFITPAPER));	break;
				default: ERROR2(FALSE,"Unknown print fit type");
			}
		}
		else if (pGadgetIDList[i] == _R(IDC_TOPEDIT))
		{
			// Custom fit editable values
			if (ShowMargins)
				pPrefsDlg->SetUnitGadgetValue(_R(IDC_TOPEDIT),	CurrentPageUnits, LocalPrintControl.GetTopMargin());
			else
				pPrefsDlg->SetStringGadgetValue(_R(IDC_TOPEDIT), Dash);
		}
		else if (pGadgetIDList[i] == _R(IDC_LEFTEDIT))
		{	
			// Custom fit editable values
			if (ShowMargins)
				pPrefsDlg->SetUnitGadgetValue(_R(IDC_LEFTEDIT),	CurrentPageUnits, LocalPrintControl.GetLeftMargin());
			else
				pPrefsDlg->SetStringGadgetValue(_R(IDC_LEFTEDIT), Dash);
		}
		else if (pGadgetIDList[i] == _R(IDC_WIDTHEDIT))
		{
			pPrefsDlg->SetUnitGadgetValue(_R(IDC_WIDTHEDIT),CurrentPageUnits, LocalPrintControl.GetWidth());
		}
		else if (pGadgetIDList[i] == _R(IDC_HEIGHTEDIT))
		{
			pPrefsDlg->SetUnitGadgetValue(_R(IDC_HEIGHTEDIT),CurrentPageUnits, LocalPrintControl.GetHeight());
		}
		else if (pGadgetIDList[i] == _R(IDC_ROWSEDIT))
		{
			pPrefsDlg->SetLongGadgetValue(_R(IDC_ROWSEDIT),	LocalPrintControl.GetRows());
		}
		else if (pGadgetIDList[i] == _R(IDC_COLUMNSEDIT))
		{
			pPrefsDlg->SetLongGadgetValue(_R(IDC_COLUMNSEDIT),LocalPrintControl.GetColumns());
		}
		else if (pGadgetIDList[i] == _R(IDC_GUTTEREDIT))
		{
			pPrefsDlg->SetUnitGadgetValue(_R(IDC_GUTTEREDIT),CurrentPageUnits, LocalPrintControl.GetGutter());
		}
		else
		{
			ERROR3_PF(("Unknown gadget ID (%D)",pGadgetIDList));
		}
	}

	BOOL ok = EnableControls();

	IgnoreTextChangedMsgs = FALSE;

	return ok;
}

/******************************************************************************************

>	BOOL PrintLayoutTab::EnableControls()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all went well
	Purpose:	The enables and disables the various controls depending on the values held
				in the given print control object
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintLayoutTab::EnableControls()
{
	if (pPrefsDlg == NULL) return TRUE;

	if (!TalkToPage())
		return(TRUE);

//	BOOL BestFit 	 = (LocalPrintControl.GetFitType() == PRINTFIT_BEST);
	BOOL CustomFit 	 = (LocalPrintControl.GetFitType() == PRINTFIT_CUSTOM);
	BOOL MultipleFit = (LocalPrintControl.GetFitType() == PRINTFIT_MULTIPLE);
//	BOOL BestFitPaper= (LocalPrintControl.GetFitType() == PRINTFIT_BESTPAPER);

	// Controls available all the time
	EnableControl(_R(IDC_BESTFIT),			TRUE);
	EnableControl(_R(IDC_CUSTOMFIT),		TRUE);
	EnableControl(_R(IDC_MULTIPLEFIT),		TRUE);
	EnableControl(_R(IDC_BESTFITPAPER),		TRUE);
	EnableControl(_R(IDC_PRINT_DOCNAME),	TRUE);
	EnableControl(_R(IDC_PRINT_DOCNAMETEXT),TRUE);
	EnableControl(_R(IDC_LAYOUTINFO),		TRUE);
	EnableControl(_R(IDC_PAPERSIZETEXT),	TRUE);
	EnableControl(_R(IDC_PAPERSIZEINFO),	TRUE);

	// Controls available when in custom fit mode
	EnableControl(_R(IDC_TOPEDIT),		CustomFit);
	EnableControl(_R(IDC_LEFTEDIT),		CustomFit);
	EnableControl(_R(IDC_WIDTHEDIT),	CustomFit);
	EnableControl(_R(IDC_HEIGHTEDIT),	CustomFit);
	EnableControl(_R(IDC_TOPTEXT),		CustomFit);
	EnableControl(_R(IDC_LEFTTEXT),		CustomFit);
	EnableControl(_R(IDC_WIDTHTEXT),	CustomFit);
	EnableControl(_R(IDC_HEIGHTTEXT),	CustomFit);
	EnableControl(_R(IDC_SCALEEDIT),	CustomFit);
	EnableControl(_R(IDC_SCALETEXT),	CustomFit);
	EnableControl(_R(IDC_SCALEPERCENT),	CustomFit);

	// Controls available when in multiple fit mode
	EnableControl(_R(IDC_ROWSEDIT),		MultipleFit);
	EnableControl(_R(IDC_COLUMNSEDIT),	MultipleFit);
	EnableControl(_R(IDC_GUTTEREDIT),	MultipleFit);
	EnableControl(_R(IDC_ROWSTEXT),		MultipleFit);
	EnableControl(_R(IDC_COLUMNSTEXT),	MultipleFit);
	EnableControl(_R(IDC_GUTTERTEXT),	MultipleFit);

	// Controls available when in multiple or custom fit mode
	EnableControl(_R(IDC_BTN_UPRIGHT),			CustomFit || MultipleFit);
	EnableControl(_R(IDC_BTN_SIDEWAYS),			CustomFit || MultipleFit);
	EnableControl(_R(IDC_PRINTORIENTATIONTEXT),	CustomFit || MultipleFit);

	// Controls available when the page is a double page spread
	EnableControl(_R(IDC_DPS_WHOLESPREAD),		LocalPrintControl.IsDoublePageSpread());
	EnableControl(_R(IDC_DPS_INDIVIDUALPAGES),	LocalPrintControl.IsDoublePageSpread());

	// If in windows 95, hide the picture boxes as they don't slab in, and go against Win95 look & feel
PORTNOTE("other", "Assume new windows UI")
#ifndef EXCLUDE_FROM_XARALX
	if (CCamApp::IsNewWindowsUI()) //IsWindows95())
#endif
	{
		if (pPrefsDlg != NULL)
		{
			pPrefsDlg->HideGadget(_R(IDC_LAYOUTINFO_BOX),TRUE);
			pPrefsDlg->HideGadget(_R(IDC_PAPERSIZEINFO_BOX),TRUE);
		}
	}

	return TRUE;
}

#ifdef _DEBUG
/******************************************************************************************

>	static void PrintLayoutTab::TestPrinting(PrintControl* pPrCtrl)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		pPrCtrl = ptr to print control to test
	Outputs:	-
	Returns:	-
	Purpose:	This tests the printing loop functions in the print control object
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void PrintLayoutTab::TestPrinting(PrintControl* pPrCtrl)
{
PORTNOTE("printing", "Disable PrintLayoutTab::TestPrinting")
#ifndef EXCLUDE_FROM_XARALX
	if (pPrCtrl != NULL && pPrCtrl->StartPrinting())
	{
		BOOL Aborted = FALSE;
		PrintProgressDlg* pPrgDlg = new PrintProgressDlg(NULL);

		if (pPrgDlg != NULL)
		{
			pPrgDlg->SetDocName(Document::GetCurrent()->GetTitle());
			pPrgDlg->Show();
		}

		time_t CurrentTime = time(NULL);

		while (!Aborted && pPrCtrl->GetNextPaper())
		{
			Layer* pLayer = new Layer;

			PrintPatchInfo PatchInfo;

			SIZEL PaperSize;
			if (!CCPrintDialog::GetPaperSize(&PaperSize))
				return;
			MakeRectangle(pLayer,DocRect(0,0,PaperSize.cx,PaperSize.cy),COLOUR_MIDGREY);

			BOOL DisplayClipRect = TRUE;

			while (!Aborted && pPrCtrl->GetNextPatch(&PatchInfo))
			{
				if (pPrgDlg != NULL)
				{
					pPrgDlg->SetPageNumber(	PatchInfo.PaperNumber,
											PatchInfo.MaxPaperNumber,
											1, 1, NULL,
											PatchInfo.PatchNumber,
											PatchInfo.MaxPatchNumber);

					pPrgDlg->SetSliderSubRangeMax(2000);
				}

				INT32 pos=0;
				while ((time(NULL) - CurrentTime) < 2)
				{
					if (!PrintProgressDlg::AbortProc(0,0))
					{
						Aborted = TRUE;
						break;
					}
					if (pPrgDlg != NULL)
						pPrgDlg->SetSliderSubRangePos(pos++);
				}
				CurrentTime = time(NULL);

				if (DisplayClipRect)
				{
					MakeRectangle(pLayer,PatchInfo.GetClipRect(FALSE,FALSE),COLOUR_GREEN);
					DisplayClipRect = FALSE;
				}

				DocRect Rect = PatchInfo.GetClipRect(FALSE,FALSE);
				double ScaleFactor = PatchInfo.Scale.MakeDouble() / 100;

				INT32 w = Rect.Width();
				INT32 h = Rect.Height();

				if (PatchInfo.Rotate)
				{	INT32 t = w;	w = h;	h = t;	}

				Rect.lox = PatchInfo.XTrans;
				Rect.loy = PatchInfo.YTrans;

				Rect.hix = Rect.lox + (INT32((double(w) * ScaleFactor)+0.5));
				Rect.hiy = Rect.loy + (INT32((double(h) * ScaleFactor)+0.5));

				MakeRectangle(pLayer,Rect,COLOUR_RED);
			}

			if (!Aborted && pLayer != NULL && PatchInfo.pSpread != NULL)
			{
				TCHAR name[256];
				camSprintf(name,_T("Page %d"),PatchInfo.PaperNumber);
				pLayer->SetLayerID(String_256(name));
				pLayer->AttachNode(PatchInfo.pSpread,LASTCHILD);
				pLayer->SetVisible(FALSE);
				LayerSGallery::MakeActiveLayer(pLayer);
			}
		}

		if (pPrgDlg != NULL)
		{
			pPrgDlg->DestroyWindow();
			delete pPrgDlg;
			pPrgDlg = NULL;
		}

		pPrCtrl->EndPrinting();
		Document::GetCurrent()->ForceRedraw();
	}
#endif
}

void PrintLayoutTab::MakeRectangle(Layer* pLayer,DocRect Rect, StockColour Col)
{
	NodeRect* pNodeRect = new NodeRect;
	if (pNodeRect != NULL && pLayer != NULL)
	{
		if (pNodeRect->InkPath.Initialise())
		{
			pNodeRect->CreateShape(Rect);
			pNodeRect->AttachNode(pLayer,LASTCHILD);
			pNodeRect->InkPath.IsFilled = TRUE;
			pNodeRect->InkPath.IsStroked = TRUE;

			DocColour DocCol(Col);
			
			FlatFillAttribute Attr(DocCol);
			NodeAttribute* pAttr = Attr.MakeNode();
			if (pAttr != NULL)
				pAttr->AttachNode(pNodeRect,LASTCHILD);
		}
	}
}

#endif	// _DEBUG


/************************************************************************************************
CLASS PrintSepsTab
 Adrian, 17/06/96
************************************************************************************************/

/************************************************************************************************
  >	PrintSepsTab::PrintSepsTab() 


	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintSepsTab constructor.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PrintSepsTab::PrintSepsTab()
{
	OldPrintSpotsAsProcess = IndexedColour::AreSpotsForcedToProcess();
}


/********************************************************************************************

>	PrintSepsTab::~PrintSepsTab()


	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintSepsTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
PrintSepsTab::~PrintSepsTab()
{ 
}        

CDlgResID PrintSepsTab::GetPageID()	{ return _R(IDD_OPTSTAB_PRINTPRINT); }

/********************************************************************************************

>	BOOL PrintSepsTab::HandleMsg(DialogMsg* Msg)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>/Jason
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles messages sent by various controls on this tab  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PrintSepsTab::HandleMsg(DialogMsg* Msg)
{
	if ((IsAllGreyed() || pDocument == NULL) && Msg->DlgMsg != DIM_CREATE)
		return(TRUE);

	ERROR2IF(Msg == NULL,FALSE,"PrintSepsTab::Message null message received");
	if ((IsAllGreyed() || pDocument == NULL) && Msg->DlgMsg != DIM_CREATE)
		return(TRUE);


	BOOL ok = TalkToPage();
	if (!ok)
		return TRUE;		// page not present

	TypesetInfo *pTypesetInfo = LocalPrintControl.GetTypesetInfo();
	ERROR2IF(pTypesetInfo == NULL, TRUE, "Can't find typesetting info structure");

	switch(Msg->DlgMsg)
	{
		case DIM_CANCEL:
			// If cancelled, we have to restore the previous setting of the "print spot colours as process"
			// flag, which (rather than being local has to be set on a global scale).
			IndexedColour::ForceSpotsToBeProcess(OldPrintSpotsAsProcess);

			// And call the base class to keep it happy.
			return(PrintBaseTab::HandleMsg(Msg));


		case DIM_LFT_BN_CLICKED:
			if (FALSE) {}
			// Separations checkbox
			else if (Msg->GadgetID == _R(IDC_CHECKSEPARATIONS))
			{
				BOOL DoSeparations = pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKSEPARATIONS));
				pTypesetInfo->SetSeparations(DoSeparations);

				// Toggle the OutputPrintersMarks state automatically
				if (pTypesetInfo->OutputPrintersMarks() != DoSeparations)
				{
					pTypesetInfo->SetOutputPrintersMarks(DoSeparations);

					// Make sure the print layout tab updates scale factors etc suitably
					BROADCAST_TO_ALL(PrintMsg(PrintMsg::SETTINGSCHANGED));
				}

				// And let the imagesetting tab know that the seps state has changed
				BROADCAST_TO_ALL(OptionsChangingMsg(OptionsChangingMsg::SEPARATIONSENABLED));

				// Adjust the shading of controls as appropriate, and make sure things like
				// the page size are updated to take printers marks into account
				PageSizeHasChanged();
				OptionsTabs::SetApplyNowState(TRUE);
			}
			else if (Msg->GadgetID == _R(IDC_CHECKPROCESS))
			{
				// If the "print spot colours as process colours" option is changed,
				// interactively update the plate list
				IndexedColour::ForceSpotsToBeProcess(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKPROCESS)));
				ShowDetails();
				OptionsTabs::SetApplyNowState(TRUE);
			}
			else if (Msg->GadgetID == _R(IDC_PRINTERDEFAULTS))
			{
				EnableControl(_R(IDC_COMBORESOLUTION),	FALSE);
				EnableControl(_R(IDC_COMBOFREQUENCY),	FALSE);
				EnableControl(_R(IDC_COMBOTYPE),		FALSE);

				pTypesetInfo->SetScreening(FALSE);		// Disable screening
				pTypesetInfo->ResetAllPlatesToDefaultScreens();
				ShowDetails();							// Make sure colour plate list resets to defaults

				OptionsTabs::SetApplyNowState(TRUE);
			}
			else if (Msg->GadgetID == _R(IDC_CUSTOMSETTINGS))
			{
				EnableControl(_R(IDC_COMBORESOLUTION),	TRUE);
				EnableControl(_R(IDC_COMBOFREQUENCY),	TRUE);
				EnableControl(_R(IDC_COMBOTYPE),		TRUE);

				pTypesetInfo->SetScreening(TRUE);
				OptionsTabs::SetApplyNowState(TRUE);
			}
			else if (Msg->GadgetID == _R(IDC_PLATEPROPS))
			{
				if (pTypesetInfo->AreSeparating())
				{
					String_256 GreyReason;
					OpState DlgState = SepsDlg::GetState(&GreyReason, NULL);

					if (!DlgState.Greyed)
					{
						// Store away the current plate settings in our local print control
						ReadCurrentPlateSettings(pTypesetInfo);

						ColourPlate *pPlate = GetSelectedPlate();
						if (pPlate != NULL)
						{
							if (SepsDlg::InvokeDialog(pPlate, this))
							{
								// The angle or frequency has been changed by the user, so make sure that
								// screening is auto-enabled...
								if (!pTypesetInfo->AreScreening())
									pTypesetInfo->SetScreening(TRUE);
							}

							ShowDetails(); //update tab data - maybe it should check whether the dialog returned with OK or Cancel 
							OptionsTabs::SetApplyNowState(TRUE);
						}
					}
				}
			}

		case DIM_TEXT_CHANGED:
			// Enable the Apply button 
			OptionsTabs::SetApplyNowState(TRUE);
			break;

		case DIM_FOCUS_LOST:
			if (FALSE) {}
			else if (Msg->GadgetID == _R(IDC_COMBORESOLUTION))
			{
				pTypesetInfo->SetPrintResolution(pPrefsDlg->GetLongGadgetValue(_R(IDC_COMBORESOLUTION), 100, 10000));
			}
			else if (Msg->GadgetID == _R(IDC_COMBOFREQUENCY))
			{
				BOOL valid = TRUE;
				// Check that the value is within range, if not warn the user and fall back to a default value
				
				double NewVal = pPrefsDlg->GetDoubleGadgetValue(_R(IDC_COMBOFREQUENCY), 2, 1000, _R(IDS_INVALID_FREQUENCY), &valid);
				if (valid) // update the default frequency
				{
					pTypesetInfo->SetScreening(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CUSTOMSETTINGS)));
					if (pTypesetInfo->AreScreening())
						pTypesetInfo->SetDefaultScreenFrequency(NewVal, TRUE);
				}

				ShowDetails();
			}
			break;

		
		case DIM_SELECTION_CHANGED:
			if (FALSE) {}
			else if (Msg->GadgetID == _R(IDC_COMBOFREQUENCY)) // make sure a change in the default frequency is reflected in the plate list
			{
				pTypesetInfo->SetScreening(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CUSTOMSETTINGS)));
				if (pTypesetInfo->AreScreening())
					pTypesetInfo->SetDefaultScreenFrequency(pPrefsDlg->GetDoubleGadgetValue(_R(IDC_COMBOFREQUENCY), 2, 1000), TRUE);

				ShowDetails();
			}
			else if (Msg->GadgetID == _R(IDC_COMBORESOLUTION))
			{
				pTypesetInfo->SetPrintResolution(pPrefsDlg->GetLongGadgetValue(_R(IDC_COMBORESOLUTION), 100, 10000));
			}
			else if (Msg->GadgetID == _R(IDC_COMBOTYPE))
			{
				pTypesetInfo->SetScreenFunction((ScreenType) (pPrefsDlg->GetSelectedValueIndex(_R(IDC_COMBOTYPE)) + 1));
			}
			else if (Msg->GadgetID == _R(IDC_LISTCOLOURS))
			{
				// Store away the current plate settings in our local print control
				if (pTypesetInfo->AreSeparating())
					ReadCurrentPlateSettings(pTypesetInfo);
			}			
			OptionsTabs::SetApplyNowState(TRUE); // enable the Apply button
			break;


		case DIM_SELECTION_CHANGED_COMMIT:
			OptionsTabs::SetApplyNowState(TRUE);
			if (FALSE) {}
			else if (Msg->GadgetID == _R(IDC_LISTCOLOURS))
			{
				if (pTypesetInfo->AreSeparating())
				{
					String_256 GreyReason;
					OpState DlgState = SepsDlg::GetState(&GreyReason, NULL);

					if (!DlgState.Greyed)
					{
						// Store away the current plate settings in our local print control
						ReadCurrentPlateSettings(pTypesetInfo);

						// And give the user a properties dialogue to edit in
						ColourPlate *pPlate = GetSelectedPlate();
						if (pPlate != NULL)
						{
							if (SepsDlg::InvokeDialog(pPlate, this))
							{
								// The angle or frequency has been changed by the user, so make sure that
								// screening is auto-enabled...
								if (!pTypesetInfo->AreScreening())
									pTypesetInfo->SetScreening(TRUE);
							}
						}

						ShowDetails(); //update tab data - maybe it should check whether the dialog returned with OK or Cancel
					}
				}
			}
			break;

		default:
			return(PrintBaseTab::HandleMsg(Msg));
	}		
	return TRUE;
}  



/********************************************************************************************

>	virtual MsgResult PrintSepsDlg::Message(Msg *pMessage)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/96
	Inputs:		pMessage - the message

	Returns:	OK

	Purpose:	Handles miscellaneous system messages
				Note that many messages (e.g. docuement/view changing) are handled by
				the base dialogue, and call our UpdateSection() function. Only "left over"
				messages like ColourChangingMsg are passed on to the tabs

********************************************************************************************/

MsgResult PrintSepsTab::Message(Msg *pMessage)
{
	if (MESSAGE_IS_A(pMessage, ColourChangingMsg))
	{
		ColourChangingMsg *TheMsg = (ColourChangingMsg *) pMessage;

		switch ( TheMsg->State )
		{
			case ColourChangingMsg::COLOURUPDATED:
			case ColourChangingMsg::COLOURUPDATEDINVISIBLE:
			case ColourChangingMsg::LISTUPDATED:
				// Only handle these messages - others will be handled implicitly
				// by the Document/View changing handlers.
				ShowDetails();
				break;
			default:
				break;
		}
	}

	return(OK);
}



/******************************************************************************************

>	BOOL PrintSepsTab::CommitSection()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>/Jason
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Print Separations section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintSepsTab::CommitSection()
{
	if (IsAllGreyed() || pDocument == NULL)
		return(PrintBaseTab::CommitSection());

	ERROR3IF(pPrefsDlg == NULL, "PrintLayoutTab::CommitSection called with no dialog pointer");

	if (!TalkToPage())
		return TRUE;	// Just return if TalkToPage() fails

	// Always update the real document print control
	PrintControl* pPrCtrl = &LocalPrintControl; //GetPrintControl();
	ERROR2IF(pPrCtrl == NULL,TRUE,"NULL print control object");

	TypesetInfo *pTypesetInfo = pPrCtrl->GetTypesetInfo();
	ERROR2IF(pTypesetInfo == NULL, TRUE, "Can't find typesetting info structure");

	// Read the screening settings
	BOOL AreScreening = pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CUSTOMSETTINGS));
	if (AreScreening)
	{
		// Custom screening settings - read the other controls
		pTypesetInfo->SetPrintResolution(pPrefsDlg->GetLongGadgetValue(_R(IDC_COMBORESOLUTION), 100, 100000));
		pTypesetInfo->SetDefaultScreenFrequency(pPrefsDlg->GetDoubleGadgetValue(_R(IDC_COMBOFREQUENCY), 2, 1000));
		pTypesetInfo->SetScreenFunction((ScreenType) (pPrefsDlg->GetSelectedValueIndex(_R(IDC_COMBOTYPE)) + 1));
	}

	// Read the current plate settings into the document's TypesetInfo
	ReadCurrentPlateSettings(pTypesetInfo);

	// And turn on screening, making sure that the "global" screen settings are properly copied
	// through all the ColourPlates
	pTypesetInfo->SetScreening(AreScreening);
	pTypesetInfo->EnsureAllPlatesHaveGlobalSettings();

	// Read the "print spot colours as process colours" state, and now that we've committed to the value,
	// remember it as the "old" state so that cancel doesn't "undo" it! (Needed if the user uses "Apply"
	// followed by "Cancel")
	OldPrintSpotsAsProcess = pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKPROCESS));
	IndexedColour::ForceSpotsToBeProcess(OldPrintSpotsAsProcess);

	return(PrintBaseTab::CommitSection());
}



/******************************************************************************************

>	INT32 PrintSepsTab::ConvertDoubleToMinString(double Value, String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/96

	Inputs:		Value - The number to output
	Outputs:	Result - Returned with a string representation of the number
	Returns:	The number of decimal places that were output

	Purpose:	Converts a double to a "minimal" string.
				That is, it outputs up to 4 decimal places, but strips any trailing
				zeroes off (excepting the first after the decimal point)

				e.g 1.23456 -> 1.2345    1.230000 -> 1.23    1.00001 -> 1.0

******************************************************************************************/

INT32 PrintSepsTab::ConvertDoubleToMinString(double Value, String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal null param");

	INT32 DecimalPlaces = 4;
	const String_8 Zero(TEXT("0"));
	String_8 Temp;

	// Let's do this with applied brute force!
	while (DecimalPlaces >= 1)
	{
		Convert::DoubleToString(Value, (StringBase *)Result, DecimalPlaces);

		Result->Right(&Temp, 1);	// If the rightmost character isn't a 0, then return the string immediately
		if (Temp != Zero)
			return(DecimalPlaces);

		// Otherwise, see if we can stripanother char off the end
		DecimalPlaces--;
	}

	// Stripped loads of trailing zeroes, so return 1 d.p.
	return(1);
}



/******************************************************************************************

>	void PrintSepsTab::ReadCurrentPlateSettings(TypesetInfo *pTypesetInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96

	Outputs:	pTypesetInfo is updated to reflect the current dialogue state

	Purpose:	Retrieves all the colour plate settings in the dialogue and writes them into
				the given TypesetInfo.
				Used to update both the LocalPrintControl.GetTypesetInfo() (when settings
				are changed) and the document's TypesetInfo (in CommitSection)

******************************************************************************************/

void PrintSepsTab::ReadCurrentPlateSettings(TypesetInfo *pTypesetInfo)
{
	if (!TalkToPage())
		return;

	// Read the "Print Colour separations" check box state and set the print control
	// setting as appropriate
	BOOL DoSeparations = pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKSEPARATIONS));
	pTypesetInfo->SetSeparations(DoSeparations);

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
	// Read back the settings from the list view
	CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_LISTCOLOURS));
	if (pListGadget)
	{
		// Read the values of the list switches and set the print/overprint options accordingly
		BOOL ok = pTypesetInfo->CreatePlateList();
		ERROR3IF(!ok, "Unable to create plate list");

		ColourPlate* pPlate;
		pPlate = pTypesetInfo->GetFirstPlate();
		ERROR3IF(pPlate == NULL,"Error building colour plate list");
		for (INT32 j = 0; j < (INT32) pTypesetInfo->GetNumPlates(); j++)
		{
			if (pPlate)
			{
				pPlate->SetDisabled(!pListGadget->GetSwitchState(j, 0));
				pPlate->SetOverprint(pListGadget->GetSwitchState(j, 1));

				// Reading in the angle and frequency values is a bit more complicated
				String_256 Angle;
				String_256 Frequency;
				pListGadget->GetItemString(Angle, j, 4);
				pListGadget->GetItemString(Frequency, j, 5);
				pPlate->SetScreenInfo(camAtof((TCHAR*) Angle) , camAtof((TCHAR*) Frequency));

				pPlate = pTypesetInfo->GetNextPlate(pPlate);
			}
		}
	}
	else
	{
		ERROR3("Failed to find list gadget");
	}
#endif
}



/******************************************************************************************

>	BOOL PrintSepsTab::InitSection()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> - adapted from Markn's stuff
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the print separations section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PrintSepsTab::InitSection()
{
	if (PrintBaseTab::InitSection())
	{
		TalkToPage();
		// First retrieve the custom list control object and set the column widths
PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
		CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_LISTCOLOURS));
		ERROR2IF(!pListGadget, FALSE, "No list gadget");
		pListGadget->SetColumnWidth(0, GetSystemMetrics(SM_CXMENUCHECK) + 4); // Check 1
		pListGadget->SetColumnWidth(1, GetSystemMetrics(SM_CXMENUCHECK) + 5); // Check 2
		pListGadget->SetColumnWidth(2, 17);	// Colour bitmaps
		pListGadget->SetColumnWidth(3, 90); // text 1
		pListGadget->SetColumnWidth(4, 50); // text 2

		BOOL ok = pListGadget->CreateCustomHeader(_R(IDB_HEADER));
		ERROR3IF(!ok, "Cannot create listview header");

		// Remember the current "print spots as process" setting so we can undo it on a cancel
		OldPrintSpotsAsProcess = IndexedColour::AreSpotsForcedToProcess();

		BOOL Result = UpdateSection(OptionsTabs::GetDocumentName());		// Update section just fills everything in from scratch

		if (!IsAllGreyed() && pDocument != NULL)
			pListGadget->SetSelectedItemIndex(0);

		return(Result);
#endif
	}

	return FALSE;
}


/******************************************************************************************

>	BOOL PrintSepsTab::GreySection()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>/Adrian
	Created:	20/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when the user has closed all documents. Gives any tabbed dialog a
				chance to grey any fields which are document specific.
	Errors:		-
	SeeAlso:	PrintSepsTab::UngreySection(); PrintSepsTab::UpdateSection();

******************************************************************************************/

BOOL PrintSepsTab::GreySection()
{
	if (TalkToPage())
	{
PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
		// Delete the contents of the list
		CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_LISTCOLOURS));
		if (pListGadget != NULL)
				pListGadget->DeleteAllItems();		
#endif
		// Destroy the plate list to avoid all kinds of problems that might arise if it contained spot plates 
		TypesetInfo *pTypesetInfo = LocalPrintControl.GetTypesetInfo();
		ERROR2IF(pTypesetInfo == NULL, TRUE, "Can't find typesetting info structure");

		pTypesetInfo->DestroyPlateList();

		// Grey out all controls
		pPrefsDlg->SetBoolGadgetSelected(_R(IDC_PRINTERDEFAULTS), FALSE);
		pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CUSTOMSETTINGS), FALSE);
		pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKSEPARATIONS), FALSE);
		pPrefsDlg->DeleteAllValues(_R(IDC_COMBORESOLUTION));
		pPrefsDlg->DeleteAllValues(_R(IDC_COMBOFREQUENCY));
		pPrefsDlg->DeleteAllValues(_R(IDC_COMBOTYPE));
		pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKPROCESS), FALSE);
		EnableControl((CGadgetID) _R(IDC_PRINT_DOCNAMETEXT), FALSE);
		EnableControl((CGadgetID) _R(IDC_PRINT_DOCNAME), FALSE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC1), FALSE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC2), FALSE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC3), FALSE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC4), FALSE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC5), FALSE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC6), FALSE);
	}
	return(PrintBaseTab::GreySection());
}



/******************************************************************************************

>	BOOL PrintSepsTab::UngreySection()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>/Adrian
	Created:	20/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when the user has opened a document. Gives any tabbed dialog a
				chance to update any fields which may have been greyed when the last
				document was closed.
	Errors:		-
	SeeAlso:	 PrintSepsTab::GreySection(); PrintSepsTab::UpdateSection();

******************************************************************************************/

BOOL PrintSepsTab::UngreySection()
{
	if (TalkToPage())
	{
		// Switch to the new document print control  and update the tab
		if (pDocument)
			UpdateSection(OptionsTabs::GetDocumentName());	

		EnableControl((CGadgetID) _R(IDC_PRINT_DOCNAMETEXT), TRUE);
		EnableControl((CGadgetID) _R(IDC_PRINT_DOCNAME), TRUE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC1), TRUE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC2), TRUE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC3), TRUE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC4), TRUE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC5), TRUE);
		EnableControl((CGadgetID) _R(IDC_SSTATIC6), TRUE);
	}
	return PrintBaseTab::UngreySection();
}



/******************************************************************************************

>	BOOL PrintSepsTab::UpdateSection(String_256 *DocumentName)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/96
	Inputs:		Name of document 
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when the user has clicked on a new document or opened a new document.
				Gives any tabbed dialog a chance to update any fields which are used to
				reflect the current document's status.
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	PrintSepsTab::UngreySection(); PrintSepsTab::GreySection();

******************************************************************************************/

BOOL PrintSepsTab::UpdateSection(String_256 *DocumentName)
{
	// Copy all the new document's details into our local print control
	CopyLocalPrintControlFromDoc();

	BOOL ok = TalkToPage();
	if (!ok)
		return(FALSE);

	// Set the new document name
	pPrefsDlg->SetStringGadgetValue(_R(IDC_PRINT_DOCNAME), *DocumentName);

	// And show the new details
	ShowDetails(NULL, 0);
	
	return TRUE;
}



/******************************************************************************************

>	BOOL PrintSepsTab::ShowDetails()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		pGadgetIDList = ptr to gadgets to show details in (NULL means all gadgets)
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets values for the print layout section of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs; ShowDetails(CGadgetID* pGadgetIDList,CGadgetID Exclude)

******************************************************************************************/

BOOL PrintSepsTab::ShowDetails()
{
	return ShowDetails(NULL,0);
}



/******************************************************************************************

>	BOOL PrintSepsTab::ShowDetails(CGadgetID* pGadgetIDList,CGadgetID Exclude)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> - adapted from Markn's stuff
	Created:	12/6/96
	Inputs:		pGadgetIDList 	= ptr to gadgets to show details in (NULL means all gadgets)
				Exclude			= gadget to exclude in list
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets values for the print seps section of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PrintSepsTab::ShowDetails(CGadgetID* pGadgetIDList, CGadgetID Exclude)
{
	// For now, just rebuild the entire tab
	BOOL ok = TalkToPage();
	if (!ok)
		return(FALSE);

	if (IsAllGreyed() || pDocument == NULL)	// Don't try to fill in anything when the window is shaded
	{
PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
		CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_LISTCOLOURS));
		if (pListGadget != NULL)
			pListGadget->DeleteAllItems();
#endif
		EnableControls();
		return(TRUE);
	}

	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintSepsTab::InitSection called with no dialog pointer");

	TypesetInfo* tpInfo = LocalPrintControl.GetTypesetInfo();
	ERROR2IF(tpInfo == NULL,FALSE,"Error in PrintSepsTab::InitSection() : unexpected null pointer");

	// Now set up the controls
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_PRINTERDEFAULTS), !tpInfo->AreScreening());
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CUSTOMSETTINGS), tpInfo->AreScreening());


	// Set up the printer resolution list (just a default value at this stage)
	static INT32 Resolution[] = 
	{
		300,
		600,
		635,
		1200,
		1270,
		1693,
		2400,
		2540,
		0		// NULL terminator
	};

	pPrefsDlg->DeleteAllValues(_R(IDC_COMBORESOLUTION));
	String_256 Temp;
	INT32 i = 0;
	INT32 Selected = -1;
	while (Resolution[i] != 0)
	{
		if (Resolution[i] == tpInfo->GetPrintResolution())
			Selected = i;

		Temp._MakeMsg(TEXT("#1%ld"), Resolution[i]);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_COMBORESOLUTION), Temp, FALSE, i);
		i++;
	}
// As usual, this call causes dreadful gadget flicker
//	pPrefsDlg->SetComboListLength(_R(IDC_COMBORESOLUTION));

	if (Selected >= 0)
		pPrefsDlg->SetSelectedValueIndex(_R(IDC_COMBORESOLUTION), Selected);
	else
	{
		Temp._MakeMsg(TEXT("#1%ld"), tpInfo->GetPrintResolution());
		pPrefsDlg->SetStringGadgetValue(_R(IDC_COMBORESOLUTION),Temp, FALSE, -1);
	}

	//Set up the screen frequency list (just a default value at this stage)  
	static INT32 Frequency[] =
	{
		60,
		70,
		80,
		90,
		100,
		110,
		120,
		130,
		140,
		150,
		0
	};

	pPrefsDlg->DeleteAllValues(_R(IDC_COMBOFREQUENCY));
	i = 0;
	Selected = -1;
	while (Frequency[i] != 0)
	{
		if ((double)Frequency[i] == tpInfo->GetDefaultScreenFrequency())
			Selected = i;

		Temp._MakeMsg(TEXT("#1%ld"), Frequency[i]);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_COMBOFREQUENCY), Temp, FALSE, i);
		i++;
	}
// As usual, this call causes dreadful gadget flicker
//	pPrefsDlg->SetComboListLength(_R(IDC_COMBOFREQUENCY));

	if (Selected >= 0)
		pPrefsDlg->SetSelectedValueIndex(_R(IDC_COMBOFREQUENCY), Selected);
	else
	{
		// Put in the user custom value, with as few decimal places as we can get away with
//		ConvertDoubleToMinString(tpInfo->GetDefaultScreenFrequency(), &Temp);
		INT32 IntFreq = (INT32) floor(tpInfo->GetDefaultScreenFrequency());
		Temp._MakeMsg(TEXT("#1%d"), IntFreq);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_COMBOFREQUENCY), Temp, FALSE, -1);
	}

	//Set up the screen types list
	pPrefsDlg->DeleteAllValues(_R(IDC_COMBOTYPE));

	for (i = 0; ScreenIDs[i]; i++)
	{
		String_256 screenType(ScreenIDs[i]);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_COMBOTYPE), screenType, FALSE, i);
	}

	pPrefsDlg->SetSelectedValueIndex(_R(IDC_COMBOTYPE), ((INT32)tpInfo->GetScreenFunction()) - 1);

	// Set the "Print colour separations" checkbox
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKSEPARATIONS), tpInfo->AreSeparating());

	// Set up the separations list box
PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
	CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_LISTCOLOURS));
	ERROR2IF(pListGadget == NULL, FALSE, "No list gadget?!");

	// Remember the selected item so we can restore it after rebuilding the list
	INT32 SelectedItem = pListGadget->GetSelectedItemIndex();
	if (SelectedItem < 0)
		SelectedItem = 0;

	// Clear the gadget out and rebuild it for the (new) plate list
	pListGadget->DeleteAllItems();
#endif

	// Set the "print spot colours as process colours" flag appropriately
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKPROCESS), IndexedColour::AreSpotsForcedToProcess());

	// Call CreatePlateList to ensure that the list of printing plates is set up
	// This will just update the list (if necessary) if it has been previously created
	ok = tpInfo->CreatePlateList();
	ERROR2IF(!ok, FALSE, "Unable to create plate list");

	ColourPlate* pPlate;
	pPlate = tpInfo->GetFirstPlate();
	ERROR2IF(pPlate == NULL, FALSE, "PrintSepsTab::Init() : unable to get first plate");

	// First we loop through the plates just to have a look at their angle values so that we know how
	// to format them in the second loop
	INT32 AnglePrecision = 1;
	INT32 FreqPrecision = 1;
	for (i = 0 ; i < (INT32) tpInfo->GetNumPlates(); i++)
	{
		if (pPlate)
		{
			// For each plate, see how many decimal places it needs to display
			INT32 MinPrecision = ConvertDoubleToMinString(pPlate->GetScreenAngle(), &Temp);
			if (MinPrecision > AnglePrecision)
				AnglePrecision = MinPrecision;

			MinPrecision = ConvertDoubleToMinString(pPlate->GetScreenFrequency(), &Temp);
			if (MinPrecision > FreqPrecision)
				FreqPrecision = MinPrecision;

			pPlate = tpInfo->GetNextPlate(pPlate);
		}
	}

	// Second loop, this time round we fill in the plate list
	pPlate = tpInfo->GetFirstPlate();
	for (i = 0 ; i < (INT32) tpInfo->GetNumPlates(); i++)
	{
		if (pPlate)
		{
			String_256 plateDescription;
			pPlate->GetDescription(&plateDescription);

			double Angle = pPlate->GetScreenAngle();
			double Frequency = pPlate->GetScreenFrequency();
			ERROR3IF(Angle < 0.0 || Angle >= 360.0, "Colour plate returns an illegal screen angle");
			ERROR3IF(Frequency < 1.0 || Frequency > 1000.0, "Colour plate returns an illegal screen frequency");


			String_256 plateAngle;
			String_256 plateFrequency;

//			plateAngle.MakeMsg(_R(IDS_DEGREES), Angle);
//			String_256 DegreeFormat(_R(IDS_DEGREES));			
			//_stprintf((TCHAR *)plateAngle, (TCHAR *)DegreeFormat, Angle);
//			_stprintf((TCHAR *)plateAngle,"%.*f", precision,  Angle);
			Convert::DoubleToString(Angle, (StringBase *) &plateAngle, AnglePrecision);
			plateAngle += TEXT("");		// Let's give the internationalisation boys a heart attack

//			plateFrequency.MakeMsg(_R(IDS_LPI_FLOAT), Frequency);			
//			String_256 FreqFormat(_R(IDS_LPI_FLOAT));
//			_stprintf((TCHAR *)plateFrequency, (TCHAR *)FreqFormat, Frequency);
			Convert::DoubleToString(Frequency, (StringBase *) &plateFrequency, FreqPrecision);

			// Draw the colour patch, converting to screen RGB with the selected view's
			// colour context, so that we use the same colour correction options as that view
			ColourContext *cc = ColourManager::GetColourContext(COLOURMODEL_RGBT, DocView::GetSelected());

			DocColour PlateCol;
			pPlate->GetDisplayColour(&PlateCol);

			PColourRGBT RGB;
			cc->ConvertColour(&PlateCol, (ColourPacked *) &RGB);

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
			ok = pListGadget->AddColourListItem(plateDescription, RGB.Red, RGB.Green, RGB.Blue,
												pPlate->GetType() == COLOURPLATE_SPOT);
			ERROR3IF(!ok, "Failed to add item to list");

			// Add the text columns
			ok = pListGadget->SetItemString(plateAngle, i, 4);
			ERROR3IF(!ok, "Failed to set listview string");

			ok = pListGadget->SetItemString(plateFrequency, i, 5);
			ERROR3IF(!ok, "Failed to set listview string");

			pListGadget->SetSwitchState(!pPlate->IsDisabled(), i, 0);
			pListGadget->SetSwitchState(pPlate->Overprints(), i, 1);
#endif
			pPlate = tpInfo->GetNextPlate(pPlate);
		}
		else
			ERROR3("Fewer printing plates were found than expected");

	}

	// Shade/unshade the controls as appropriate
	EnableControls();

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
	// And re-select the previously selected (or first) item in the plate list
	pListGadget->SetSelectedItemIndex(SelectedItem);
#endif

	return TRUE;
}



/******************************************************************************************

>	BOOL PrintSepsTab::EnableControls()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>/Jason
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all went well
	Purpose:	Enables and disables the controls on the tab as appropriate for the 
				current settings
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintSepsTab::EnableControls()
{
	if (pPrefsDlg == NULL)
		return TRUE;

	if (!TalkToPage())
		return(FALSE);

	// If there is no XaraCMS colour manager, we simply can't do colour separations
	if (GetApplication()->GetCMSManager() == NULL)
		EnableControl(_R(IDC_CHECKSEPARATIONS), FALSE);

	TypesetInfo* tpInfo = NULL;
	if (!IsAllGreyed() && pDocument != NULL)
		tpInfo = LocalPrintControl.GetTypesetInfo();

	// Enable/Disable the postscript/screening options as appropriate
	BOOL Enabled = (tpInfo == NULL) ? FALSE : tpInfo->AreScreening();
	EnableControl(_R(IDC_COMBORESOLUTION),	Enabled);
	EnableControl(_R(IDC_COMBOFREQUENCY),	Enabled);
	EnableControl(_R(IDC_COMBOTYPE),		Enabled);


	// Enable/Disable the separation plate options depending on whether we're doing separations
	Enabled = (tpInfo != NULL && tpInfo->AreSeparating() && !IsAllGreyed() && pDocument != NULL);
	EnableControl(_R(IDC_LISTCOLOURS),		Enabled);
	EnableControl(_R(IDC_PLATEPROPS),		Enabled);

	Enabled = (!IsAllGreyed() && pDocument != NULL);
	EnableControl(_R(IDC_PRINTERDEFAULTS),	Enabled);

	EnableControl(_R(IDC_CHECKPROCESS),		Enabled);

	EnableControl(_R(IDC_CUSTOMSETTINGS),	Enabled);
	EnableControl(_R(IDC_CHECKSEPARATIONS),	Enabled);

	return TRUE;
}



/*********************************************************************************************

  >	PrintSepsTab::GetSelectedPlate() 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>/Jason
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	- a pointer to the colour plate currently selected in the list box (NULL if none)
	Purpose:	used to invoke the Properties popup with the curently selected plate
	Errors:		-
	SeeAlso:	- class SepsDlg

********************************************************************************************/

ColourPlate* PrintSepsTab::GetSelectedPlate()
{
	if (IsAllGreyed() || pDocument == NULL)
		return(NULL);

	ERROR2IF(pPrefsDlg == NULL,FALSE,"Error in PrintSepsTab::GetSelectedPlate() : unexpected null pointer");

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
	ColourPlate* pPlate = NULL;

	CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_LISTCOLOURS));
	if (pListGadget)
	{
		INT32 index = pListGadget->GetSelectedItemIndex(); 
		if (index != -1)
		{
			TypesetInfo* tpInfo = LocalPrintControl.GetTypesetInfo();
			if (tpInfo)
			{
				tpInfo->CreatePlateList();

				pPlate = tpInfo->GetFirstPlate();
				for (INT32 i = 0; i < index && pPlate != NULL; i++)
					pPlate = tpInfo->GetNextPlate(pPlate);
			}
			return pPlate;
		}
	}
	else
		ERROR3("Cannot find list gadget");
#endif

	return(NULL); // return NULL if no plate is selected
}

/************************************************************************************************
*************************************************************************************************
********************                END CLASS PrintSepsTab     **********************************
*************************************************************************************************
************************************************************************************************/






/************************************************************************************************
*************************************************************************************************
***************************               CLASS SepsDlg                ********************************
***************************               Adrian  17/06/96                 ********************************
*************************************************************************************************
************************************************************************************************/



const INT32 SepsDlg::IDD = _R(IDD_PLATEPROPS);
const CDlgMode SepsDlg::Mode = MODAL;


/*********************************************************************************************
  >	SepsDlg::SepsDlg() 


	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/96
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	default constructor
	Errors:		-
	SeeAlso:	- class SepsDlg
********************************************************************************************/

SepsDlg::SepsDlg() : DialogOp(IDD, Mode)
{
	pColourPlate = NULL;
	pLocalColourPlate = NULL;
	pParam = NULL;
}

/*********************************************************************************************
  >	SepsDlg::Create() 


	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/96
	Inputs:		-
	Outputs:	-
	Returns:	- DialogOp::Create() for now
	Purpose:	local version of DialogOp::Create()
	Errors:		-
	SeeAlso:	- class SepsDlg
********************************************************************************************/


BOOL SepsDlg::Create()
{
	return DialogOp::Create();
}



/*********************************************************************************************

>	void SepsDlg::InitControls(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96

	Purpose:	(Re-)Initialises all controls in the seps dialogue tab

********************************************************************************************/

void SepsDlg::InitControls(void)
{
	String_256 Temp;
	pColourPlate->GetDescription(&Temp);

	String_256 plateDescr;
	plateDescr.MakeMsg(_R(IDS_SEPPROPERTIES), (TCHAR *) Temp);

	SetStringGadgetValue(_R(IDC_PLATECAPTION), plateDescr);

	//Screen frequency  
	String_256 frequency;
	camSprintf((TCHAR*) frequency, _T("%1.4f"), double(pColourPlate->GetScreenFrequency()));
	SetStringGadgetValue(_R(IDC_EDITFREQUENCY), frequency, TRUE);

	//Screen angle  
	String_256 angle;
	camSprintf((TCHAR*) angle, _T("%1.4f"), double(pColourPlate->GetScreenAngle()));
	SetStringGadgetValue(_R(IDC_EDITANGLE), angle, TRUE);

	//'Print this ink' checkbox
	SetBoolGadgetSelected(_R(IDC_CHECKPRINT), !pColourPlate->IsDisabled());

	//'Overprint' checkbox
	SetBoolGadgetSelected(_R(IDC_CHECKOVERPRINT), pColourPlate->Overprints());
}



/*********************************************************************************************
  >	SepsDlg::Message(Msg* Message) 


	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/96
	Inputs:		- pointer to a Msg object
	Outputs:	-
	Returns:	- 
	Purpose:	main message handler, will call HandleMessage if message is for the dialog box,
				otherwise forward to base class
	Errors:		-
	SeeAlso:	- class SepsDlg
********************************************************************************************/


MsgResult SepsDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
		return HandleMessage((DialogMsg*) Message);

	return DialogOp::Message(Message);
}



/*********************************************************************************************
  >	SepsDlg::HandleMessage(DialogMessage* Msg) 


	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/96
	Inputs:		- pointer to a DialogMessage object
	Outputs:	-
	Returns:	- MsgResult::OK for now 
	Purpose:	local message handler
	Errors:		-
	SeeAlso:	- class SepsDlg
********************************************************************************************/

MsgResult SepsDlg::HandleMessage(DialogMsg* Msg)
{
	ERROR2IF(pColourPlate == NULL,FAIL,"Error in SepsDlg::HandleMessage() : unexpected null pointer");	
	ERROR2IF(pLocalColourPlate == NULL,FAIL,"Error in SepsDlg::HandleMessage() : unexpected null pointer");
	switch (Msg->DlgMsg)
	{
		case DIM_CREATE:
			//Display the current settings data for our colour plate 
			InitControls();
			break;

		case DIM_COMMIT:		// OK button clicked
			{
				BOOL angleIsValid = FALSE;
				BOOL frequencyIsValid = FALSE;
				double angle = GetDoubleGadgetValue(_R(IDC_EDITANGLE), 0, 360, _R(IDS_INVALID_ANGLE), &angleIsValid); 
				double frequency = GetDoubleGadgetValue(_R(IDC_EDITFREQUENCY), 1, 1000, _R(IDS_INVALID_FREQUENCY), &frequencyIsValid); 
				if (angleIsValid && frequencyIsValid)
				{
					pColourPlate->SetOverprint(pLocalColourPlate->Overprints());
					pColourPlate->SetScreenInfo(angle, frequency);
					pColourPlate->SetDisabled(pLocalColourPlate->IsDisabled());

					if (fabs(pLocalColourPlate->GetScreenAngle() - pColourPlate->GetScreenAngle()) > 0.0001)
						pParam->Result = TRUE;
					else if (fabs(pLocalColourPlate->GetScreenFrequency() - pColourPlate->GetScreenFrequency()) > 0.0001)
						pParam->Result = TRUE;

					Close();
					End();
					return OK;
				}
			}
			break;

		case DIM_CANCEL:		// Cancel button clicked
			{
				Close();
				End();
				return OK;		// After End, we mustn't call base class, as this object may have been deleted
			}
			break;

		case DIM_LFT_BN_CLICKED: // Left button pressed
			{
				if (Msg->GadgetID == _R(IDC_CHECKPRINT))
					pLocalColourPlate->SetDisabled(!GetBoolGadgetSelected(_R(IDC_CHECKPRINT)));
				else if (Msg->GadgetID == _R(IDC_CHECKOVERPRINT))
					pLocalColourPlate->SetOverprint(GetBoolGadgetSelected(_R(IDC_CHECKOVERPRINT)));
			}
		default:
			break;

	}
	// Always call the base class, or things like help buttons won't work
	return DialogOp::Message(Msg);
}



/*********************************************************************************************
  >	SepsDlg::~SepsDlg() 


	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/96
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:	destructor
	Errors:		-
	SeeAlso:	- class SepsDlg
********************************************************************************************/

SepsDlg::~SepsDlg()
{
	delete pLocalColourPlate;
}


void SepsDlg::Do(OpDescriptor*)
{
	ERROR3("Don't call SepsDlg::Do - use DoWithParam");
	End();
}


void SepsDlg::DoWithParam(OpDescriptor*, OpParam *Param)
{
	ERROR3IF(Param == NULL, "Illegal NULL param");
	if (!Param)
		return;

	pParam = (SepsDlgParam *) Param;
	ERROR3IF(pParam->pColourPlate == NULL, "Illegal NULL param");
	if (!pParam->pColourPlate)
		return;

	pColourPlate = pParam->pColourPlate;
	pLocalColourPlate = new ColourPlate(*pColourPlate);
	ERROR3IF(pLocalColourPlate == NULL,"Error in SepsDlg::DoWithParam() : allocation failure");	
	if (!pLocalColourPlate)
		return;

	pParent = pParam->pParent;

	pParam->Result = FALSE;		// Set up a reasonable default return value

	if (!Create())
	{
		InformError();
  		End();
	}
}


BOOL SepsDlg::Init(void)
{
	return RegisterOpDescriptor(	0,
									_R(IDS_SEPSDIALOG),
									CC_RUNTIME_CLASS(SepsDlg),
									OPTOKEN_SEPSDIALOG,
									SepsDlg::GetState,
									0,							// help ID 
									0			// bubble help
								);

}



OpState SepsDlg::GetState(String_256*, OpDescriptor*)
{
	OpState State;

	return(State);
}

/*********************************************************************************************

  >	BOOL SepsDlg::InvokeDialog(ColourPlate *pTheColourPlate, PrintBaseTab* pParentDialog) 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/96
	Inputs:		- pointer to the colour plate we're setting, pointer to the Separations dialog tab
	Outputs:	-
	Returns:	TRUE if the colour plate angle or frequency values were changed
	Purpose:	display the dialog
	Errors:		-
	SeeAlso:	- class SepsDlg
********************************************************************************************/

BOOL SepsDlg::InvokeDialog(ColourPlate *pTheColourPlate, PrintBaseTab* pParentDialog)
{
	ERROR3IF(pTheColourPlate == NULL, "Come on, play by the rules");
	ERROR3IF(pParentDialog == NULL, "Come on, play by the rules!!!");

	OpDescriptor *TheDlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(SepsDlg));

	ERROR3IF(TheDlg == NULL, "SepsDlg::InvokeDialog is unable to find the Dlg OpDescriptor");

	if (TheDlg != NULL)
	{
		SepsDlgParam Param;
		Param.pColourPlate = pTheColourPlate;
		Param.pParent = pParentDialog;
		Param.Result = FALSE;

		TheDlg->Invoke(&Param);
		return(Param.Result);
	}

	return(FALSE);
}


/************************************************************************************************
*************************************************************************************************
********************                END CLASS SepsDlg     **********************************
*************************************************************************************************
************************************************************************************************/










/************************************************************************************************
*************************************************************************************************
****************************** CLASS PrintImagesetterTab   ********************************
**************                              Adrian  10/08/96                    ********************************
*************************************************************************************************
************************************************************************************************/


/********************************************************************************************
*********************************************************************************************
*********************************************************************************************

  >	PrintImagesetterTab::PrintImagesetterTab() 

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintImagesetterTab default constructor.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PrintImagesetterTab::PrintImagesetterTab()
{
}

/********************************************************************************************

>	PrintImagesetterTab::~PrintImagesetterTab()


	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PrintImagesetterTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
PrintImagesetterTab::~PrintImagesetterTab()
{ 
}        

CDlgResID PrintImagesetterTab::GetPageID()	{ return _R(IDD_OPTSTAB_IMAGESETTING); }

/********************************************************************************************

>	BOOL PrintImagesetterTab::HandleMsg(DialogMsg* Msg)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles messages sent by various controls on this tab  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PrintImagesetterTab::HandleMsg(DialogMsg* Msg)
{
	if (Msg->DlgMsg != DIM_CREATE && (IsAllGreyed() || pDocument == NULL))
		return(TRUE);

	ERROR2IF(Msg == NULL,FALSE,"PrintImagesetterTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintImagesetterTab::HandleMsg called with no dialog pointer");

	BOOL ok = TalkToPage();	
	if (!ok)
		return TRUE;		// page not present

	switch(Msg->DlgMsg)
	{
		case DIM_LFT_BN_CLICKED:
			{
				TypesetInfo *tpInfo = LocalPrintControl.GetTypesetInfo();
				ERROR2IF(tpInfo == NULL, TRUE, "No typesetInfo?!");

				if (FALSE) {}
				else if (Msg->GadgetID == _R(IDC_CHECKPMARKS))
				{
					tpInfo->SetOutputPrintersMarks(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKPMARKS)));

					// Make sure the print layout tab updates scale factors etc suitably
					BROADCAST_TO_ALL(PrintMsg(PrintMsg::SETTINGSCHANGED));

					// And shade/unshade the controls as appropriate
					EnableControls();
				}
				else if (Msg->GadgetID == _R(IDC_CHECKOPBLACK))
				{
					tpInfo->SetOverprintBlack(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKOPBLACK)));
				}
				else if (Msg->GadgetID == _R(IDC_CHECKNEGATIVE))
				{
					tpInfo->SetPhotoNegative(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKNEGATIVE)));
				}
				else if (Msg->GadgetID == _R(IDC_CHECKREFLECT))
				{
					tpInfo->SetEmulsionDown(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKREFLECT)));
				}
				OptionsTabs::SetApplyNowState(TRUE);
			}
			break;

		case DIM_SELECTION_CHANGED:
		case DIM_SELECTION_CHANGED_COMMIT:
			// If the user has ticked a print mark, check if it's a target or star, and if it is, 
			// un-tick all other marks which are also of the same type.
			EnsureSensiblePrintMarks();
			break;

		default:
			return(PrintBaseTab::HandleMsg(Msg));
	}

	return TRUE;
}  



/******************************************************************************************

>	BOOL PrintImagesetterTab::GreySection()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when the user has closed all documents. Gives any tabbed dialog a
				chance to grey any fields which are document specific.
	Errors:		-
	SeeAlso:	PrintImagesetterTab::UngreySection(); PrintImagesetterTab::UpdateSection();

******************************************************************************************/

BOOL PrintImagesetterTab::GreySection()
{
	if (TalkToPage())
	{
		pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKPMARKS), FALSE);
		pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKREFLECT), FALSE);
		pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKNEGATIVE), FALSE);
		pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKOPBLACK), FALSE);

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
		CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_PRINTERMARKSLIST));
		if (pListGadget != NULL)
			pListGadget->DeleteAllItems();
#endif

		EnableControl((CGadgetID) _R(IDC_PRINT_DOCNAME), FALSE);
		EnableControl((CGadgetID) _R(IDC_ISTATIC1), FALSE);
		EnableControl((CGadgetID) _R(IDC_ISTATIC2), FALSE);
		EnableControl((CGadgetID) _R(IDC_ISTATIC3), FALSE);
		EnableControl((CGadgetID) _R(IDC_ISTATIC4), FALSE);
	}
	return(PrintBaseTab::GreySection());
}






/******************************************************************************************

>	BOOL PrintImagesetterTab::UngreySection()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Called when a new document is open
	Errors:		-
	SeeAlso:	PrintImagesetterTab::GreySection(); PrintImagesetterTab::UpdateSection();

******************************************************************************************/

BOOL PrintImagesetterTab::UngreySection()
{
	if (TalkToPage())
	{
		EnableControl((CGadgetID) _R(IDC_PRINT_DOCNAME), TRUE);
		EnableControl((CGadgetID) _R(IDC_ISTATIC1), TRUE);
		EnableControl((CGadgetID) _R(IDC_ISTATIC2), TRUE);
		EnableControl((CGadgetID) _R(IDC_ISTATIC3), TRUE);
		EnableControl((CGadgetID) _R(IDC_ISTATIC4), TRUE);
	}
	return PrintBaseTab::UngreySection();
}




/********************************************************************************************

>	void PrintImagesetterTab::EnsureSensiblePrintMarks(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/8/96

	Purpose:	Ensures that only one printer mark of either "target" or "star" types
				is ticked in the print marks list. It assumes that the currently selected
				list item is the one that you want enabled, and disables all others
				of the same type - if a non-star/target type mark is selected, nothing
				is done.
				
				This just stops the user selecting two mutually exclusive print marks

********************************************************************************************/

void PrintImagesetterTab::EnsureSensiblePrintMarks(void)
{
	if (!pDocument)
		return;

	OptionsTabs::SetApplyNowState(TRUE);

	INT32 SelIndex = 0;

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
	CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_PRINTERMARKSLIST));
	if (!pListGadget)
		return;

	SelIndex = pListGadget->GetSelectedItemIndex();
	if (SelIndex < 0)		// No selected item?!
		return;

	if (!pListGadget->GetSwitchState(SelIndex, 0))
		return;				// Selected Item is disabled, so no problem
#endif

	PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan == NULL)
		return;

	if (!TalkToPage())
		return;

	// Get that print marks doc component
	PrintMarksComponent* pMarksComp = (PrintMarksComponent*)pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintMarksComponent));
	if (!pMarksComp)
		return;

	// Scan the print marks to see if it's a star or target mark that is currently selected
	PrintMarkItem* pMarkItem = pMarksMan->PMMCache.GetFirstMark();
	INT32 Index = 0;
	while (pMarkItem && Index < SelIndex)		// Scan to find the SelIndex'th item
	{
		Index++;
		pMarkItem = pMarksMan->PMMCache.GetNextMark(pMarkItem);
	}

	// Find the print mark for the selection if we can
	if (pMarkItem == NULL)
		return;

	PrintMark *ThisMark = pMarkItem->GetPrintMark();
	if (ThisMark == NULL)
		return;

	// See if the selected item is a star/target
	MarkType TypeToKill = ThisMark->GetType();

	if (TypeToKill != MarkType_Star && TypeToKill != MarkType_Registration)
		return;		// Selected item not star/target mark, so no need 

	// OK, the user just selected a star/target, so turn off all others of the same type...
	pMarkItem = pMarksMan->PMMCache.GetFirstMark();
	Index = 0;
	while (pMarkItem)
	{
		if (Index != SelIndex)
		{
			ThisMark = pMarkItem->GetPrintMark();
PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
			if (ThisMark != NULL && ThisMark->GetType() == TypeToKill)		// Kill all other marks of this type
				pListGadget->SetSwitchState(FALSE, Index, 0);
#endif
		}

		pMarkItem = pMarksMan->PMMCache.GetNextMark(pMarkItem);
		Index++;
	}
}



/********************************************************************************************

>	virtual MsgResult PrintImagesetterTab::Message(Msg *pMessage)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/96
	Inputs:		pMessage - the message

	Returns:	OK

	Purpose:	Handles miscellaneous system messages
				Note that many messages (e.g. docuement/view changing) are handled by
				the base dialogue, and call our UpdateSection() function. Only "left over"
				messages like ColourChangingMsg are passed on to the tabs

********************************************************************************************/

MsgResult PrintImagesetterTab::Message(Msg *pMessage)
{
	if (MESSAGE_IS_A(pMessage, OptionsChangingMsg))
	{
		OptionsChangingMsg *TheMsg = (OptionsChangingMsg *) pMessage;

		switch ( TheMsg->State )
		{
			case OptionsChangingMsg::SEPARATIONSENABLED:
				// If the colour separation enablement state has changed, then
				// update our controls approrpately. This basically means shading
				// or unshading the printers marks list - the output of printers
				// marks (while otherwise independent is slaved off the "do separations"
				// state whenever it is changed
				ShowDetails();
				break;
			default:
				break;
		}
	}

	return(OK);
}



/******************************************************************************************

>	BOOL PrintImagesetterTab::CommitSection()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Print Separations section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintImagesetterTab::CommitSection()
{
	if (IsAllGreyed() || pDocument == NULL || !TalkToPage())
		return(PrintBaseTab::CommitSection());

	ERROR3IF(pPrefsDlg == NULL, "PrintImagesetterTab::CommitSection called with no dialog pointer");
	ERROR2IF(GetPrintControl() == NULL, FALSE, "Can't find PrintControl info");

	TypesetInfo* tpInfo = /*GetPrintControl()->*/LocalPrintControl.GetTypesetInfo();
	ERROR2IF(tpInfo == NULL,FALSE,"Error in PrintImagesetterTab::CommitSection() : unexpected null pointer");

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
	// Get a pointer to the print marks list
	CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_PRINTERMARKSLIST));
	if (pListGadget)
	{ 
		//Read the values of the list switches and set the printer marks accordingly
		PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
		ERROR2IF(!pMarksMan, FALSE, "Print marks manager is NULL");

		// Set the overall printing of Printers Marks
		BOOL UseMarks = pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKPMARKS));
		tpInfo->SetOutputPrintersMarks(UseMarks);
		TalkToPage();	// SetOutputPrintMarks could load a new document, so do this just to be safe

		if (pDocument)
		{
			// Get that print marks doc component
			PrintMarksComponent* pMarksComp = (PrintMarksComponent*)pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintMarksComponent));
			if (pMarksComp)
			{
				// Trash the current mark list
				pMarksComp->RemoveAllMarks();

				// now go through each print mark and add it if necessary
				PrintMarkItem* pMarkItem = pMarksMan->PMMCache.GetFirstMark();
				INT32 index = 0;
				while (pMarkItem)
				{
					// Get the handle of this print mark item
					UINT32 handle = pMarkItem->GetHandle();

					// Get the corresponding list state
					if (pListGadget->GetSwitchState(index, 0))
						pMarksComp->AddMark(handle);

					// find the next print mark item
					pMarkItem = pMarksMan->PMMCache.GetNextMark(pMarkItem);
					index++;
				}
			}
		}
	}
	else
	{
		ERROR3("Failed to find list gadget");
	}
#endif

	// Set the general imagesetting flags
	tpInfo->SetOverprintBlack(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKOPBLACK)));
	tpInfo->SetPhotoNegative(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKNEGATIVE)));
	tpInfo->SetEmulsionDown(pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKREFLECT)));

	// Make sure the print layout tab updates scale factors etc suitably
//	BROADCAST_TO_ALL(PrintMsg(PrintMsg::SETTINGSCHANGED));

	return(PrintBaseTab::CommitSection());
}



/******************************************************************************************

>	BOOL PrintImagesetterTab::InitSection()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the print separations section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PrintImagesetterTab::InitSection()
{
	if (!TalkToPage())
		return(FALSE);

	if (PrintBaseTab::InitSection())
	{
		ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintImagesetterTab::InitSection called with no dialog pointer");

		// Set the check boxes for the general options
		if (!TalkToPage())
			return TRUE;

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
		// Set up the printer marks list
		CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_PRINTERMARKSLIST));
		ERROR2IF(!pListGadget, FALSE, "No list gadget");
		pListGadget->SetColumnWidth(0, GetSystemMetrics(SM_CXMENUCHECK) + 2); // Check box
		pListGadget->SetColumnWidth(1, 20); // bitmap
#endif

		// Fill in all the controls, and enable/disable as appropriate
		UpdateSection(OptionsTabs::GetDocumentName());
		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL PrintImagesetterTab::UpdateSection(String_256 *DocumentName)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>/Adrian
	Created:	28/8/96
	Inputs:		DocumentName - Name of the new document
	Returns:	TRUE for success
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
	Errors:		-
	SeeAlso:	PrintBaseTab::GreySection; PrintBaseTab::UngreySection; PrintBaseTab::InitSection;

********************************************************************************************/

BOOL PrintImagesetterTab::UpdateSection(String_256 *DocumentName)
{
	if (!TalkToPage())
		return(FALSE);

	// Set the docuemnt name
	pPrefsDlg->SetStringGadgetValue(_R(IDC_PRINT_DOCNAME), *DocumentName);

	// And set up all the other controls
	ShowDetails();

	return(TRUE);
}



/******************************************************************************************

>	virtual BOOL PrintImagesetterTab::ShowDetails()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets values for the Imagesetting section of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL PrintImagesetterTab::ShowDetails()
{
	if (IsAllGreyed() || pDocument == NULL)		// Don't try to fill in anything when the window is shaded
	{
PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
		CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_PRINTERMARKSLIST));
		if (pListGadget != NULL)
			pListGadget->DeleteAllItems();
#endif
		EnableControls();
		return(TRUE);
	}

	ERROR2IF(pPrefsDlg == NULL,FALSE,"PrintBaseTab::UpdateSection called with no dialog pointer");

	if (!TalkToPage())
		return(FALSE);

	TypesetInfo* tpInfo = LocalPrintControl.GetTypesetInfo();
	ERROR2IF(tpInfo == NULL,FALSE,"Error in PrintSepsTab::InitSection() : unexpected null pointer");
	
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKREFLECT),	tpInfo->PrintEmulsionDown());
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKNEGATIVE),	tpInfo->PrintPhotoNegative());
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKOPBLACK),	tpInfo->AlwaysOverprintBlack()); 

	// Set the global "output printers marks" checkbox state
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECKPMARKS), tpInfo->OutputPrintersMarks());

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
	// And set up the marks list
	CCustomList* pListGadget = CCustomList::GetGadget(pPrefsDlg->GetReadWriteWindowID(), _R(IDC_PRINTERMARKSLIST));
	if (!pListGadget)
		return(FALSE);
#endif

	PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
	ERROR3IF(pMarksMan == NULL, "No print mark manager?");

	// Make sure this document includes the default marks.. assume it's the selected doc!
	if (Document::GetSelected() != NULL)
		pMarksMan->AddDefaultMarksToDoc(Document::GetSelected());

	PrintMarkItem* pMarkItem = pMarksMan->PMMCache.GetFirstMark();

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
	// Vape and re-build the marks list
	pListGadget->DeleteAllItems();
#endif

	while (pDocument && pMarkItem)
	{
		PrintMarksComponent* pMarksComp = (PrintMarksComponent*)pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintMarksComponent));
		// Get the handle of this print mark item
		UINT32 handle = pMarkItem->GetHandle();
		// Get the bitmap ID for this type of mark

PORTNOTE("other", "While CCustomList, mark 2 variables as used to avoid warnings")
		// Mark the above as used
		CAM_USE(pMarksComp);
		CAM_USE(handle);

		ERROR2IF(!pMarkItem->GetPrintMark(), FALSE, "Illegal NULL pointer");

		UINT32 ImageID = _R(IDB_PRINTMARK_UNKNOWNMK);
		UINT32 GreyedID = _R(IDB_PRINTMARK_UNKNOWNMK);
		MarkType markType = pMarkItem->GetPrintMark()->GetType();
		switch (markType)
		{
			case MarkType_Star:			ImageID = _R(IDB_PRINTMARK_STARMK);			GreyedID = _R(IDB_PRINTMARK_GREYED_STARMK);			break;
			case MarkType_Registration:	ImageID = _R(IDB_PRINTMARK_REGISTRATIONMK);	GreyedID = _R(IDB_PRINTMARK_GREYED_REGISTRATIONMK);	break;
			case MarkType_Information:	ImageID = _R(IDB_PRINTMARK_INFORMATIONMK);	GreyedID = _R(IDB_PRINTMARK_GREYED_INFORMATIONMK);		break;
			case MarkType_Crop:			ImageID = _R(IDB_PRINTMARK_CROPMK);			GreyedID = _R(IDB_PRINTMARK_GREYED_CROPMK);			break;
			case MarkType_GreyBar:		ImageID = _R(IDB_PRINTMARK_GRAYBARMK);		GreyedID = _R(IDB_PRINTMARK_GRAYBARMK);			break;
			case MarkType_ColourBar:	ImageID = _R(IDB_PRINTMARK_COLOURBARMK);	GreyedID = _R(IDB_PRINTMARK_GREYED_COLOURBARMK);		break;
			default:
				break;
		}

		String_256 markDescription(pMarkItem->GetPrintMark()->GetMarkMenuText());
PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
		pListGadget->AddItem(markDescription, ImageID, GreyedID);

		if (pMarksComp)
			pListGadget->SetSwitchState(NULL != pMarksComp->FindMark(handle), (pListGadget->GetItemCount() - 1), 0);
#endif

		// find the next print mark item
		pMarkItem = pMarksMan->PMMCache.GetNextMark(pMarkItem);
	}

PORTNOTE("other", "Disabled CCustomList")
#ifndef EXCLUDE_FROM_XARALX
	// And make the first item in the list selected
	pListGadget->SetSelectedItemIndex(0);
#endif

	EnableControls();

	return(TRUE);
}



/******************************************************************************************

>	BOOL PrintImagesetterTab::EnableControls()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all went well
	Purpose:	Enables and disables the controls on the tab as appropriate for the 
				current settings
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintImagesetterTab::EnableControls()
{
	if (pPrefsDlg == NULL)
		return TRUE;

	if (!TalkToPage())
		return(TRUE);

	if (!IsAllGreyed() && pDocument != NULL)
	{
		// If we have a marks manager, fill out the printer marks list
		PrintMarksMan *pPMM = GetApplication()->GetMarksManager();

		// If there is no print marks manager, the list will be disabled 
		EnableControl(_R(IDC_PRINTERMARKSLIST),
					(pPMM != NULL && pDocument != NULL && pPrefsDlg->GetBoolGadgetSelected(_R(IDC_CHECKPMARKS))));
	}
	else
		EnableControl(_R(IDC_PRINTERMARKSLIST), FALSE);

	// Enable/Disable the postscript/screening options as appropriate
	EnableControl(_R(IDC_CHECKREFLECT),		!IsAllGreyed() && pDocument != NULL);
	EnableControl(_R(IDC_CHECKNEGATIVE),	!IsAllGreyed() && pDocument != NULL);
	EnableControl(_R(IDC_CHECKOPBLACK),		!IsAllGreyed() && pDocument != NULL);
	EnableControl(_R(IDC_CHECKPMARKS),		!IsAllGreyed() && pDocument != NULL);

	return TRUE;
}



/*************************************************************************************************************/
/*********************** END CLASS PrintImagesetterTab ************************************************/
/*************************************************************************************************************/ 


/********************************************************************************************

>	static BOOL OpShowPrintBorders::Init()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptors for show grid toggle op
	Errors:		Returns FALSE on failure.
	Scope:		Static

********************************************************************************************/


BOOL OpShowPrintBorders::Init()
{
	return RegisterOpDescriptor(	0,
									_R(IDS_SHOWPRINTBORDERS),
									CC_RUNTIME_CLASS(OpShowPrintBorders),
									OPTOKEN_SHOWPRINTBORDERS,
									GetState,
									0,							// help ID 
									_R(IDBBL_PRINTBORDERS),			// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_PRINTBORDERS),			// control ID
									SYSTEMBAR_FILE,				// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,							// No vertical counterpart
									0,							// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
}

/********************************************************************************************

>	OpShowPrintBorders::OpShowPrintBorders()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpShowPrintBorders operation. It is not undoable.
	Errors:		None

********************************************************************************************/

OpShowPrintBorders::OpShowPrintBorders()
{
}

/********************************************************************************************

>	void OpShowPrintBorders::Do(OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Forces a redraw of the selected DocView and toggles the NodeGrid::RenderFlag flag
	Errors:		None

********************************************************************************************/

void OpShowPrintBorders::Do(OpDescriptor*)
{
	DocView *pDocView = DocView::GetSelected();

	if (pDocView != NULL)
	{
		pDocView->SetShowPrintBorders(!pDocView->GetShowPrintBorders());
		pDocView->ForceRedraw(TRUE);
	}

	End();
}

/********************************************************************************************

>	OpState OpShowPrintBorders::GetState(String_256*, OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available, so long as a document is visible.
	Errors:		None

********************************************************************************************/

OpState OpShowPrintBorders::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	DocView *pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		OpSt.Ticked = pDocView->GetShowPrintBorders();

	return OpSt;
}


//--------------------------------------------------------------------------------------------
#endif //webster
