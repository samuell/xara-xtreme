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

#include "docview.h"
//#include "mario.h"
//#include "justin.h"

#include "nativeop.h"
#include "dbugtree.h"
#include "brushmsg.h"

#include "camdoc.h"
#include "errors.h"
#include "menuops.h"
#include "camelot.h"
#include "filedlgs.h"

IMPLEMENT_DYNAMIC_CLASS( CCamDoc, wxDocument )

// Setting this to some document will force all CCamDoc's created by the
// default constructor to attach to m_pForceDocument
Document *CCamDoc::s_pForceDocument = NULL;
BOOL CCamDoc::s_RemoveExistingOnNewDoc = FALSE;
PathName CCamDoc::ms_pathNextTemplateToUse;

/************************************************************************************
> 	CCamDoc::CCamDoc(Document* pKernelDoc = 0)

    Author: 	Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
    Created:	ages ago
	Inputs:		pKernelDoc		---		the kernel Document to attach to this
										CCamDoc, or m_pForceDoc, a global override
										of all default creations.
    Purpose:    Creates a CCamDoc, optionally attached to a given pKernelDoc or
				if pKernelDoc is null, a new Document.
	Notes:		m_pFor
	Scope:		protected
	Errors:		May throw an MFC memory exception!
************************************************************************************/

CCamDoc::CCamDoc( Document *pKernelDoc /*= NULL*/ )
  :	m_pKernelDoc( NULL ),
	m_fIsReadOnly( false ),
	m_fIsACopy( false ),
	m_fIsUntouched( false ),
	m_bIsModifiable( false ),
#if (_OLE_VER >= 0x200)
	m_fExportPreview( true ),
	m_fIsVisible( false ),
	m_fDoneInitialShow( false ),
#endif
	m_lpszInitialZoomOp( NULL ),
	m_fDoneNewDoc( false )
{
#if (_OLE_VER >= 0x200)
	// For most apps enable compound documents.
	EnableCompoundFile();
	EnableAutomation();
#endif

	// Graham 22/10/97: Set the template path name as empty
	SetTemplate();

	// If the default was passed then use the 'forced' one (which may also
	// be null, meaning 'create a Document'.
	if( NULL == pKernelDoc )
		pKernelDoc = s_pForceDocument;

	// Connect with a ready-made kernel Document type?
	if( pKernelDoc )
	{
		// Attach this CCamDoc to the Document type.
		ERROR3IF(pKernelDoc->GetOilDoc(),
					"Document already attached in CCamDoc::CCamDoc");
		m_pKernelDoc = pKernelDoc;
	}
	else
	{
		// Default - connect to a newly-created standard kernel Document.
		m_pKernelDoc = new Document;
		if( !m_pKernelDoc )
			throw CMemoryException();
	}

	// Try to initialise the kernel Document type and attach it to this CCamDoc.
	if( !m_pKernelDoc->Init( this ) )
	{
		// Didn't work - delete the Document object and throw an exception
		delete m_pKernelDoc;
		throw CMemoryException();
	}

	// Initialise the document flags.
	SetModifiedFlag(FALSE);

#if (_OLE_VER >= 0x200)
	// All done - now claim the OLE 2.0 libraries.
	AfxOleLockApp();
#endif
}

/************************************************************************************
> 	CCamDoc::~CCamDoc()

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/5/93
    Purpose:    Destructor - destroys the Document associated with this OIL document.
************************************************************************************/

CCamDoc::~CCamDoc()
{
	// Get rid of the Document associated with this CCamDoc.
//	wxDocument::SetNoCurrent();
	m_pKernelDoc->SetCurrent();
	delete m_pKernelDoc;
	m_pKernelDoc = NULL;
	
	// Under OLE 2.0 we must release our claim on the OLE libraries.
#if (_OLE_VER >= 0x200)
	AfxOleUnlockApp();
#endif
}/////////////////////////////////////////////////////////////////////////////
// CCamDoc document command handlers.

/********************************************************************************************
>	BOOL CCamDoc::GeneralOpenDocument(CCLexFile* pFile, UINT32 nPrefFilter)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		pFile			-	the file to load the document from.
				nPrefFilter		-	the preferred filter to use, usually FILTERID_GENERIC.
									Pass -1 to use the last filter the user chose in a
									file dialog box.
	Returns:	TRUE if the document was opened ok, FALSE if there was a problem
	Purpose:	This function will load a document with the name provided to help out the
				OnNewDocument and the OpOpenDocument, since they do mostly the same thing.
	SeeAlso:	CCamDoc::OnOpenDocument; CCamDoc::OnNewDocument
********************************************************************************************/

bool CCamDoc::GeneralOpenDocument( CCLexFile *pFile, UINT32 nPrefFilter )
{
	// We need to make sure that the Current DocView is the same as the Selected DocView here
/*	TRACEUSER( "JustinF", _T("In CCamDoc::GeneralOpenDocument(%p, %u)\n"),
				(LPVOID) pFile, nPrefFilter);
*/	DocView			   *pDocView = DocView::GetCurrent();
	DocView			   *pOldSelView = DocView::GetSelected();
	if (pDocView != NULL)
	{
		// There was a current DocView, see if its got anything to do with out new doc
		// it should do
		if (pDocView->GetDoc() == m_pKernelDoc)
		{
			// make it the selected view
			Document::SetSelectedViewAndSpread( m_pKernelDoc, pDocView, NULL );
		}
	}

	// Make sure these point to this doc and this view.
	m_pKernelDoc->SetCurrent();
	if (m_pKernelDoc->GetFirstDocView() != NULL) m_pKernelDoc->GetFirstDocView()->SetCurrent();

	// Tell the Doc we are not importing the file
	if (m_pKernelDoc!=NULL) m_pKernelDoc->SetIsImporting(FALSE);

	// Do the load.
	BOOL IsImportOK = InvokeNativeFileOp(OPTOKEN_NATIVELOAD, pFile, nPrefFilter);

	// Make sure the colour bar is not left in a mess
	//GetMainFrame()->UpdateWindow();

	// Set the flag back to its default value
	if (m_pKernelDoc != NULL) m_pKernelDoc->SetIsImporting(TRUE);

	// See if the Import worked OK
	if (!IsImportOK)
	{
		// Find out what the old doc was
		Document* pDoc = NULL;
		if (pOldSelView != NULL) pDoc = pOldSelView->GetDoc();
		
		// Set the old document and view up and fail.
		Document::SetSelectedViewAndSpread(pDoc, pOldSelView, NULL);
		return FALSE;
	}

	// Make this document the selected document (this used to be done in document::Init but
	// was moved here for technical reasons
	Document::SetSelectedViewAndSpread(m_pKernelDoc, 0, 0);	

	// If we had a doc, make sure that the Operation History is emptied out
	if (m_pKernelDoc != NULL && !m_pKernelDoc->EmptyOperationHistory()) return FALSE;

	// Seems to have worked (for now).
	return TRUE;
}

/********************************************************************************************
>	BOOL CCamDoc::LoadDefaultDocument()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Returns:	TRUE if successful.
	Purpose:	Loads the default document.  Called by CCamDoc::OnNewDocument etc.
********************************************************************************************/

bool CCamDoc::LoadDefaultDocument()
{
	// If this flags gets set to TRUE it means we have had to 'fake' the embedded doc
	// from the normal doc template.  In this case, quarter it's size.
//	TRACEUSER( "JustinF", _T("In CCamDoc::LoadDefaultDocument\n"));
#if (_OLE_VER >= 0x200)
	BOOL fQuarterDoc = FALSE;
#endif
	
	//Graham 22/10/97: First get the path of the template
	PathName			pathTemplateToUse = GetTemplate();

	String_256			Path = pathTemplateToUse.GetPath();

	TCHAR			   *Filename = NULL;
	String_256			Location = pathTemplateToUse.GetLocation( TRUE );
	String_256			File = pathTemplateToUse.GetFileName( TRUE );

	// See if the file exists
	if (Path != String_256(_T("")));
	{
		// In braces so file closes when res goes out of scope
		CamResource res;
		wxFSFile* f = res.OpenwxFSFile((TCHAR *)Path);
		if (f)
			Filename = (TCHAR*) Path;
	}
#if (_OLE_VER >= 0x200)		
	else if (IsEmbedded())
	{
		// Try to load the normal one instead and quarter it's size
		File.Load(_R(IDS_DEFAULTDOCNAME));

		Path = Location;
		Path += File;
//		TRACEUSER( "JustinF", _T("\t- trying template doc %s\n"), (LPCTSTR) Path);
	
		// See if the file exists
		CFileStatus Status;
		if (CFile::GetStatus((TCHAR*) Path, Status) != 0)
		{
			// It did exist, so set the filename to load to the file we have found
			Filename = (TCHAR*) Path;
		}

		// Remember to quarter it's size.
		fQuarterDoc = TRUE;
	}

#endif

	// Hide the user's preferred filter.
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	INT32 nOldVal = BaseFileDialog::SelectedFilter; 
	BaseFileDialog::SelectedFilter = 0;
#endif

	CamResource res;
	// If we can't find the disk-based template, load the one from resources instead.
	CCLexFile		   *pFile;
	if (Filename)
	{
		if( wxFile::Exists( Filename ) )
			// Found a disk file, use that.
			pFile = new CCDiskFile(PathName(Filename), ios::in | ios::binary);
		else
			// Else we fall back to memory
			pFile = res.Open(Filename);
	}
	else
	{
		// Previously, Xara X (in Corel Xara times) would load a default template 
		// from its resources (defdoc.ppp) when it could not find the real default
		// template.  For a variety of reasons, this does not work in Xara X.  The new 
		// behaviour shows an error message requesting that the user fix the problem 
		// (by saving a new default template).  - Jonathan 4/9/2000
		ERROR1(FALSE, _R(IDS_TEMPLATE_NOT_FOUND));
	}

	// Check if we managed to allocate a file.
	if (!pFile)
	{
//		TRACE( _T("Not enough memory for a new file in CCamDoc::LoadDefaultDocument\n"));
//		AfxThrowMemoryException();
		return FALSE;
	}

	// Was the file opened OK?
	if (!pFile->good())
	{
//		delete pFile; - Freeing CamResource does this
		ERROR1(FALSE, _R(IDT_IMPORT_NOTFOUND));
	}
	
	// Do the load.
	m_pKernelDoc->SetTemplateLoading(TRUE);
	//	TRACEUSER( "JustinF", _T("\t- about to call CCamDoc::GeneralOpenDocument\n"));
	BOOL fOK = GeneralOpenDocument(pFile, FILTERID_GENERIC);
//	delete pFile; - Freeing CamResource does this
	m_pKernelDoc->SetTemplateLoading(FALSE);

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Restore the user's preferred filter.
	BaseFileDialog::SelectedFilter = nOldVal;
#endif

	// Did it work?
	if (!fOK)
	{
//		TRACE( _T("GeneralOpenDocument failed in DefaultDocumentRequired - dial 999\n"));
		return FALSE;
	}

#if (_OLE_VER >= 0x200)

	// Did we load the embedded template from the default doc in resources?
	if (fQuarterDoc)
	{
		// Yes, so quarter the size of the page, to make a neat A6-ish embedded drawing.
		// We must set the new page size directly, without generating undo records.  First,
		// get the document's first spread, ie. its page.
//		TRACEUSER( "JustinF", _T("\t- quartering size of page as doc is embedded\n"));
		Spread* pSpread = GetKernelDoc()->FindFirstSpread();
		ERROR2IF(!pSpread, FALSE, "Can't find a Spread in CCamDoc::LoadDefaultDocument");

		// Scale all the objects on the page, if there are any, by a factor of 4.
		ERROR2IF(!ScaleContents(0.5, 0.5), FALSE,
					"Can't scale document contents in CCamDoc::LoadDefaultDocument");

		// Get the the page settings that we just loaded.  Only the width and height
		// will be changed (and the aspect ratio will be preserved).
		PageResizeInfo pri;
		ERROR2IF(!pSpread->GetPageSize(&pri.Width, &pri.Height,
									   &pri.Margin, &pri.Bleed,
									   &pri.Dps, &pri.ShowDropShadow),
					FALSE, "GetPageSize failed in CCamDoc::LoadDefaultDocument");

		// Now all that's set-up, resize the spread by a factor of 1/2 x 1/2.
		pSpread->SetPageSize(pri.Width / 2, pri.Height / 2,
							 pri.Margin, pri.Bleed, pri.Dps, pri.ShowDropShadow);
	}

#endif
	// Success!
	return true;
}

