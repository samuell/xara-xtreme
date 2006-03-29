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

#include "camtypes.h"
#include "appprefs.h"
#include "optsinet.h"	// dialog/gadget ids
//#include "optsres.h"	// _R(IDS_PROXY)...
#include "app.h"		// Camelot object
#include "document.h"	// document object
#include "spread.h"		// spread object
#include "page.h"		// page object
//#include "webster.h"
#include "camnet.h"

//#include "resource.h"	// _R(IDS_CANCEL)#

CC_IMPLEMENT_DYNAMIC(InternetTab, OptionsTabs)   

/******************************************************************************************

>	BOOL InternetTab::CommitSection()

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/97
	Returns:	TRUE if succesful, FALSE otherwise 
	Purpose:	Effect any changes made to the Internet settings

******************************************************************************************/

BOOL InternetTab::CommitSection() 
{
	ERROR2IF(pPrefsDlg==NULL,FALSE,"GridTab::CommitSection() - pPrefsDlg==NULL");
	if (pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_INTERNET))==FALSE)
		return TRUE;	// just return with no error

	// Read in the new potential cache size
	BOOL Valid = TRUE;
	UINT32 lCacheSize = pPrefsDlg->GetMemoryGadgetValue(_R(IDC_CACHESIZE),1024,UINT32_MAX,_R(IDS_INVALIDCACHESIZE),&Valid);

	if ((lCacheSize < 500 * SIZEOFKILOBYTE) || (lCacheSize == UINT32_MAX))
	{
		InformWarning(_R(IDS_INVALIDCACHESIZE));

		if (lCacheSize < 500 * SIZEOFKILOBYTE)
		{
			lCacheSize = 500 * SIZEOFKILOBYTE;
		}
		else
		{
			lCacheSize = UINT32_MAX;
		}

		pPrefsDlg->SetMemoryGadgetValue(_R(IDC_CACHESIZE), lCacheSize);
	}

	if(!Valid) return FALSE;

	if (Valid && !DownloadCache::SetSize(lCacheSize))
	{
		InformError(_R(IDS_INSUFFICIENT_DISK_SPACE));
		return FALSE;
	}

	UINT32 lCacheUsage = DownloadCache::GetUsage();
	pPrefsDlg->SetLongGadgetValue(_R(IDC_CACHEUSAGE), lCacheUsage);

	// Sort out the connection type
	if (pPrefsDlg->GetBoolGadgetSelected(_R(IDC_14K)))
		InternetManager::SetConnectionType(CONNECTION_SLOWMODEM);
	else if (pPrefsDlg->GetBoolGadgetSelected(_R(IDC_28K)))
		InternetManager::SetConnectionType(CONNECTION_FASTMODEM);
	else if (pPrefsDlg->GetBoolGadgetSelected(_R(IDC_56K)))
		InternetManager::SetConnectionType(CONNECTION_X2);
	else if (pPrefsDlg->GetBoolGadgetSelected(_R(IDC_ISDN)))
		InternetManager::SetConnectionType(CONNECTION_ISDN);
	return TRUE;
}


/********************************************************************************************

>	BOOL InternetTab::HandleMsg(DialogMsg* Msg)

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/07/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Internet options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL InternetTab::HandleMsg(DialogMsg* Msg)
{
	ERROR2IF(Msg == NULL,FALSE,"Null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_INTERNET));	// The Page page identifier
	if (!ok)
		return TRUE;		// page not present

	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
			ok = InitSection();
			if (!ok)
				InformError();
			break;
		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			OptionsTabs::UngreyApplyNow();
			break;
		case DIM_LFT_BN_CLICKED:
			switch (Msg->GadgetID)
			{
				case _R(IDC_FLUSHCACHE):
					{
						DownloadCache::Flush();
						UINT32 lCacheUsage = DownloadCache::GetUsage();
						pPrefsDlg->SetMemoryGadgetValue(_R(IDC_CACHEUSAGE), lCacheUsage);
					}
					break;
				default:
					OptionsTabs::SetApplyNowState(TRUE);
			}
			break;
	}	
	return TRUE;
}  


/******************************************************************************************

>	BOOL GridTab::InitSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Page section of the options dialog box.
				This section includes the:-
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs; UpdateSection;

******************************************************************************************/

BOOL InternetTab::InitSection()
{
	ERROR2IF(pPrefsDlg == NULL,FALSE,"InitSection called with no dialog pointer");

	// Fill in the data regardless of whether the proxy is enabled or not
	// Now get the current settings
	TCHAR szHostName[INTERNET_MAX_PATH_LENGTH];
	SERVENT proxyEntry = {szHostName, NULL, 0x0000, NULL};
	bool bProxyEnabled;

	String_256 strProxyNote(_T(""));
	
	if (!InternetManager::GetProxyServer(&proxyEntry, &bProxyEnabled))
	{
		// failed to read proxy settings
		String_256 strProxyError(_R(IDS_PROXY_SUB_ERROR));
		strProxyNote.MakeMsg(_R(IDS_PROXY_NOTE), static_cast<TCHAR *>(strProxyError));
	}
	else
	{
		// read settings ok
		if (bProxyEnabled)
		{
			// proxy server enabled
			String_256 strProxyEnabled;
			strProxyEnabled.MakeMsg(_R(IDS_PROXY_SUB_ENABLED), proxyEntry.s_name, proxyEntry.s_port);
			strProxyNote.MakeMsg(_R(IDS_PROXY_NOTE), static_cast<TCHAR *>(strProxyEnabled));
		}
		else
		{
			// proxy server disabled
			String_256 strProxyDisabled(_R(IDS_PROXY_SUB_DISABLED));
			strProxyNote.MakeMsg(_R(IDS_PROXY_NOTE), static_cast<TCHAR *>(strProxyDisabled));
		}
	}

	pPrefsDlg->SetStringGadgetValue(_R(IDC_PROXY_DETAILS), &strProxyNote);

	// Sort out the connection type
	UINT32 nSelID;
	switch (InternetManager::GetConnectionType())
	{
		case CONNECTION_SLOWMODEM:
			nSelID = _R(IDC_14K);
			break;
		case CONNECTION_FASTMODEM:
			nSelID = _R(IDC_28K);
			break;
		case CONNECTION_X2:
			nSelID = _R(IDC_56K);
			break;
		case CONNECTION_ISDN:
			nSelID = _R(IDC_ISDN);
	}
	pPrefsDlg->SetBoolGadgetSelected(nSelID, TRUE);

	
	// Sort out the download cache section.
	UINT32 lCacheUsage = DownloadCache::GetUsage();
	pPrefsDlg->SetLongGadgetValue(_R(IDC_CACHEUSAGE), lCacheUsage);

	UINT32 lCacheSize = DownloadCache::GetSize();
	pPrefsDlg->SetMemoryGadgetValue(_R(IDC_CACHESIZE), lCacheSize);
	return TRUE;
}


