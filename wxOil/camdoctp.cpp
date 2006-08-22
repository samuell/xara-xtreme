// $Id: wxOil/camdoctp.cpp, 1, 01-Jan-2006, Anonymous $
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

This license does not apply to the clipart within the directory
filters/clipart/OpenClipart. These have kindly been put into the
public domain by their authors. See http://www.openclipart.org/


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
#include "camdoctp.h"			// document template
#include "camdoc.h"				// document
//#include "ccmdikid.h"			// view frame
//#include "scrcamvw.h"  			// screen view
#include "appstate.h"
#include "errors.h"
//#include "mfccopy.h"
#include "camelot.h"
#include "clipint.h"

IMPLEMENT_CLASS(CCamDocTemplate, wxDocTemplate);
DECLARE_SOURCE("$Revision: 25 $");



CCamDocTemplate::CCamDocTemplate( wxDocManager* pManager, const wxString& descr, const wxString& filter, 
		const wxString& dir, const wxString& ext, const wxString& docTypeName, const wxString& viewTypeName, 
		wxClassInfo* docClassInfo, wxClassInfo* viewClassInfo )
 : wxDocTemplate( pManager, descr, filter, dir, ext, docTypeName, viewTypeName, docClassInfo, viewClassInfo )
{
#if 0
	if (m_hMenu != NULL)
	{
		::DestroyMenu(m_hMenuShared);
		m_hMenuShared = m_hMenu;
	}
		
	if (m_hAccelTable != NULL)
		m_hAccelTable = m_hAccelTable;
	
	#ifdef _DEBUG
		// warnings about missing components (don't bother with accelerators)
		if (m_hMenuShared == NULL)
			TRACE1("Warning: no shared menu for document template #%d\n", nIDResource);
	#endif //_DEBUG

#if _MFC_VER >= 0x400
	// Strangeness we need to do here to create the DocManager as our derived version instead
	// of MFC's version, so we can over-ride DoPromptFilename() - see mfccopy.cpp/h. (Tim)
	// This code fragment taken from mfc\src\doctempl.cpp (MFC4).

	// Er, Richard here... Defining _AFXDLL and using the MD version of MFC seems not to
	// contain this bStaticInit stuff. This needs looking into...
	if (CDocManager::bStaticInit)
	{
		if (CDocManager::pStaticDocManager == NULL)
			CDocManager::pStaticDocManager = new CamelotDocManager;
	}
#endif // _MFC_VER >= 0x400
#endif
}



/********************************************************************************************

>	CDocTemplate::Confidence CCamDocTemplate::MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/95
	Purpose:	This function has been overiden from its MFC version. It is actually
				identical apart from the bit that checks to see if the document is already
				open. The MFC version of the function goes and looks at the files involved
				by calling AfxComparePaths(), but we just compare the paths. This stops the
				floppy drive from being accessed all the time if you have a document opened
				from a floppy drive (it also stops network problems if the conection to
				an open document goes away). I am not too sure what all the params mean.

********************************************************************************************/

#ifndef _MAC
CCamDocTemplate::Confidence CCamDocTemplate::MatchDocType(LPCTSTR lpszPathName, wxDocument*& rpDocMatch)
#else
CCamDocTemplate::Confidence CCamDocTemplate::MatchDocType(LPCTSTR lpszPathName, DWORD dwFileType,
													   wxDocument*& rpDocMatch)
#endif
{
	ASSERT(lpszPathName != NULL);
	rpDocMatch = NULL;

	// go through all documents
	wxList&				listDocuments = GetDocumentManager()->GetDocuments();
	wxNode*				pNode = listDocuments.GetFirst();
	while (pNode != NULL)
	{
		// Get the next document
		CCamDoc* pDoc = (CCamDoc*)pNode->GetData();

	#if (_OLE_VER >= 0x200)
		// Don't consider embedded or hidden docs.
		if (pDoc->IsEmbedded() || pDoc->GetKernelDoc()->IsAHiddenDoc()) continue;
	#endif

		// Find out the names of the old and new documents
		wxString OldDocName  = pDoc->GetFilename();
		wxString OrigDocName = wxString((LPCTSTR) pDoc->GetOriginalPath());
		wxString NewDocName  = wxString(lpszPathName);
		OldDocName.MakeLower();
		OrigDocName.MakeLower();
		NewDocName.MakeLower();

		// Test if the docs refer to the same existing .ART file or if the two docs are based
		// on the same existing non-.ART file.
		BOOL fSameArtFiles = (OldDocName == NewDocName);
		BOOL fSameNonArtFiles = (OldDocName.IsEmpty() && OrigDocName == NewDocName);

		// see if they are the same
		if (fSameArtFiles || fSameNonArtFiles)
		{
			// already open
			TRACEUSER("JustinF", _T("File %s is already open\n"),  (LPCTSTR) lpszPathName);
			rpDocMatch = pDoc;
			return yesAlreadyOpen;
		}

		pNode = pNode->GetNext();
	}

	// see if it matches our default suffix
	if( !m_defaultExt.IsEmpty() )
	{
		// see if extension matches
		LPCTSTR			lpszDot = camStrrchr( lpszPathName, _T('.') );
		if( NULL != lpszDot && 
			0 == camStricmp( lpszDot + 1, m_defaultExt ) )
			return yesAttemptNative; // extension matches, looks like ours
	}

	// otherwise we will guess it may work
	return yesAttemptForeign;
}