/********************************************************************************************
>	PathName CCamDoc::GetTemplate() const

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/97
	Returns:	PathName - the template that was used to create this document.
	Purpose:	As above
	SeeAlso:	CCamDoc::LoadDefaultDocument
********************************************************************************************/

PathName CCamDoc::GetTemplate() const
{
#if (_OLE_VER >= 0x200)
	//First, is this an embedded document?
	if (IsEmbedded())
	{
		//Yes. So get the default templates path.
		PathName pathEmbeddedTemplatePath = GetApplication()->GetTemplatesPath();

		//And add "embedded.xar" on the end
		String_256 strEmbeddedTemplateFilename(_R(IDS_DEFAULT_EMBEDDED_DOCNAME));

		pathEmbeddedTemplatePath.SetFileNameAndType(strEmbeddedTemplateFilename);

		return pathEmbeddedTemplatePath;
	}
	else
	{
		//No. So return our member variable
		return m_pathTemplate;
	}
#else
	return m_pathTemplate;
#endif
}

/********************************************************************************************
>	void CCamDoc::SetTemplate(const PathName& pathToSet=PathName())

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/97
	Inputs:		pathToSet - the PathName to set as our default template
	Purpose:	As above
	SeeAlso:	CCamDoc::LoadDefaultDocument
********************************************************************************************/

void CCamDoc::SetTemplate( const PathName &pathToSet )
{
	m_pathTemplate = pathToSet;
}

/********************************************************************************************
>	static PathName CCamDoc::GetNextTemplateToUse() const

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/97
	Returns:	PathName - the value of ms_pathNextTemplateToUse
	Purpose:	To find the path of the next template to use
	SeeAlso:	CCamDoc::LoadDefaultDocument
********************************************************************************************/

PathName CCamDoc::GetNextTemplateToUse()
{
	if (ms_pathNextTemplateToUse.IsValid())
	{
		wxFSFile * f;
		{
			// In braces so closes on descope
			CamResource res;
			f = res.OpenwxFSFile((const TCHAR *)(ms_pathNextTemplateToUse.GetPath()));
		}
		if (f)
		{
			return ms_pathNextTemplateToUse;
		}
	}
	
	PathName pathToReturn(DocOps::GetDefaultDrawingTemplate());
	return pathToReturn;

}

/********************************************************************************************
>	static void CCamDoc::SetNextTemplateToUse(const PathName& pathToSet) 

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/97
	Inputs:		pathToSet - the path of the next template to use
	Purpose:	To set the path of the next template to use
	SeeAlso:	CCamDoc::LoadDefaultDocument
********************************************************************************************/

void CCamDoc::SetNextTemplateToUse(const PathName& pathToSet) 
{
	ms_pathNextTemplateToUse=pathToSet;
}



/********************************************************************************************
>	static BOOL CCamDoc::EnableRemoveUntouchedDocs()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/96
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Enable to idle-time removal of untouched documents.
				Called when the user creates new documents or opens existing documents from
				the highest possible level: menucmds.cpp.
********************************************************************************************/

BOOL CCamDoc::EnableRemoveUntouchedDocs()
{
PORTNOTE("other", "Removed idle processing");
#ifndef EXCLUDE_FROM_XARALX
	s_fRunIdleProcessing = TRUE;
#endif
	return TRUE;
}


/********************************************************************************************
>	virtual BOOL CCamDoc::OnNewDocument()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/3/94
	Returns:	TRUE if a new document is OK, FALSE otherwise.
	Purpose:	Called by MFC during the processing of a File/New command.  Before
				this is called MFC/kernel have already contructed new instances of
				the CCamDoc / Document classes.  Calls the kernel's Document and
				CCamDoc's base class to do the actual work.
	Errors:		-
	SeeAlso:	CCamDoc::OnOpenDocument; CCamDoc::OnSaveDocument; CCamDoc::DoNewDocument
********************************************************************************************/

bool CCamDoc::OnNewDocument()
{
	// Make sure the kernel is in the know.
	TRACEUSER( "JustinF", _T("In CCamDoc::OnNewDocument\n"));
//	ASSERT_VALID(this);

	//Graham 22/10/97: First set the template that this document is based on
	SetTemplate(GetNextTemplateToUse());

	SetNextTemplateToUse();

#if (_OLE_VER >= 0x200)
	// Make sure the doc is considered 'visible' unless it is embedded.
	if (!IsEmbedded()) m_fIsVisible = TRUE;
#endif

	// We must call the base class if OLE & MFC is to work properly.
	if (!wxDocument::OnNewDocument())
	{
		TRACE( _T("CCamDocBase::OnNewDocument failed - that's quite scary, isn't it?\n"));
		return FALSE;
	}

	// Set the prefix on the status-line.
//	StatusLine::SetPrefix(String_64(_R(IDS_WAIT_CREATING_DOC_PFX)));

	// Get the document's default title prefix, as set from resources by MFC.
	m_TitlePrefix = (LPCTSTR) GetTitle();

	// Try to initialise the document.
	if (!DoNewDocument())
	{
		// Failed somehow!
		TRACEUSER( "JustinF", _T("\t- DoNewDocument failed in CCamDoc::OnNewDocument\n"));
	//	StatusLine::SetDefaultPrefix();
		return FALSE;
	}

	// New documents can be discarded if they haven't been changed, and they need
	// a path name if they are to be saved.  Set the document's flags appropriately.
	m_pKernelDoc->MakeJustCreated();
	SetOriginalPath(TEXT(""));

	// Templates should also not ask whether they need to be saved in the old format,
	// so force this off regardless.
	m_pKernelDoc->SetLoadedAsVersion1File(FALSE);

	// Make sure the window titles accurately reflect the state of the document.
	UpdateTitle();
	if (s_RemoveExistingOnNewDoc)	RemoveExistingDocs();

	m_fIsUntouched = FALSE;				// Mark this document as having been "touched"
										// by the user (because he's told us to open it)
//	fRunIdleProcessing = TRUE;			// Flag the idle document processor that it should
										// scan for untouched docs and remove them.

	// Now that the document is stable, everything is selected and ready to go
	// we should now broadcast a message to this effect. This is so that things
	// like the options system can then update itself in the knowledge that
	// everything is fine and fabby rather than just sitting on doc selection
	// messages and wondering why it doesn't recieve a last one to say that 
	// everything has been set up..
	m_pKernelDoc->SetStable(TRUE);
	BROADCAST_TO_ALL(DocChangingMsg(m_pKernelDoc, DocChangingMsg::BORNANDSTABLE));

#if (_OLE_VER >= 0x200)
	// Make sure the first view onto this new document zooms to the page bounds.
	if (IsServer()) m_lpszInitialZoomOp = OPTOKEN_ZOOMSPREAD;
#endif

	// Tidy up.
	m_bIsModifiable = TRUE;
	SetModified(FALSE);
//	StatusLine::SetDefaultPrefix();
	return TRUE;
}

/********************************************************************************************
>	virtual BOOL CCamDoc::DoNewDocument()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/96
	Returns:	TRUE if successful.
	Purpose:	Called to load the default document etc by CCamDoc::OnNewDocument.  Does
				the actual load of the document data, without changin any flags etc.
	SeeAlso:	CCamDoc::OnNewDocument
********************************************************************************************/

bool CCamDoc::DoNewDocument()
{
	// Set up the kernel.
	m_pKernelDoc->SetCurrent();

	// If we've already 'new-ed' this document then bail out now.
	if (m_fDoneNewDoc) return TRUE;

	// Try to load the default document.
	if (!LoadDefaultDocument()) return FALSE;
	m_fDoneNewDoc = TRUE;

#if (_OLE_VER >= 0x200)
	// Embedded docs (OLE or hidden) don't export preview bitmaps.
	if (IsEmbedded()) SetExportPreview(FALSE);
#endif

	// Success.
	return TRUE;
}
/********************************************************************************************
>	virtual bool CCamDoc::OnSaveDocument(const wxString& filename)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/3/94
	Inputs:		A C string with the directory path / filename of the document.
	Returns:	TRUE for a successful save, FALSE otherwise
	Purpose:	Saves the document to the specified path.  Calls the kernel
				to do the actual work.
	SeeAlso:	CCamDoc::OnNewDocument; CCamDoc::OnOpenDocument; CCamDoc::DoSaveDocument
********************************************************************************************/

