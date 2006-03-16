// $Id: optsplug.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of the plug-in tab of the options dialog box

/*

*/

#include "camtypes.h"
#include "appprefs.h"
#include "optsplug.h"
//#include "prefsdlg.h"	// dialog/gadget ids
//#include "optsres.h"	// error strings
#include "app.h"		// Camelot object
//#include "ollie.h"		// For _R(IDS_NO_WEB_UTILITY) and _R(IDS_NO_WEB_PAGE)
//#include "resource.h"	// For _R(IDS_OK)

//#include "plugmngr.h"	// PlugInManager
#include "xpehost.h"
//#include "urls.h"

CC_IMPLEMENT_DYNAMIC(PlugInsTab, OptionsTabs)   

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");
#define THIRTY_TWO 32

/********************************************************************************************

>	PlugInsTab::PlugInsTab() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PlugInsTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PlugInsTab::PlugInsTab()
{   
	// Set up our default states
	m_ChangedList = FALSE;
}        

/********************************************************************************************

>	PlugInsTab::~PlugInsTab()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	PlugInsTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
PlugInsTab::~PlugInsTab()
{   
}        

/********************************************************************************************

>	BOOL PlugInsTab::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	PlugInsTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL PlugInsTab::Init()
{   
//	m_pManager = NULL;

	return TRUE;
}        


/******************************************************************************************

>	CDlgResID PlugInsTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID PlugInsTab::GetPageID()
{
	return _R(IDD_OPTSTAB_PLUGINS);
}

/******************************************************************************************

>	BOOL PlugInsTab::IsDocumentOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a document option.	  
	Purpose:	Allows the document options dialog code to determine if this tab belongs
				to the group of document options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL PlugInsTab::IsDocumentOption()
{
	return FALSE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL PlugInsTab::IsProgramOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the program options dialog code to determine if this tab belongs
				to the group of program options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL PlugInsTab::IsProgramOption()
{
	return TRUE;	// This tab is a program option 
}


/******************************************************************************************

>	BOOL AppPrefsDlg::CommitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Units section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PlugInsTab::CommitSection()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PlugInsTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_PLUGINS));	// The units page identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	ok = CommitDialogValues();

	return ok;
}

/********************************************************************************************

>	BOOL PlugInsTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Plug-ins options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PlugInsTab::HandleMsg(DialogMsg* Msg)
{
	ERROR2IF(Msg == NULL,FALSE,"PlugInsTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PlugInsTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_PLUGINS));	// The edit page identifier
	if (!ok)
		return TRUE;		// page not present
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
//			GreyStatus = FALSE;	// we are ungreyed by default
			ok = InitSection();
			if (!ok)
				InformError();
			break;

	    case DIM_LFT_BN_CLICKED:
			OptionsTabs::SetApplyNowState(TRUE);
//			ERROR2IF(m_pManager == NULL,FALSE,"PlugInsTab::HandleMsg called with no plug-ins manager pointer");
			switch (Msg->GadgetID)
			{
			case _R(IDC_OPTS_BUTTON_PLUGINS_SETUP):
				BOOL bOK = XPEEditOp::EditPluginSettings();
				break;
/*				case _R(IDC_OPTS_NEWPATH):
					// Create the user a new path
					CreateNewPath();
					break;

				case _R(IDC_OPTS_BUYPLUGS):
					// The user wishes to be relieved of money by buying plug-ins
					BuyPluginsAction();
					break;

				case _R(IDC_OPTS_DELETEPATH):
					// Delete the currently selected path
					DeletePath();
					break;
				case _R(IDC_OPTS_PARSEATSTART):
				{
					BOOL Valid = FALSE;
					BOOL ParseAtStartUp = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_PARSEATSTART), 0, 1, 0, &Valid);
					if (Valid)
						m_pManager->SetParseAtStartUp(ParseAtStartUp);
					break;
				}
*/
			}
			break; // DIM_LFT_BN_CLICKED

		case DIM_SELECTION_CHANGED:
		{
			OptionsTabs::SetApplyNowState(TRUE);
	
/*			switch (Msg->GadgetID)
			{
				case _R(IDC_OPTS_PLUGINSLIST):
				{
					// Clicked on the list of plug-in paths
					// If its valid then ungrey the delete button
					WORD Index = 0;
					pPrefsDlg->GetValueIndex(_R(IDC_OPTS_PLUGINSLIST), &Index); 
					BOOL Valid = FALSE;
					BOOL ok = FALSE;
					String_256 Path = pPrefsDlg->GetStringGadgetValue(_R(IDC_OPTS_PLUGINSLIST), &Valid, Index);
					if (Valid && !Path.IsEmpty())
					{
						pPrefsDlg->EnableGadget(_R(IDC_OPTS_DELETEPATH), TRUE);
					}
					break;
				}
			}
*/
			break; // DIM_SELECTION_CHANGED
		}
	}

	return TRUE;
}  