bool CCamDoc::OnSaveDocument(const wxString& filename)
{
	const TCHAR* pcszPathName = filename.c_str();
#ifdef STANDALONE

	// Do nothing on a standalone version
	return TRUE;

#else

//	ASSERT_VALID(this);
//	ASSERT(pcszPathName == 0 || AfxIsValidString(pcszPathName));

PORTNOTE("other", "Disabled OLE")
#ifndef EXCLUDE_FROM_XARALX
#if (_OLE_VER >= 0x200)
	#ifdef _DEBUG
		if (IsUserName("JustinF"))
		{
			TRACE( _T("In CCamDoc::OnSaveDocument("));	
			if (pcszPathName)
				TRACE( _T("FILE %s"), (LPCTSTR) pcszPathName);
			else
				TRACE( _T("STORAGE 0x%p"), (LPVOID) m_lpRootStg);	
			TRACE( _T(", %s)\n", (LPCTSTR) (m_bSameAsLoad ? TEXT("SAVE"))
													  : (m_bRemember ? TEXT("SAVE AS")
																	 : TEXT("SAVE COPY"))));
		}
	#endif
#endif

#if (_OLE_VER >= 0x200)
	// This patched in from COleLinkingDoc::OnSaveDocument.
	BOOL fRemember = m_bRemember;
#endif

	// TO DO: Check if we need to call the base-class, and how.
/*	if (!CCamDocBase::OnSaveDocument(pcszPathName))
	{
		TRACE( _T("CCamDocBase::OnSaveDocument failed - that's quite scary, isn't it?\n"));
		return FALSE;
	}
*/
	// Set the busy prefix.
//	StatusLine::SetPrefix(String_64(_R(IDS_WAIT_SAVING_DOC_PFX)));
#endif
	// Try the actual save.
	if (!DoSaveDocument(pcszPathName))
	{
//		StatusLine::SetDefaultPrefix();
		return FALSE;
	}

PORTNOTE("other", "Disabled OLE")
#ifndef EXCLUDE_FROM_XARALX
#if (_OLE_VER >= 0x200)
	// Update the moniker/registration if the name has changed.
	// Neville 8/8/97 Fix for bug 5592: MFC Assert while saving
	// Check that the document is not a copy before trying to do OLE bits
	// If its a copy then it should never be an OLE doc.
	if (fRemember && m_strMoniker != pcszPathName && !IsACopy())
	{
		// update the moniker/registration since the name has changed
		Revoke();
		RegisterIfServerAttached(pcszPathName, TRUE);
	}
#endif
#endif
	// Close the file, update the flags etc and return success.
	if (pcszPathName) SetOriginalPath(TEXT(""));
	m_fIsUntouched = FALSE;
	SetModified(FALSE);
	SetDocumentSaved(true);

#ifdef __WXMAC__
    wxFileName fn(pcszPathName) ;
    fn.MacSetDefaultTypeAndCreator() ;
#endif

//	StatusLine::SetDefaultPrefix();
	return TRUE;

#endif
}

/********************************************************************************************
>	virtual BOOL CCamDoc::DoSaveDocument(LPCTSTR pcszPathName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/96
	Inputs:		pcszPathName	---		path to the file to load (NULL means an embedding)
	Returns:	TRUE if successful.
	Purpose:	Helper function for CCamDoc::OnSaveDocument.  Does the actual saving of
				the data, without disturbing aby flags etc.
	SeeAlso:	CCamDoc::OnSaveDocument; CCamDoc::SaveToStorage
********************************************************************************************/

bool CCamDoc::DoSaveDocument( const wxString &strPathName )
{
	// Create and open the appropriate kind of file.
	CCLexFile* pFile = CreateCCFile( strPathName, ios::out | ios::trunc);
	if (!pFile) return FALSE;

	// Make sure the kernel is set up.
	m_pKernelDoc->SetCurrent();

	// Try to save the document.
// WEBSTER - markn 28/1/97
// Native file saving is to be .web, not .xar
#ifndef WEBSTER
	bool				fOK = FALSE != InvokeNativeFileOp( OPTOKEN_NATIVESAVE, pFile, FILTERID_NATIVE );
#else
	bool				fOK = FALSE != InvokeNativeFileOp( OPTOKEN_NATIVESAVE, pFile, FILTERID_WEB );
#endif // WEBSTER

	// Did that work?
	delete pFile;
	return fOK;
}

bool CCamDoc::OnOpenDocument( const wxString &strFilename )
{
#if (_OLE_VER >= 0x200)
	#ifdef _DEBUG
		if (IsUserName("JustinF"))
		{
			TRACE( _T("In CCamDoc::OnOpenDocument("));	
			if (pcszPathName)
				TRACE( _T("FILE %s)\n"), (LPCTSTR) pcszPathName);
			else
				TRACE( _T("STORAGE 0x%p)\n"), (LPVOID) m_lpRootStg);
		}
	#endif
#endif

	// Put the kernel in the know.
	m_bIsModifiable = FALSE;

#if (_OLE_VER >= 0x200)
	// Make sure the doc is considered 'visible' unless it is embedded.
	if (!IsEmbedded()) m_fIsVisible = TRUE;
#endif

	// Remember the selected document, for possible restoration.
	DocView* pOldSelView = DocView::GetSelected();

#if (_OLE_VER >= 0x200)
	// If this doc is not embedded then the user will want control after the server session.
	extern BOOL Bodge_NoRecentFileList;
	if (!IsEmbedded() && !Bodge_NoRecentFileList) AfxOleSetUserCtrl(TRUE);
#endif

	// Set the busy prefix.
//	StatusLine::SetPrefix(String_64(_R(IDS_WAIT_OPENING_DOC_PFX)));

	// Get the title from MFC - we use it as a prefix to the whole title.
	m_TitlePrefix = wxFileNameFromPath(strFilename); //GetTitle();

	// Try to open the document.
	if( !DoOpenDocument( strFilename ) )
	{
	#if !defined(EXCLUDE_FROM_RALPH)
		// If we failed then because we may have loaded a default document we will
		// have to ensure that the selection is put back to what it was before this
		// rather than what happens in the GeneralOpenDocument which is to put it back
		// to the selection before the function was called. If we don't do this then we
		// will loose the selection if the default doc was loaded.
		// Find out what the old doc was, if any.
		if (Document::GetSelected())
		{
			// Don't do this if no documents are open ('selected').
			Document* pDoc = NULL;
			if (pOldSelView != NULL) pDoc = pOldSelView->GetDoc();
			Document::SetSelectedViewAndSpread(pDoc, pOldSelView, NULL);
		}
	#endif

		// Damn, blown it again.
//		StatusLine::SetDefaultPrefix();
		return FALSE;
	}

#if (_OLE_VER >= 0x200)
	
	// Don't try to re-register copies of already registered docs.
	if (!IsACopy())
	{
		// Always register the document before opening it.
		Revoke();
		if (!RegisterIfServerAttached(pcszPathName, FALSE))
		{
			// always output a trace (it is just an FYI -- not generally fatal)
			TRACEUSER( "JustinF", _T("Warning: unable to register moniker '%s' as running\n"),
						(LPCTSTR) pcszPathName);
		}	
	}

#endif

	// Record the path we opened in a preference so it survives sessions
	BaseFileDialog::DefaultOpenFilePath = AfxGetApp().GetDocumentManager()->GetLastDirectory();

	SetOriginalPath( strFilename.c_str() );
	
	if( s_RemoveExistingOnNewDoc )
		RemoveExistingDocs();

	m_fIsUntouched = FALSE;				// Mark this document as having been "touched"
										// by the user (because he's told us to open it)
//	fRunIdleProcessing = TRUE;			// Flag the idle document processor that it should
										// scan for untouched docs and remove them.

	// Now that the document is stable, everything is selected and ready to go
	// we should now broadcast a message to this effect. This is so that things
	// like the options system can then update itself in the knowledge that
	// everything is fine and fabby rather than just sitting on doc selection
	// messages and wondering why it doesn't recieve a last one to say that 
	// everything has been set up..
	m_pKernelDoc->SetStable( TRUE );
	BROADCAST_TO_ALL( DocChangingMsg( m_pKernelDoc, DocChangingMsg::BORNANDSTABLE ) );

#if (_OLE_VER >= 0x200)

#ifdef _DEBUG
	DocRect drBounds;
	GetPageBounds(&drBounds);
	TRACEUSER( "JustinF", _T("\t- doc is %ld x %ld HIMETRIC\n"),
				(INT32) CCamSrvrItem::MPtoHM(drBounds.Width()),
				(INT32) CCamSrvrItem::MPtoHM(drBounds.Height()));
#endif

	// Make sure the first view onto this already-existing document *doesn't* auto-zoom.
	m_lpszInitialZoomOp = 0;

#endif

	SetFilename( strFilename, true );
    SetTitle(wxFileNameFromPath(strFilename));
	Modify(false);
	UpdateAllViews();

	// Tidy up.
	m_bIsModifiable = TRUE;
	SetModified(FALSE);

#if defined(_DEBUG)
	if ( Error::IsUserName("Gavin") )
		DebugTreeDlg::TweeDump( GetKernelDoc() );
#endif

//	StatusLine::SetDefaultPrefix();
	return TRUE;
}

/********************************************************************************************
>	virtual BOOL CCamDoc::DoOpenDocument(LPCTSTR pcszPathName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/96
	Inputs:		pcszPathName	---		path to file to open (NULL means an embeddeding).
	Returns:	TRUE if successful.
	Purpose:	Helper function to CCamDoc::OnOpenDocument.  Does the actual load of the
				data, without disturbing any flags etc.
	SeeAlso:	CCamDoc::OnOpenDocument; CCamDoc::LoadFromStorage
********************************************************************************************/

bool CCamDoc::DoOpenDocument( const wxString &strFilename )
{
	// Create and open the appropriate kind of file.
	CCLexFile		   *pFile = CreateCCFile( strFilename.c_str(), ios::in );
	if (!pFile) return FALSE;

	// Set up the kernel.
	m_pKernelDoc->SetCurrent();

	// Try to load the document and it's item, if it's embedded.
	bool				fOK = false;
	try
	{
	#ifndef STANDALONE
		// Before we open the file we must ask the filter system to work out what filter is
		// required to load the file, ask that filter if it requires a default document or not
		// and if it says yes then load the default document in. Required because if a file such
		// as a bitmap is loaded the user would expect to see this loaded into the default
		// document, as this file format defines no document structure, otherwise the default
		// constructed document would be used.
		// Only returns False if a problem happened in the testing and loading process.
		// -- Standalone
		// Don't do it on a standalone version otherwise the items get loaded on top of the  
		// document with all the info on rather than a blank page.
		// Note the selected document before we start anything
//		DocView* pOldSelView = DocView::GetSelected();
		if (!DefaultDocumentRequired(pFile, FILTERID_USERCHOICE))
		{
			TRACE( wxT("DefaultDocumentRequired failed in CCamDoc::OnOpenDocument - doh?\n") );
			delete pFile;
			return FALSE;
		}
	#endif
		
		// Try to load the file.
		m_pKernelDoc->SetTemplateLoading(FALSE); // Note that it is not a template being loaded
		fOK = GeneralOpenDocument(pFile, FILTERID_USERCHOICE);

	#if (_OLE_VER >= 0x200)
		// Embedded docs (OLE or hidden) don't export preview bitmaps.
		if (IsEmbedded()) SetExportPreview(FALSE);
	#endif
	}
	catch( ... )
	{
		// Tidy up.
		delete pFile;
		return FALSE;
	}

	// Did this work?
	delete pFile;
	return fOK;
}

/********************************************************************************************
>	BOOL CCamDoc::DefaultDocumentRequired(CCLexFile* pFile, UINT32 nPrefFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/95
	Inputs:		pFile			-	file to check
	Returns:	TRUE if the process went ok, FALSE if there was a problem
	Purpose:	Given the pathname that we have been asked to open and before we actually go
				and open the file, we must ask the filter system to work out what filter is
				required to load the file, ask that filter if it requires a default document
				or not and if it says yes then load the default document in.
				This is required because if a file such as a bitmap is loaded the user would
				expect to see this loaded into the default document, as this file format
				defines no document structure, otherwise the default constructed document
				would be used and this is one that is never seen elsewhere by the user.
				Other file types apart form bitmaps might require the same treatment,
				examples at present are Corel palette files and Windows metafiles.
				Formats which define document structures, such as our native format and the
				EPS formats will be changing the default constructed document given
				information in the format and so there is no point in loading the default
				document. Would cause problems anyway as things like colours would need
				clearing out etc.
				Needs to be done here as otherwise we would be right in the middle of the
				loading process before we decide what filter is required and hence would be
				calling effectively the same code re-enterantly!
				Only returns False if a problem happened in the testing and loading process.
		
	Errors:		-
	SeeAlso:	CCamDoc::GeneralOpenDocument; CCamDoc::OnOpenDocument;
				Filter::IsDefaultDocRequired;
********************************************************************************************/

bool CCamDoc::DefaultDocumentRequired( CCLexFile *pFile, UINT32 nPrefFilter )
{
#if defined(EXCLUDE_FROM_RALPH)
	return true;
#else

	// NULL pathname implies that we can't do any checking so return immediately
	// Shouldn't happen, maybe we should error?
//	TRACEUSER( "JustinF", _T("In CCamDoc::DefaultDocumentRequired(%p)\n"), (LPVOID) pFile);
	PathName			pth = pFile->GetPathName();
	if (!pth.IsValid())
	{
//		TRACEUSER( "JustinF", _T("\t- file has no valid path, assuming it's OK\n"));
		return TRUE;
	}

#ifndef EXCLUDE_FROM_XARALX
PORTNOTE("other", "Assuming we actually do need a default document rather than asking the filter system");
	// Find out the position of the filter selected by the user in the open dialog
	INT32 SelectedPos = BaseFileDialog::SelectedFilter;

	// Do we know which filter was used (we know nothing about things in the recent file list)
	Filter* pFilter = Filter::GetFirst();
	if (nPrefFilter != FILTERID_USERCHOICE || SelectedPos == 0)
	{
		// We know nothing. We will have to go and have a look at all the possibles
		// We will find the Filter Family and ask it to try and load the file
		// Find the filter that the user chose.
		UINT32 nID = (nPrefFilter != FILTERID_USERCHOICE) ? nPrefFilter : FILTERID_GENERIC;
		while (pFilter != NULL && pFilter->FilterID != nID)
		{
			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}
	}
	else
	{
		// We know which type of filter the user had selected in the file dialog
		// Find the filter that the user chose
		while (pFilter != NULL)
		{
			// This is the filter?
			if (pFilter->GetFlags().CanImport && 
				pFilter->pOILFilter->Position == SelectedPos)
					break;

			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}
	}
	
	// Check that the Filter existed
	if (pFilter == NULL)
	{
		// It did not...
		TRACE( _T("Can't find filter #%u in CCamDoc::DefaultDocumentRequired\n"),
				(UINT32) nPrefFilter);
		InformError(_R(IDT_CANT_FIND_FILTER));
		return FALSE;
	}

	// Ask the filter or filter family about whether a default document is required.
	// If it is a filter family then we will have to work out what filter is actually
	// required and hence ask that filter the qeustion.
	if (!pFilter->IsDefaultDocRequired(pth.GetPath())) return TRUE;
#else
	return TRUE;
#endif

	//Graham 22/10/97: First set the template that this document is based on
	SetTemplate(GetNextTemplateToUse());

	SetNextTemplateToUse();

	// Try to load the default document.
	return LoadDefaultDocument();
#endif
}


/********************************************************************************************
>	BOOL CCamDoc::RemoveExistingDocs()

	Author:         Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:        26/5/95
	Returns:        TRUE if successful, FALSE otherwise
	Purpose:        Removed existing documents, when a new document is created.
********************************************************************************************/

BOOL CCamDoc::RemoveExistingDocs()
{
	if (!s_RemoveExistingOnNewDoc ||
			Camelot.Documents.GetCount() < 2 ||
			AskQuestion(_R(IDS_CLOSEOTHERDOCS), _R(IDS_KEEPDOCS), _R(IDS_CLOSEDOCS), 0, 0, 2, 2) != 2)
	{
		// Nothing to do.
		return TRUE;
	}

	// Delete any other documents still associated with this application
	Document *pDoc;
	Document *pRootDoc = m_pKernelDoc;
	Document *pNextDoc = (Document*) Camelot.Documents.GetNext(m_pKernelDoc);

	while (pNextDoc != NULL)
	{
		if (pNextDoc != m_pKernelDoc)
			pNextDoc->GetOilDoc()->OnCloseDocument();   // This will remove it from the list

		// User may have pressed 'Cancel' so we need to check that the doc
		// has gone away
		pDoc = pNextDoc;
		pNextDoc = (Document*) Camelot.Documents.GetNext(pRootDoc);

		if (pNextDoc == pDoc)
		{
			// User must have pressed 'Cancel ...
			// So skip this doc in future
			pRootDoc = pDoc;
			pNextDoc = (Document*) Camelot.Documents.GetNext(pRootDoc);
		}
	}

	Document::SetSelectedViewAndSpread(m_pKernelDoc);
	DocView::GetSelected()->SetCurrent();
	return TRUE;
}


/********************************************************************************************

>	virtual void CCamDoc::SetTitle(LPCTSTR lpszTitle)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Returns:	-
	Purpose:	To set a new title for the document. Overide the parent class versions so we
				can do things like send a message to inform people that things have changed.
	SeeAlso:	GetKernelTitle(); CDocument::SetTitle();

********************************************************************************************/

void CCamDoc::SetTitle(LPCTSTR lpszTitle)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Make a note of the current document's title
	wxString	EntryTitle = GetTitle();

	// Set the new Title Prefix
	m_TitlePrefix = String_256(lpszTitle);

	// We now have to go and work out what extra bits we want to appear after the actual
	// title, such as Modified, Read Only flags etc.
	// Load the suffix separator string.  Clear the string used to build up a suffix to
	// the title.
	String sep(_R(IDS_DOC_SUFFIX_SEPARATOR));
	String_256 sfx;
	sfx.Empty();

	// Check the "document is modified" flag.
	BOOL fSuffixExists = FALSE;
	if (IsModifiable() && !GetKernelDoc()->IsAHiddenDoc() && IsModified())
	{
		fSuffixExists = TRUE;
		sfx += String(_R(IDS_DOC_MODIFIED_MARK));
	}

	// Check the "document is read-only" flag.
	if (IsReadOnly())
	{
		if (fSuffixExists)
		{
			sfx += sep;
		}
		else
		{
			fSuffixExists = TRUE;
		}
		sfx += String(_R(IDS_DOC_READ_ONLY_MARK));
	}

	// Check the "document is a copy" flag.
	if (IsACopy())
	{
		if (fSuffixExists)
		{
			sfx += sep;
		}
		else
		{
			fSuffixExists = TRUE;
		}
		sfx += String(_R(IDS_DOC_COPY_MARK));
	}

	// Get the document's title, as looked after by MFC.
	String_256 title = m_TitlePrefix;

	// If a suffix exists, format and append the it to the title.
	if (fSuffixExists)
	{
		String_256 tmp;
		tmp.MakeMsg(_R(IDS_DOC_SUFFIX_FORMAT), &sfx);
		title += tmp;
	}

	// Now we have the complete title that we want in the title bar, including
	// the name of the document and all the flags, so set it in MFC
	wxDocument::SetTitle( (const TCHAR *)title );

	// If the document's title has been changed then broadcast to the kernel to
	// let everyone know about the change.
	// Do not send it if TitlePrefix is empty as we seem to get some random settings
	// during say document opening where we will get people to update with say "[Modified]".
	if (!m_TitlePrefix.IsEmpty() && (EntryTitle != GetTitle()))
	{
		BROADCAST_TO_ALL( DocChangingMsg( m_pKernelDoc, DocChangingMsg::TITLECHANGED ) );
	}
#endif
}

/********************************************************************************************

>	virtual void CCamDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Returns:	-
	Purpose:	To set a new pathname for the document. Overide the parent class versions so we
				can do things like send a message to inform people that things have changed.
	SeeAlso:	GetPathName(); CDocument::SetPathName();

********************************************************************************************/

void CCamDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	// Make a note of the current document's pathname
	wxString EntryPathName = GetFilename();

	// Call the parent class form of the function to do the job
	wxDocument::SetFilename( lpszPathName, bAddToMRU );

	// If the document's pathname has been changed then broadcast to the kernel to
	// let everyone know about the change as this will affect the title of the document.
	if (EntryPathName != *lpszPathName)
	{
		BROADCAST_TO_ALL( DocChangingMsg( m_pKernelDoc, DocChangingMsg::TITLECHANGED ) );
	}
}

/********************************************************************************************

>	virtual void CCamDoc::SetPathNameEmpty()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Returns:	-
	Purpose:	To set a new pathname for the document. Overide the parent class versions so we
				can do things like send a message to inform people that things have changed.
	SeeAlso:	GetPathName(); CDocument::SetPathName();

********************************************************************************************/