/********************************************************************************************

>	BOOL PlugInsTab::CommitDialogValues()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Takes the values in the dialog.
				Called when the OK button is selected
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PlugInsTab::CommitDialogValues()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PlugInsTab::CommitDialogValues called with no dialog pointer");
//	ERROR2IF(m_pManager == NULL,FALSE,"PlugInsTab::CommitDialogValues called with no plug-ins manager pointer");

	// All our settings should be up todate on the dialog box itself
	// All we need to do is rescan all of the plug-in paths available.

	// If we have changed our list changed flag then we should reparse the list of plug-ins for
	// these paths.
	BOOL ok = TRUE;
//	if (m_ChangedList)
//	{	
//		ok = ok && m_pManager->DestroyPlugInsList();
//		ok = ok && m_pManager->ParseAllPaths();
//
//		if (ok)
//			m_ChangedList = FALSE;
//	}

	BOOL SetOk = FALSE;
	INT32 State = 0;
	BOOL Valid = FALSE;

/*	// Cacheing preferences
	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_CHECK_CACHE_ENABLED), 0, 1, 0, &Valid);
	if (Valid)
		SetOk = Camelot.SetPrefValue(TEXT("Rendering"), TEXT("Cacheing"), &State);

//	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_EDIT_CACHE_SIZE), 1, 100, 0, &Valid);
	String_256 strCachePercent = pPrefsDlg->GetStringGadgetValue(_R(IDC_EDIT_CACHE_SIZE), &Valid);
	double dValue = 0;
	INT32 Pos = 0;
	INT32 iValue = 0;
	Convert::ReadNumber(&strCachePercent, &Pos, &dValue);	// Allow trailing chars
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
*/
	// Effect resolution preferences
	INT32 dLiveResolution = pPrefsDlg->GetResDropListValue(_R(IDC_COMBO_LIVEEFFECT_RES), TRUE, &Valid);
	if (Valid)
		Camelot.SetPrefValue(TEXT("Effects"), TEXT("DefaultLivePixelsPerInch"), &dLiveResolution);

	INT32 dLockedResolution = pPrefsDlg->GetResDropListValue(_R(IDC_COMBO_LOCKEDEFFECT_RES), FALSE, &Valid);
	if (Valid)
		Camelot.SetPrefValue(TEXT("Effects"), TEXT("DefaultLockedPixelsPerInch"), &dLockedResolution);

	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_CHECK_NEW_EFFECTS_LOCKED), 0, 1, 0, &Valid);
	SetOk = Camelot.SetPrefValue(TEXT("Effects"), TEXT("DefaultLocked"), &State);

	return ok;
}