void CCamDoc::SetPathNameEmpty()
{
	SetFilename(_T(""));
}

/********************************************************************************************
>	void CCamDoc::SetModified(BOOL fState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/7/93
	Inputs:		A boolean which if TRUE means "mark document as modified"
	Purpose:	To tell the document that it has been modified.  While MFC provides an
				implementation of this, it doesn't update the document's title, so
				call this function instead.
	SeeAlso:	CDocument::IsModified
********************************************************************************************/

void CCamDoc::SetModified( bool fState )
{
	// If the docuement cannot be modified (ie. it's internal implementation) then
	// ignore this call.
	if (!IsModifiable()) return;


/*	TRACEUSER( "JustinF", _T("In CCamDoc::SetModified(%s)\n"),
				(LPCTSTR) (fState ? TEXT("TRUE") : TEXT("FALSE")));
*/	
	// Do stuff only when the doc's state is changed.
	if (fState != IsModified())
	{
		// Set the flag & update the flags in the title
		SetModifiedFlag( fState );		
		UpdateTitle();
	}

	// It can't be "just created" any more
	if( fState )
		m_fIsUntouched = FALSE;

#if (_OLE_VER >= 0x200)
	// Count this modification (or unmodification) for the idle-time server update if
	// the doc is embedded.
	if (IsServer()) GetEmbeddedItem()->PostponeUpdate();

	// Don't do any updates for a while
	ResetContainerUpdateTimer();
#endif

	// tell the world we have changed something
	BROADCAST_TO_ALL(ScreenChangeMsg());
	//TRACEUSER( "Diccon", _T("CamDoc::SetModified\n"));
}



/********************************************************************************************
>	void CCamDoc::SetReadOnly(BOOL fState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/7/93
	Inputs:		A boolean which if TRUE means "mark document as read-only"
	Outputs:	-
	Returns:	-
	Purpose:	To tell the document whether it can be modified or not.
	Errors:		-
	SeeAlso:	CCamDoc::IsReadOnly
********************************************************************************************/

void CCamDoc::SetReadOnly( bool fState )
{
	if (fState != IsReadOnly())
	{
		m_fIsReadOnly = fState;
		UpdateTitle();
	}
}



/********************************************************************************************
>	void CCamDoc::SetCopy(BOOL fState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/7/93
	Inputs:		A boolean which if TRUE means "mark document as modified"
	Outputs:	-
	Returns:	-
	Purpose:	To tell the document whether it is a copy of an ineditable document.
	Errors:		-
	SeeAlso:	CCamDoc::IsACopy
********************************************************************************************/

void CCamDoc::SetCopy( bool fState )
{
	if (fState != IsACopy())
	{
		m_fIsACopy = fState;
		UpdateTitle();
	}
}

/********************************************************************************************
>	BOOL CCamDoc::IsUntouched() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this document is untouched by the user. That means the program has
				created the document but the user hasn't modified, edited, operated on it
				or saved it.
				FALSE otherwise.
	Purpose:	Provides a safe way to test the document's flags.
				Note there's no equivalent Set function for this flag because only this
				class should ever set its state.
	Errors:		-
	SeeAlso:	CCamDoc::SetJustCreated
********************************************************************************************/

bool CCamDoc::IsUntouched() const
{
	return m_fIsUntouched;
}

/********************************************************************************************
>	BOOL CCamDoc::IsReadOnly() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this document cannot be edited, only browsed.
	Purpose:	Provides a safe way to test the document's flags.
	Errors:		-
	SeeAlso:	CCamDoc::SetReadOnly
********************************************************************************************/

bool CCamDoc::IsReadOnly() const
{
	return m_fIsReadOnly;
}

/********************************************************************************************
>	BOOL CCamDoc::IsACopy() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this document is a copy of another (ineditable) document.
	Purpose:	Provides a safe way to test the document's flags.
	Errors:		-
	SeeAlso:	CCamDoc::SetCopy
********************************************************************************************/

bool CCamDoc::IsACopy() const
{
	return m_fIsACopy;
}

/********************************************************************************************
>	BOOL CCamDoc::IsModifiable() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/09/96
	Returns:	TRUE if this document modifiable via SetModified
				FALSE otherwise.
	Purpose:	Because of a bodge in Operation::EndOp(), SetModified() is called needlessly.
				So we know whether to pay any attention to IsModified() we can use this

********************************************************************************************/
bool CCamDoc::IsModifiable() const
{
	return m_bIsModifiable;
}

/***********************************************************************************************
> 	Document* CCamDoc::GetKernelDoc() const

    Author: 	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:	19/5/93
	Inputs:		- 
    Outputs:    -
    Returns:   	Pointer to the Document object associated with this object.
    Purpose:    Find the Kernel document associated with this Oil document.
	Errors:		-
	SeeAlso:	-
***********************************************************************************************/

Document* CCamDoc::GetKernelDoc() const
{
	return m_pKernelDoc;
}



/********************************************************************************************

>	BOOL CCamDoc::SaveModified()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if safe to continue & close document, FALSE if cannot
	Purpose:	Prompts user to save document if required. MFCCOPY. Normal MFC uses non-
				UI compliant Yes/No box.
	Errors:		-

********************************************************************************************/

bool CCamDoc::OnSaveModified()
{
#ifdef STANDALONE
	// Don't prompt in standalone version - we can't save anyway
	return TRUE;
#else
	// Check if we can dodge this job.  Don't save "hidden" kernel documents, or docs
	// that cannot be modified.
	if (!IsModifiable() || (GetKernelDoc() && GetKernelDoc()->IsAHiddenDoc()))
	{
		return true;        // ok to continue
	}

#if (_OLE_VER >= 0x200)
	// Don't save (to file) embedded documents, they always save into a storage.
	if (IsEmbedded()) return CCamDocBase::SaveModified();
#endif
	
	// Don't save docs that haven't been changed since last being saved.
	if (!IsModified()) return TRUE;

	// Get filename to display in msg.
	wxString name = m_documentFile; 	// Was: m_strPathName;
	if (name.IsEmpty())
	{
		// No path means untitled, so use the made-up name instead.
		name = (const TCHAR*)m_TitlePrefix;	
	}
	else
	{
		// Extract the abbreviated path from the full path as held by MFC.
		// This involves a bit of string juggling...
		String_256 Temp(name);
		PathName Path(Temp);
		Temp = Path.GetTruncatedPath(20);
		name = (TCHAR*)Temp;
	}

	// use MakeMsg, so the context-sensitive help will work.
	StringBase buf;
	if (!buf.Alloc(512))
	{
		// Out of memory!
		ERROR( _R(IDE_NOMORE_MEMORY), false );
	}

	// Make the "save changes to <file>" message.
	buf.MakeMsg(_R(IDM_SAVEPROMPT), (LPCTSTR) name);

	// Set up the question we want to ask.
	ErrorInfo Question;
	Question.ErrorMsg = 0;
	Question.Button[0] = _R(IDB_SAVE);
	Question.Button[1] = _R(IDB_DONTSAVE);
	Question.Button[2] = _R(IDB_CANCEL);
//	Question.Button[3] = _R(IDB_HELP);
	Question.OK = 1;
	Question.Cancel = 3;

	// We use a special title for the dialogue, usually "Closing Document" but if
	// Camelot is shutting down then "Exiting Xara Studio".
	Question.Title = (Camelot.CamelotIsDying()) ? _R(IDS_EXITING_XSTUDIO_TITLE)
												: _R(IDS_CLOSING_DOC_TITLE);

	BOOL Again;
	do
	{
		// Set the error message
		Error::SetError( 0, buf, 0 );

		// And ask the question
		Again = FALSE;
		TRACEUSER( "Andy", _T("Creating SAVE? dialog box\n"));
/*		switch ( AskQuestion( &Question ) )
		{
			case _R(IDB_CANCEL):
				return FALSE;       // don't continue
				break;

			case _R(IDB_SAVE):
				// If so, either Save or Update, as appropriate
				if (!DoSave(m_strPathName))
					return FALSE;       // don't continue
				
				// fall through . . .

			case _R(IDB_DONTSAVE):
				// If not saving changes, revert the document.

				// OK, we must clear the modified flag here as the doc is no longer "dirty".
				// If we don't then we will be prompted twice about saving open docs when
				// we process a system shutdown message.
				SetModified(FALSE);
				break;

			default:
				ASSERT(FALSE);
				break;
		}
*/
		UINT32 answer = AskQuestion( &Question );
		if (answer==_R(IDB_CANCEL))
			return FALSE;       // don't continue

		else if (answer==_R(IDB_SAVE))
		{
			// If so, either Save or Update, as appropriate
			if (!DoSave(GetOriginalPath()))
				return FALSE;       // don't continue
			// else doc has been saved so it's no longer "modified"
			SetModified(FALSE);
		}

		else if (answer==_R(IDB_DONTSAVE))
			// If not saving changes, revert the document.

			// OK, we must clear the modified flag here as the doc is no longer "dirty".
			// If we don't then we will be prompted twice about saving open docs when
			// we process a system shutdown message.
			SetModified(FALSE);

		else
			ERROR3("Unexpected return from AskQuestion\n");

	}
	while (Again);

	return TRUE;    // keep going
#endif
	return true;
}


/********************************************************************************************

This overrides the default wxDocument SaveAs behaviour, initially to tell user that save 
doesn't work, but in the longer term it will play host to the real SaveAs code

********************************************************************************************/

bool CCamDoc::SaveAs()
{
//	return wxDocument::SaveAs();
    wxDocTemplate *docTemplate = GetDocumentTemplate();
    if (!docTemplate)
        return false;

#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
    wxString filter = docTemplate->GetDescription() + wxT(" (") + docTemplate->GetFileFilter() + wxT(")|") + docTemplate->GetFileFilter();

    // Now see if there are some other template with identical view and document
    // classes, whose filters may also be used.

    if (docTemplate->GetViewClassInfo() && docTemplate->GetDocClassInfo())
    {
        wxList::compatibility_iterator node = wxDocManager::GetDocumentManager()->GetTemplates().GetFirst();
        while (node)
        {
            wxDocTemplate *t = (wxDocTemplate*) node->GetData();

            if (t->IsVisible() && t != docTemplate &&
                t->GetViewClassInfo() == docTemplate->GetViewClassInfo() &&
                t->GetDocClassInfo() == docTemplate->GetDocClassInfo())
            {
                // add a '|' to separate this filter from the previous one
                if ( !filter.empty() )
                    filter << wxT('|');

                filter << t->GetDescription() << wxT(" (") << t->GetFileFilter() << wxT(") |")
                       << t->GetFileFilter();
            }

            node = node->GetNext();
        }
    }
#else
    wxString filter = docTemplate->GetFileFilter() ;
#endif

    wxString strPath;
	wxString strName;
	wxString strExt;

	// If this document already has a path, make Save As default to that path
	wxString tmp = GetFilename();
	if (!tmp.IsEmpty())
	{
	    wxSplitPath(tmp, &strPath, &strName, &strExt);
	}

	if (strPath.IsEmpty())
//		strPath = docTemplate->GetDirectory();
		strPath = (LPCTSTR) BaseFileDialog::DefaultSaveFilePath;

	// We want to force the use of the .xar extension...
	strExt = docTemplate->GetDefaultExtension();

	// We can't use wxOVERWRITE_PROMPT because is doesn't conform to our UI guidelines
	// So we must run that logic ourselves...
	BOOL bShowFileSelectorAgain = FALSE;
	do
	{
		wxString title(CamResource::GetText(_R(IDS_SAVEAS)));
		tmp = wxFileSelector(title,
							strPath,
							strName,
							strExt,
							filter,
							wxSAVE,
							GetDocumentWindow());

		if (tmp.IsEmpty())
			return false;

		if (wxFileExists(tmp))
		{
			ErrorInfo Info;
			Info.ErrorMsg = _R(IDS_SAVEAS_OVERWRITE);
			Info.Button[0] = _R(IDS_OVERWRITE);
			Info.Button[1] = _R(IDB_SAVEAS);
			Info.Button[2] = _R(IDS_CANCEL);

			UINT32 Answer = AskQuestion(&Info);

			if (Answer==_R(IDS_OVERWRITE))
			{
				// Just use the name the user selected to overwite the existing file
				bShowFileSelectorAgain = FALSE;
			}
			else if (Answer== _R(IDB_SAVEAS))
			{
				// User wants to save as some other name
				wxSplitPath(tmp, &strPath, &strName, &strExt);
				strName = wxFileNameFromPath(tmp);
				tmp = _T("");
				bShowFileSelectorAgain = TRUE;
			}
			else if (Answer==_R(IDS_CANCEL))
			{
				// User has chosen to abort the operation
				return FALSE;
			}
			else
				ERROR3("Unknown Answer from AskQuestion");
		}
	}
	while (bShowFileSelectorAgain);

    wxString fileName(tmp);
    wxSplitPath(fileName, &strPath, &strName, &strExt);

    if (strExt.IsEmpty())
    {
        fileName += wxT(".");
        fileName += docTemplate->GetDefaultExtension();
    }

    SetFilename(fileName);
    SetTitle(wxFileNameFromPath(fileName));

    // Notify the views that the filename has changed
    wxList::compatibility_iterator node = m_documentViews.GetFirst();
    while (node)
    {
        wxView *view = (wxView *)node->GetData();
        view->OnChangeFilename();
        node = node->GetNext();
    }

    // Files that were not saved correctly are not added to the FileHistory.
    if (!OnSaveDocument(m_documentFile))
        return false;

	// Record the path we saved in a preference so it survives sessions
	BaseFileDialog::DefaultSaveFilePath = strPath;

	// A file that doesn't use the default extension of its document template cannot be opened
	// via the FileHistory, so we do not add it.
	if (docTemplate->FileMatchesTemplate(fileName))
	{
		GetDocumentManager()->AddFileToHistory(fileName);
	}
	else
	{
		// The user will probably not be able to open the file again, so
		// we could warn about the wrong file-extension here.
	}

	return true;
}


/********************************************************************************************

>	BOOL CCamDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Inputs:		lpszPathName - path name where to save document file
				if 'bReplace' is TRUE will change file name if successful (SaveAs)
				if 'bReplace' is FALSE will not change path name (SaveCopyAs) or clear
				the "dirty" flag.
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	This is an exact copy of the CDocument version of this function. I had to
				copy it out as I need to change the bit that displays the SaveAs dialog so
				that I could set it to the correct directory and generally change it about
				to meet our requirements. This function ends up calling OnSaveDocument above
				to do the actual saving.

********************************************************************************************/

PORTNOTE("other","CCamDoc::DoSave - Replaced by simpler version")
#ifndef EXCLUDE_FROM_XARALX
bool CCamDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
#ifdef STANDALONE
	// Do nothing on a standalone version
	return true;
#else

	// Get the name to save it as and see if it is empty
	bool				SaveAsNewName = false;
	wxString			newName = lpszPathName;

	// See if we have loaded this document as an old EPS native format
	// If so, then give the user the oppertunity to rename the file so that we don't
	// overwrite the original old format document.
	// Don't though, if the document is unnamed, so we are going to do a 'save as ...'
	// operation anyway.
	if( m_pKernelDoc && m_pKernelDoc->IsLoadedAsVersion1File() && !newName.IsEmpty() )
	{
#ifndef _DEBUG
		// Old EPS format document so ask the user which format to save in
		ErrorInfo Info;
		Info.ErrorMsg = _R(IDS_OVERWRITEOLDFILE);
		Info.Button[0] = _R(IDS_OVERWRITE);
		Info.Button[1] = _R(IDB_SAVEAS);
		Info.Button[2] = _R(IDS_CANCEL);
		UINT32 Answer = AskQuestion(&Info);
#else
		UINT32 Answer;
		if (!OpConvertFileFormats::IsActive ())
		{
			// Old EPS format document so ask the user which format to save in
			ErrorInfo Info;
			Info.ErrorMsg = _R(IDS_OVERWRITEOLDFILE);
			Info.Button[0] = _R(IDS_OVERWRITE);
			Info.Button[1] = _R(IDB_SAVEAS);
			Info.Button[2] = _R(IDS_CANCEL);
			Answer = AskQuestion(&Info);
		}
		else
		{
			Answer = SaveAsNewName;
		}
#endif
		if (Answer==_R(IDS_OVERWRITE))
			// Rename the new file to make sure we don't overwrite old
			SaveAsNewName = FALSE;
		else if (Answer== _R(IDB_SAVEAS))
			// Rename the new file to make sure we don't overwrite old
			SaveAsNewName = TRUE;
		else if (Answer==_R(IDS_CANCEL))
			// User has chosen to abort the operation
			return FALSE;
		else
			ERROR3("Unknown Answer from AskQuestion");

		// Once we have asked the question ensure that the user is not asked again
		// We need to set the loaded flag as FALSE to effectively say that the document
		// is now considered version 2.
		// In a save as operation, the document will switch to the new name.
		// Now Disabled, as cannot set it until actually saved the document as the user may choose
		// something like save as and then cancel the file dialog box
		//KernelDoc->SetLoadedAsVersion1File(FALSE);
	}

	// If the name is empty, i.e. wasn't loaded as a .xar (e.g. open a tiff file) or newly created
	// then ask the user for a name i.e. do a 'save as....' operation.
	// Also, if we loaded an old format eps xar file and the user has chosen to not overwrite the
	// file then we must give them a 'save as....' option.
	if (newName.IsEmpty() || SaveAsNewName)
	{
		// Its empty, so create a default name
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);

		newName = m_strPathName;
		if (bReplace && newName.IsEmpty())
		{
			// No path means untitled, so use the made-up name instead.
			// This is our stored form of the title before having any items such as ' * '
			// or '(Copy)' appended. It is used by the close unsaved document, so warn user code
			// so should be reasonably safe.
			newName = (const TCHAR*) TitlePrefix;
		}

		// we will do it like this instead...
		// Set up the dialog
		String_256 FilterString(_R(IDS_SAVEFILTERS));

		// Get the filename.
		String_256 Str((const char*)newName);

		// If the string is still empty, get a default out
		if (Str.IsEmpty()) Str.Load(_R(IDS_DEFAULTFILENAME));

		// Build a pathname out of it
		PathName Path(Str);

		// Get some details, ready for the dialog
		String_256 LowxString = Path.GetLocation(FALSE);
		String_256 NameString = Path.GetFileName();

		// create a dialog and use the path and name of the file if available, or
		// use the default path if not.
		String_256 DocName = GetKernelDocName();

		// Sorry about the long names, but we are into OS Specific scary stuff here
		// Under Windows 95 the OnFileNameOK() function of CFileDialog does not work
		// As a Result we have to do the tests ourselves afterwards under win95
		// by default we do not need to put the dialog up
		BOOL WeNeedToPutSaveAsDialogUpAgain = FALSE;
		
		// try the dialog
		do
		{
			// Default to only putting the dialog up the once.
			WeNeedToPutSaveAsDialogUpAgain = FALSE;

			// Create and display prepare the dialog
			SaveFileDialog SaveDialog(FilterString, LowxString, NameString, DocName);
			SaveDialog.PrepareDialog();		
	
			// Display the dialog
			TRACE( _T("Display the dialog\n"));
			BOOL Result = SaveDialog.OpenAndGetFileName();

			// If they did not click on OK then stop right now
			if (!Result)
				return FALSE;

			// Make sure that the file name is OK
			// we only need to do this under windows 95, as other OS's will have already done it
			// Disable for VC4.0/MFC 4.0 as they have fixed it.
		#if _MFC_VER < 0x400
			if (IsWin32c())
			{
				TRACE( _T("Windows 95, testing for same file name after the event\n"));
				if (!SaveDialog.IsValidFilename()) WeNeedToPutSaveAsDialogUpAgain = TRUE;
			}
		#endif

			// Get the filename.
			wxString TempName(SaveDialog.m_ofn.lpstrFile);
			SaveDialog.AppendExtension(&TempName);
			Str = String_256(TempName);
			Path = PathName(Str);

			// Extract directory name (minus terminating backslash) and remember for next time.
			SaveDialog.SetDefaultPath(Path.GetLocation(FALSE));

		} while(WeNeedToPutSaveAsDialogUpAgain);

		// Put the file name into the var set aside for it
		newName = (const char*) Str;
	
	}

		
	// Check that the document isn't already open. NOTE: this is only relevant
	//	to OLE documents, as attempting to save a document with the same name as
	//	an existing, open doc will stuff up the OLE registration code in the 
	//	CCamDoc::OnSaveDocument function, after overwriting the file itself...
	
	BOOL isDocumentOpen = FALSE;