/********************************************************************************************

>	BOOL PlugInsTab::CreateNewPath()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Creates a new path that the user has just requested.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
BOOL PlugInsTab::CreateNewPath()
{
	ERROR2IF(m_pManager == NULL,FALSE,"PlugInsTab::CreateNewPath called with no plug-ins manager pointer");
	
	PathName NewSearchPath;
	BOOL ok = m_pManager->GetPlugInPath( &NewSearchPath );
	if (ok)
	{
		ok = m_pManager->AddPlugInPath(NewSearchPath);

		// Now update the list of available pathnames
		// Do it the hard way to ensure we are in sync with the plug-in manager's list
		pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_PLUGINSLIST));
		BOOL ok = ShowPathDetails();

		// Flag that we have changed the list so we should reparse it.
		m_ChangedList = TRUE;
	}
	
	return ok;
}
*/
/********************************************************************************************

>	BOOL PlugInsTab::DeletePath()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Deletes the currently selected path.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*
BOOL PlugInsTab::DeletePath()
{
	ERROR2IF(m_pManager == NULL,FALSE,"PlugInsTab::DeleteUnit called with no plug-ins manager pointer");

	// Get the currently selected item, if its valid then delete it
	WORD Index = 0;
	pPrefsDlg->GetValueIndex(_R(IDC_OPTS_PLUGINSLIST), &Index);
	BOOL Valid = FALSE;
	BOOL ok = FALSE;
	String_256 Path = pPrefsDlg->GetStringGadgetValue(_R(IDC_OPTS_PLUGINSLIST), &Valid, Index);
	PathName OldPath(Path);
	if (Valid)
	{
		ok = m_pManager->DeletePlugInPath(OldPath);
		// Now update the list of available pathnames
		// Do it the hard way to ensure we are in sync with the plug-in manager's list
		pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_PLUGINSLIST));
		ok = ShowPathDetails();

		// Flag that we have changed the list so we should reparse it.
		m_ChangedList = TRUE;
	}

	return ok;
}
*/

/******************************************************************************************************************

>	void PlugInsTab::BuyPluginsAction() 

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/09/2000
	Inputs:		
	Outputs:	
	Returns		
	Purpose:	Provides access from the 'Buy Plugins' button to Xara's plugin pages.
	Errors		If the browser could not be opened, an Error message box is diplayed.
	See Also	~ adapted from Ranbir's HelpXaraNewsAction() ~

******************************************************************************************************************/
/*
void PlugInsTab::BuyPluginsAction() 
{
	// Opens the default Browser on Xara's Plug-ins Page
	String_256 lpURL(_R(IDS_URL_PLUGINS));
	HINSTANCE hChild = ShellExecute(HWND_DESKTOP, "open", (TCHAR*)lpURL, NULL, NULL ,SW_SHOW);

	// If the function (ShellExecute()) fails, then an error value that is less than or equal to 32 is returned. 
	if ( (INT32)hChild <= THIRTY_TWO )
	{
		switch ( (INT32)hChild )
		{
			case	ERROR_FILE_NOT_FOUND:
			case	ERROR_PATH_NOT_FOUND: 
			case	SE_ERR_ACCESSDENIED:
			case	SE_ERR_ASSOCINCOMPLETE:
			{
				InformError(_R(IDS_NO_WEB_UTILITY),_R(IDS_OK));
			}
			break;
			default:
			{
				InformError(_R(IDS_NO_WEB_PAGE),_R(IDS_OK));
			}
			break;
		}
	}
}
*/


/********************************************************************************************

>	BOOL PlugInsTab::InitControls()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Inits all the controls in the dialog
				Called immediately after the bar is created to init drop lists, grey
				out gadgets, etc.
	SeeAlso:	-

********************************************************************************************/

BOOL PlugInsTab::InitControls()
{
//	ERROR2IF(m_pManager == NULL,FALSE,"PlugInsTab::InitControls called with no plug-ins manager pointer");

	EnableControls();

	return TRUE;
}
/********************************************************************************************

>	BOOL PlugInsTab::EnableControls()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Enables or disables the controls depending on the current context, i.e.
				the controls are disabled if there is no selection, etc.
	SeeAlso:	-

********************************************************************************************/

BOOL PlugInsTab::EnableControls()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PlugInsTab::EnableControls called with no dialog pointer");
//	ERROR2IF(m_pManager == NULL,FALSE,"PlugInsTab::EnableControls called with no plug-ins manager pointer");

 	BOOL enable = FALSE; //(pDocUnitList->FindFirstUserUnit() != NULL);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BUTTON_PLUGINS_SETUP),	enable);
/*	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DELETEPATH),	enable);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BUYPLUGS),		enable);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_NEWPATH),		enable);
*/
	return TRUE;
}           


/******************************************************************************************

>	BOOL PlugInsTab::ShowPathDetails()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Shows the list of pathnames in the list control.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/
/*
BOOL PlugInsTab::ShowPathDetails()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PlugInsTab::ShowPathDetails called with no dialog pointer");
	// Go through the list of pathnames stored in the plug-in manager
	// and show these on our list control
	String_256 Details;
	BOOL EndOfList = FALSE;
	INT32 Index = 0;
	PlugInPath * pPath = m_pManager->GetFirstPathName();
	while (pPath)
	{
		Details = pPath->GetPathNameAsString();
		pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_PLUGINSLIST), &Details, EndOfList, Index);
		Index++;
		pPath = m_pManager->GetNextPathName(pPath);
	}

	return TRUE;
}
*/
/******************************************************************************************

>	BOOL PlugInsTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Plug-ins section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PlugInsTab::InitSection()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"PlugInsTab::InitSection called with no dialog pointer");

/*	// Search the pathnames list in the plug-in manager
	m_pManager = GetApplication()->GetPlugInManager();
	ERROR2IF(m_pManager == NULL,FALSE,"PlugInsTab::InitSection no plug-ins manager");

	pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_PLUGINSLIST));
	BOOL ok = ShowPathDetails();

	BOOL ParseAtStartUp = m_pManager->GetParseAtStartUp();
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_PARSEATSTART), ParseAtStartUp);

	pPrefsDlg->EnableGadget(_R(IDC_OPTS_DELETEPATH),	FALSE);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BUYPLUGS),		TRUE);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_NEWPATH),		TRUE);
*/			
	BOOL ok = TRUE;
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_BUTTON_PLUGINS_SETUP),		TRUE);

	INT32 iLiveRes = 0;
	Camelot.GetPrefValue(TEXT("Effects"), TEXT("DefaultLivePixelsPerInch"), &iLiveRes);
	pPrefsDlg->BuildResDropList(_R(IDC_COMBO_LIVEEFFECT_RES), TRUE, TRUE, -1);
	pPrefsDlg->SetResDropListValue(_R(IDC_COMBO_LIVEEFFECT_RES), TRUE, iLiveRes);

	INT32 iLockedRes = 0;
	Camelot.GetPrefValue(TEXT("Effects"), TEXT("DefaultLockedPixelsPerInch"), &iLockedRes);
	pPrefsDlg->BuildResDropList(_R(IDC_COMBO_LOCKEDEFFECT_RES), FALSE, TRUE, -1);
	pPrefsDlg->SetResDropListValue(_R(IDC_COMBO_LOCKEDEFFECT_RES), FALSE, iLockedRes);

	BOOL bDefaultLocked = FALSE;
	Camelot.GetPrefValue(TEXT("Effects"), TEXT("DefaultLocked"), &bDefaultLocked);
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECK_NEW_EFFECTS_LOCKED), bDefaultLocked);


/*	BOOL bCacheingEnabled = FALSE;
	Camelot.GetPrefValue(TEXT("Rendering"), TEXT("Cacheing"), &bCacheingEnabled);
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECK_CACHE_ENABLED), bCacheingEnabled);

	INT32 iCacheRAMPercent = 0;
	Camelot.GetPrefValue(TEXT("Cache"), TEXT("CacheRAMPercent"), &iCacheRAMPercent);
//	pPrefsDlg->SetLongGadgetValue(_R(IDC_EDIT_CACHE_SIZE), iCacheRAMPercent);				// TODO: Should be percentage
	TCHAR Str[32];
	String_32 temp(_R(IDS_PERCENT_FORMAT));
	wsprintf(Str, temp, iCacheRAMPercent);
	String_32 PercentStr(Str);
	pPrefsDlg->SetStringGadgetValue(_R(IDC_EDIT_CACHE_SIZE), &PercentStr);

	CBitmapCache* pCache = GetApplication()->GetBitmapCache();
	if (pCache)
	{
		INT32 iCacheUsagePercent = ((UINT64)pCache->GetCurrentDataSize()*100)/pCache->GetMaximumDataSize();
//		pPrefsDlg->SetLongGadgetValue(_R(IDC_EDIT_CURRENTCACHE), iCacheUsagePercent);		// TODO: Should be percentage
		TCHAR Str[32];
		String_32 temp(_R(IDS_PERCENT_FORMAT));
		wsprintf(Str, temp, iCacheUsagePercent);
		String_32 PercentStr(Str);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_EDIT_CURRENTCACHE), &PercentStr);

//		pPrefsDlg->SetLongGadgetValue(_R(IDC_PROGRESS_CACHEUSAGE), iCacheUsagePercent);		// TODO: Should be percentage
	}
*/
	return ok;
}