PORTNOTETRACE("other","CCamDoc::DoSave - remove OLE usage");
#ifndef EXCLUDE_FROM_XARALX
#if (_OLE_VER >= 0x200)

	POSITION pos = AfxGetApp()->GetFirstDocTemplatePosition();

	CDocTemplate::Confidence match = CDocTemplate::noAttempt;
	CDocTemplate* pTemplate = NULL;
	CDocument* pOpenDocument = NULL;

	// cycle through the document types until we've done them all, or we find that
	//	the document is already open.
	while ((pos != NULL) && (match != CDocTemplate::yesAlreadyOpen))
	{
		pTemplate = AfxGetApp()->GetNextDocTemplate(pos);
		ASSERT (pTemplate->IsKindOf(RUNTIME_CLASS(CDocTemplate)));

		ASSERT (pOpenDocument == NULL);
		match = pTemplate->MatchDocType(newName, pOpenDocument);

			// NOTE: make sure that the match isn't the document that we're editing,
			//	otherwise we wouldn't be able to save documents at all...
		if (this == pOpenDocument)
		{
			match =  CDocTemplate::noAttempt;
			pOpenDocument = NULL;
		}
	}

	// Determine whether document is already open or not
	if (match == CDocTemplate::yesAlreadyOpen)
		isDocumentOpen = TRUE;
	else
		isDocumentOpen = FALSE;

#endif		// (OLE_VER >= 0x200)
#endif

	// End 'Already Open' check.

	if (isDocumentOpen == FALSE)
	{

		// Make a .bak version of the file here in case the save screws up
		// This will be removed after the save, unless they have the keep bak files option in
		// Rename the current version of the file to the same with a .bak extension
		// get the old filename
		String_256 BakFileName(newName);
		String_256 OldFileName = BakFileName;
		
		// Change the extension on NewFileName...
		PathName NewPathName = BakFileName;

		// and change its extension
		String_256 Extension(_R(IDS_BAKEXTENSION));
		NewPathName.SetType(Extension);

		// Go find out where the temp directory is so that we can put the .bak file in there
		String_256 TempFileName;
		char* pTempName = _tgetenv("TEMP");
		if (pTempName!=NULL)
		{
			TempFileName = String_256(pTempName);
			
			// See if we have a backslash
			String_8 LastChar;
			TempFileName.Right(&LastChar, 1);

			// if it isn't a \ then add one
			if (LastChar!=String_8("\\"))
				TempFileName += String_8("\\");

			// Add the leaf name to it
			TempFileName += NewPathName.GetFileName();
			TRACEUSER( "Neville", _T("TEMP file is %s\n"), (const char*)TempFileName);
		}

		// Copy the version with the new extension back over the original
		BakFileName = NewPathName.GetPath();

		// Get these as strings we can use
		TCHAR* pBakName = NULL;
		TCHAR* pOldName = (TCHAR*) OldFileName;
		
		// If the user want .bak files then use a local name,
		// otherwise put the bak file in the temp directory
		if ((BaseFileDialog::MakeBakFiles) || (pTempName==NULL))
			pBakName = (TCHAR*) BakFileName;
		else
			pBakName = (TCHAR*) TempFileName;

		// make sure they are not the same (ie, let them save files with a .bak extension)
		BOOL FileNamesSame = (BakFileName == OldFileName);
		if (!FileNamesSame)
		{
			// Make sure that the .bak version of the file does not exist
			// If these do not work (eg the .bak is read-only) then you
			// simply will not get your .bak file updated and run the risk
			// of loosing your current changes if the save fails.
			TRACEUSER( "Neville", _T("Check to see if %s exists\n"), pBakName);
			if (_taccess(pBakName, 0)==0)
			{
				TRACEUSER( "Neville", _T("deleteing %s\n"), pBakName);
				unlink(pBakName);
			}
		
			// Now rename the original file (but only if the old filename exists)
			TRACEUSER( "Neville", _T("Check to see if %s exists\n"), pOldName);
			if (_taccess(pOldName, 0)==0)
			{
				// Rename the original artwork to a .bak version of the file
				TRACEUSER( "Neville", _T("Renaming %s to %s\n"), pOldName, pBakName);
				INT32 RenameResult = _trename(pOldName, pBakName);

				// if we fail to rename the file, but the user wants renamed files, complain
				if ((RenameResult!=0) && (BaseFileDialog::MakeBakFiles))
				{
					// There was a problem renaming the file, ask if we want to go on
					ErrorInfo Info;
					Info.ErrorMsg = _R(IDS_RENAMEFAILED);
					Info.Button[0] = _R(IDS_SAVEANYWAY);
					Info.Button[1] = _R(IDS_ABORTSAVE);

					// If not then stop right away.
					if (AskQuestion(&Info)==_R(IDS_ABORTSAVE))
						return FALSE;
				}
			}
		}

		// This may take a while ...
		String_64 strProgress;
		strProgress.MakeMsg(_R(IDS_WAIT_UPDATING_DOC_PFX), (LPCTSTR) GetTitle());
		BeginSlowJob( -1, FALSE, NULL );

	#if (_OLE_VER >= 0x200)
		// If the document is embedded and this isn't SaveCopyAs then don't export a preview
		// bitmap.
		BOOL fOldPrev;
		if (IsEmbedded() && !bReplace)
		{
			TRACEUSER( "JustinF", _T("Exporting preview for embedded item in CCamDoc::DoSave\n"));
			fOldPrev = SetExportPreview(TRUE);
		}
	#endif

		// Do the actual saving
		BOOL fOK = OnSaveDocument(newName);

	#if (_OLE_VER >= 0x200)
		// Restore the original setting if we changed it.
		if (IsEmbedded() && !bReplace) SetExportPreview(fOldPrev);
	#endif

		// Was the save successful?
		if (!fOK)
		{
			// The save failed here, so tidy up
			// be sure to delete the corrupt file
			TRACEUSER( "Neville", _T("Save failed, removing %s\n"), newName);
			remove(newName);

			// Well, something went a bit wrong there, so copy the old version of the file
			// over the top of the rubbish one if we have the .bak version that is
			if (!FileNamesSame)
			{
				// If the bak file exists, rename it over the original
				TRACEUSER( "Neville", _T("Check to see if %s exists\n"), pBakName);
				if (_taccess(pBakName, 0) == 0)
				{
					TRACEUSER( "Neville", _T("renaming %s to %s\n"), pBakName, pOldName);
					_trename(pBakName, pOldName);
				}
			}

			// return that we failed
			EndSlowJob();
			return FALSE;
		}

		// The save worked, so get rid of the bak file if they do not want it
		// if we do not want bak file around the place, then get rid of them
		if (!BaseFileDialog::MakeBakFiles)
		{
			TRACEUSER( "Neville", _T("bak files not wanted, getting rid of it\n"));
			// Make sure the .bak file was not the real file as well!
			if (!FileNamesSame)
			{
				// If the bak file exists, delete it
				TRACEUSER( "Neville", _T("Check to see if %s exists\n"), pBakName);
				if (_taccess(pBakName, 0)==0)
				{
					TRACEUSER( "Neville", _T("Deleting %s\n"), pBakName);
					unlink(pBakName);
				}
			}		
		}

		// Once we have saved the document ok, then ensure that the user is not asked again
		if (KernelDoc) KernelDoc->SetLoadedAsVersion1File(FALSE);

		// reset the title and change the document name
		if (bReplace)
		{
			// If we have just changed the name of the document, then get rid of the copy flag
			SetCopy(FALSE);
		
			// We saved the file under a new name, as opposed to a copy of the file, so it is
			// no longer "dirty".
			SetModified(FALSE);

			// Set the name of the document
			SetPathName(newName);
		}

		EndSlowJob();
		return TRUE;        // success
	}		// Document is already open and registered
	else
	{
//		MessageBox (NULL, _R(IDS_OVERWRITEOPENFILE), NULL, MB_ICONERROR);
		UINT32 Answer;
		ErrorInfo Info;

		Info.ErrorMsg = _R(IDS_OVERWRITEOPENFILE);
		Info.Button[0] = _R(IDS_CANCEL);
		Answer = AskQuestion(&Info);

		return FALSE;		// could not open document

	}		// End of isDocumentOpen (OLE registration check)
#endif	// Not standalone (see first few lines)
}
#else


bool CCamDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
{
#ifdef STANDALONE
	// Do nothing on a standalone version
	return true;
#else
	PORTNOTETRACE("save", "CCamDoc::DoSave grossly simplified from Xtreme version!");
	BOOL bOK = TRUE;

	// Get the name to save it as and see if it is empty
	BOOL				SaveAsNewName = false;
	wxString			newName = lpszPathName;

	// If the name is empty, i.e. wasn't loaded as a .xar (e.g. open a tiff file) or newly created
	// then ask the user for a name i.e. do a 'save as....' operation.
	// Also, if we loaded an old format eps xar file and the user has chosen to not overwrite the
	// file then we must give them a 'save as....' option.
	if (newName.IsEmpty() || SaveAsNewName)
	{
		SaveAs();
	}
	else
	{
		bOK = OnSaveDocument(newName);
	}

	return bOK;
#endif
}
#endif


/********************************************************************************************
>	virtual void CCamDoc::DeleteContents()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes this document's data.  Called by MFC in various situations.
	Errors:		-
	SeeAlso:	Document::DeleteContents
********************************************************************************************/

bool CCamDoc::DeleteContents()
{
	m_pKernelDoc->SetCurrent();
	m_pKernelDoc->DeleteContents();

	return true;
}

/********************************************************************************************
>	void CCamDoc::UpdateTitle()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets the (window) titles of all views onto this document to the
				contained title of the document, plus a suffix if the document has
				been modified (cf. Visual C++ & Word), is read-only, or a copy.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void CCamDoc::UpdateTitle()
{
	m_pKernelDoc->SetCurrent();

	// Update the title of all views onto this document, and broadcast to the kernel to
	// let everyone know about the change, but only if it has actually changed
	// This actually sets the title to the same as it is now, but makes it recaluculate all
	// 'Modifed' flags on the end
	SetTitle( m_TitlePrefix );

    // Notify the views that the filename has changed
    wxList::compatibility_iterator node = m_documentViews.GetFirst();
    while (node)
    {
        wxView *view = (wxView *)node->GetData();
        view->OnChangeFilename();
        node = node->GetNext();
    }
}

/********************************************************************************************

>	String_256 CCamDoc::GetKernelTitle()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/95
	Returns:	String_256 containing the current document title
	Purpose:	Good for finding out the title of the document. If the document has been
				saved as 'Blobby.art' then the title returned will be of the form
				'Blobby.art (Modified)' etc. ie What you would see in the title of a
				non maximised document window.
	SeeAlso:	Document::GetTitle()

********************************************************************************************/

String_256 CCamDoc::GetKernelTitle()
{
	// First off, get a copy of the string we are after
	wxString			CurrentTitle = GetTitle();

	// and put it into a Kernel String_256 and return it
	return String_256( CurrentTitle );
}



/********************************************************************************************

>	String_256 CCamDoc::GetKernelPathName(UINT32 MaxSize = 0)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		MaxSize	maximum size in characters of the requried pathname.
				Zero indicates return the full pathname, which it defaults to. 
	Returns:	String_256 containing the current document pathname
	Purpose:	Good for finding out the pathname of the document.
	SeeAlso:	Document::GetPathName;
	
********************************************************************************************/

String_256 CCamDoc::GetKernelPathName(UINT32 MaxSize)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// First off, get a copy of the string we are after
	wxString			name = GetFilename();

	if( !name.IsEmpty() )
	{
		// Extract the abbreviated path from the full path as held by MFC.
		TCHAR			buf[512];
		camStrcpy( buf, name );
PORTNOTE("other","Removed AbbreviateName usage")
#ifndef EXCLUDE_FROM_XARALX
		AbbreviateName( buf, (INT32)MaxSize, TRUE );
#endif
		name = buf;
	}

	// and put it into a Kernel String_256 and return it
	String_256 KernelString(name);
	return KernelString;
#else
	return String_256( GetFilename() );
#endif
}



/********************************************************************************************

>	String_256 CCamDoc::GetKernelLocation(UINT32 MaxSize = 0)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		MaxSize	maximum size in characters of the requried location.
				Zero indicates return the full location, which it defaults to. 
	Returns:	String_256 containing the current document location
	Purpose:	Good for finding out the location of the document. This is the pathname minus
				the filename.
	SeeAlso:	Document::GetLocation
	
********************************************************************************************/

String_256 CCamDoc::GetKernelLocation(UINT32 MaxSize)
{
	// First off, get a copy of the string we are after
	wxString			name = GetFilename();  //GetPathName();

	// and put it into a Kernel String_256 ready just in case we have to return it
	String_256 KernelString(name);

	// if the pathname is blank then return it directly to the caller.
	if ( KernelString.IsEmpty() )
		return KernelString;
		
	// We will use the pathname class to try and truncate this to a more reasonable
	// pathname if it will not fit into the required space as this knows all about
	// what makes up paths etc.
	PathName   KernelPathName(KernelString);	// construct the pathname using the string_256

	if (!KernelPathName.IsValid())
	{
		// There was a problem parsing that pathname, so just return the string
		// An error will have been set.
		Error::ClearError();
		return KernelString;
	}
	
	return KernelPathName.GetTruncatedLocation(MaxSize);
}



/********************************************************************************************

>	String_256 CCamDoc::GetKernelDocName(BOOL IncludeFileType = TRUE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Returns:	String_256 containing the current document name.
	Purpose:	Good for finding out the name of the document. If the document has been
				saved as 'Blobby.art' then the title returned will be of the form
				'Blobby.art etc. otherwise will be of the form Untitled#1. Similar to what
				the functions GetTitle() and GetKernelTitle() return but is just the document
				name with none of the modified etc.
	SeeAlso:	GetKernelTitle(); Document::GetDocName();

********************************************************************************************/

String_256 CCamDoc::GetKernelDocName(BOOL IncludeFileType)
{
	// The Title Prefix is just the name of the document without all the 'Modified' bits
	// on the end of it. If it isn't, the bug is not here, but elsewhere.
	return m_TitlePrefix;
}

/********************************************************************************************
>	String_256 CCamDoc::GetOriginalPath() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/95
	Returns:	Returns the path to the original file that was used to load this doc if it
				wasn't a .ART native format file, eg. if you "open" a bitmap then Camelot
				makes a default document and imports the bitmap onto it, in this case this
				function will return the path to the bitmap.  This path is set to empty if
				the doc is modified or was created by "new" document.
	SeeAlso:	CCamDoc::SetOriginalPath; SaveAppWindowState
********************************************************************************************/

String_256 CCamDoc::GetOriginalPath() const
{
	// We *don't* set the original path to empty in the IsModified function because
	// the user might undo back to the unmodified version!
	return (((CCamDoc*) this)->IsModified()) ? String_256(TEXT("")) : m_strOriginalPath;
}



/********************************************************************************************
>	void CCamDoc::SetOriginalPath(const String_256& strPath)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/95
	Inputs:		strPath			the path to the original non-native document, eg. a bitmap
								that was "opened" to make this doc.
	Purpose:	Sets the path to the non-native document that this Camelot doc is based on.
				This path should be set to empty if the Camelot doc is no longer equivalent
				to the original non-native file, eg. if the Camelot doc becomes modified,
				or was loaded from a .ART native file, or is new (untitled).
	Errors:		-
	SeeAlso:	CCamDoc::GetOriginalPath
********************************************************************************************/

void CCamDoc::SetOriginalPath(const String_256& strPath)
{
//	TRACEUSER( "JustinF", _T("\tSetting original path to %s\n"), (LPCTSTR) strPath);
	m_strOriginalPath = strPath;
}

/********************************************************************************************
>	virtual CCLexFile* CCamDoc::CreateCCFile(LPCTSTR lpcszPath, INT32 nOpenMode)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		lpcszPath	-	path to the disk file to open, or if NULL then open the
								"CONTENTS" stream of the CCamDoc's root-storage member.
				nOpenMode	-	opening mode, eg. ios::in.  ios::binary is assumed.
	Returns:	A pointer to a CCLexFile derivative on the heap that represents the given
				file object, either on disk or in structured storage.  The caller should
				deallocate this file when finished with it.  Returns NULL if there was an
				error (eg. file not found) - if so then the error has already been reported.
	Purpose:	CCamDoc::OnOpenDocument and CCamDoc::OnSaveDocument call this to allocate
				a CCLexFile object to load ans save from/to.
	SeeAlso:	CCamDoc::OnOpenDocument; CCamDoc::OnSaveDocument
********************************************************************************************/

CCLexFile *CCamDoc::CreateCCFile( LPCTSTR lpcszPath, INT32 nOpenMode )
{
	// Fix-up the flags.
	nOpenMode |= ios::binary;
	if (nOpenMode & ios::out) nOpenMode |= ios::trunc;

	// Allocate the appropriate kind of file.
	CCLexFile* pFile = 0;
	if (lpcszPath)
	{
		// A path was passed, so open a disk-file on it.
		pFile = new CCDiskFile(PathName(lpcszPath), nOpenMode);
//		TRACEUSER( "JustinF", _T("\t- a new CCDiskFile at 0x%p, no sweat\n"), (LPVOID) pFile);
	}

#if (_OLE_VER >= 0x200)

	else
	{
		// No path was passed, so open the 'native XaraDrawing' stream of CCamDoc's
		// m_lpRootStg storage, inherited from CCamDocBase (COleServerDoc).
		ERROR2IF(!m_lpRootStg, 0, "No IStorage interface in CCamDoc::CreateCCFile");

		// We have to create or open the stream, depending on whether we are opening for read,
		// write, or read/write.
		//
		// JCF: version 2.0 change - embedded native XaraDrawings no longer have
		// version numbers as the native filter handles all versioning itself.
		// So we will try to transparently rename on opening any old style streams
		// we come across.
		LPCWSTR pszStreamName = L"Xara Drawing (native)";
		LPSTREAM pIStream;
		HRESULT hr;
		if (nOpenMode & ios::in)
		{
			// Loading, so open an existing stream.
			ERROR2IF(nOpenMode & ios::out, FALSE,
							"CCamDoc::CreateCCFile: OLE read AND write not implemented");
			hr = m_lpRootStg->OpenStream(pszStreamName, 0,
										 STGM_READ | STGM_SHARE_EXCLUSIVE,
										 0, &pIStream);

			// Try the old native format if opening the new stream didn't work.
			if (hr == STG_E_FILENOTFOUND)
			{
				LPCWSTR pszOldStreamName = L"XaraDrawing 1.5 (Native)";
				hr = m_lpRootStg->OpenStream(pszOldStreamName, 0,
											 STGM_READ | STGM_SHARE_EXCLUSIVE,
											 0, &pIStream);
				
				if (SUCCEEDED(hr))
				{
					// Try to modernise the old stream's name.
					HRESULT hr2 = m_lpRootStg->RenameElement(pszOldStreamName, pszStreamName);
					TRACEUSER( "JustinF", _T("\t- tried to rename v1.5 stream (HRESULT 0x%lX)\n"),
													(UINT32) hr2);
					
					// Pass any error along if there's not a good reason for it.
					if (FAILED(hr2) &&
						hr2 != STG_E_DISKISWRITEPROTECTED &&
						hr2 != STG_E_ACCESSDENIED)
							hr = hr2;
				}
			}
		}
		else
		{
			// If there's a Camelot class factory around then associate this storage with it.
			if (m_pFactory) WriteClassStg(m_lpRootStg, m_pFactory->GetClassID());

			// Saving, so create a new stream / truncate an existing stream.
			ERROR3IF(!(nOpenMode & ios::out), "No I/O mode in CCamDoc::CreateCCFile");
			hr = m_lpRootStg->CreateStream(pszStreamName,
										   STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
										   0, 0, &pIStream);
		}

		// Do we have a valid IStream?
		if (FAILED(hr))
		{
			// No, so report the error and fail.
			TRACE( _T("Couldn't open IStream in CCamDoc::CreateCCFile (HRESULT 0x%lX)\n"),
					(UINT32) hr);
			InformError(_R(IDE_OPEN_ERROR));
			return 0;
		}

#ifdef WEBSTER		
		// WEBSTER-Martin-29/12/96
		// no accusoft stuff so wrap a boring old CCOleStream around it
		pFile = new CCOleStream(pIStream, nOpenMode, filebuf::sh_none);
#else
		// Yes, so wrap a CCAccusoftOleStream around it.
		pFile = new CCOleAccusoftStream(pIStream, nOpenMode, filebuf::sh_none);
#endif
/*		TRACEUSER( "JustinF", _T("\t- a new CCAccusoftOleStream at 0x%p, take cover!\n"),
					(LPVOID) pFile);
*/	}

#endif

	// Check if we managed to allocate the file.
	if (!pFile)
	{
		// No, so report the error and fail.
		InformError( _R(IDE_NOMORE_MEMORY) );
		return 0;
	}

	// Was the file opened sucessfully?
	if (!pFile->isOpen())
	{
		// Couldn't open the file, so deallocate it.  Any error will have already been
		// reported.
		delete pFile;
		pFile = 0;
	}

	// Return the file (or null).
	return pFile;
}
