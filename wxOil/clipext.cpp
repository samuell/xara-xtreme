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
// External clipboard interface - classes for encapsulating clipboard data mappings
// and for managing data transfers between the internal and external clipboards

#include "camtypes.h"

#include "stdafx.h"
//#include "clipdlg.h"
#include "clipext.h"
#include "clipint.h"
#include "clipmap.h"
#include "natclipm.h"
//#include "jason.h"

// **** TEMPORARY BODGE includes for crappy bitmap export
//#include "ccobject.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "node.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grndbmp.h"
#include "layer.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dlgview.h"
#include "vstate.h"
#include "wbitmap.h"

#include "mainfrm.h"
#include "camvw.h"
// ****

#if (_OLE_VER >= 0x200)
#include "camdoc.h"
#include "camelot.h"
#include "srvritem.h"
//#include "filters.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#endif

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// --- Support for node types used in scanning the document tree for KeepClipboardOnExit
#include "nodeblnd.h"
#include "nodebmp.h"
#include "bitmpinf.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
// ---

// --- OLE clipboard
#if (_OLE_VER >= 0x200)
#include "dataobj.h"
#endif

//#include "richard2.h"	// For resource strings
//#include "justin3.h"
//#include "mario.h"

CC_IMPLEMENT_MEMDUMP(ExternalClipboard, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_DYNCREATE(OpClipboardExport, Operation)
CC_IMPLEMENT_DYNCREATE(OpClipboardImport, SelOperation)
CC_IMPLEMENT_DYNAMIC(ClipboardView, View)
CC_IMPLEMENT_DYNCREATE(PasteSpecialDlg, DialogOp)


#define new CAM_DEBUG_NEW


// define this symbol as 1 to enable paste-special behaviour (this makes a paste show
// the paste-special dialogue box) or 0 to get "normal" simple paste behaviour.
#if _DEBUG
#define DO_PASTE_SPECIAL 1
#else
#define DO_PASTE_SPECIAL 1
#endif


// Statics ----------------------------------------------------------------------------------
ExternalClipboard* ExternalClipboard::pInstance = NULL;

// This value is the (rough) number of nodes in the clipboard tree before we think it's worth
// asking the user about whether they want the clipboard contents retained on exit. This is
// a very rough guide, mainly to eliminate this question when there is only trivial data
// (a sentence or a couple of blobs) on the clipboard.
// Note that this is not a strict node count. For example, blends and grad fill attributes
// cause a large count to be added, as these nodes can produce much larger output in
// standard clipboard formats such as WMF, while text characters cause a lower value.
// Each other node generally adds about 10 to the value, so 2000 corresponds roughly
// to 2000 characters, 200 nodes in the tree, or 10 grad-filled objects.
const INT32 KeepClipboardThreshold = 4000;
// ------------------------------------------------------------------------------------------


BOOL ExternalClipboard::m_fUpdateSystemClipboard = TRUE;


/********************************************************************************************
	Preference:	DontExportCMXInIDataObject
	Section:	OLE Local Server
	Range:		0 .. 1 (FALSE .. TRUE)
	Purpose:	If TRUE then don't offer to render CorelXARA documents in Corel CMX format
				for transfer via the clipboard or drag'n'drop.  Setting this pref. to TRUE
				overcomes a bug when embedding CorelXARA documents into CorelDRAW.  By
				default this setting is TRUE.  Making it FALSE will allow you to transfer
				CorelXARA documents to CorelDRAW via the clipboard or drag'n'drop in the
				CMX format, but embedding CorelXARA documents in CorelDRAW will no longer#
				work properly.
********************************************************************************************/
static BOOL fNoCMXDataExport = TRUE;


/********************************************************************************************

>	ExternalClipboard::ExternalClipboard()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	constructor

********************************************************************************************/

ExternalClipboard::ExternalClipboard()
{
	if (pInstance == NULL)
		pInstance = this;
	else
	{
		ERROR3("Oy! You can't have more than one instance of the ExternalClipboard!");
	}

	KeepFormatsOnExit = TRUE;

#if (_OLE_VER >= 0x200)
	m_fIgnoreChange = FALSE;
#endif
}



/********************************************************************************************

>	virtual ExternalClipboard::~ExternalClipboard()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	Destructor. Cleans up our lists and wotnot.

********************************************************************************************/

ExternalClipboard::~ExternalClipboard()
{
	Mappings.DeleteAll();

	if (pInstance == this)
		pInstance = NULL;
	else
	{
		ERROR3("Oy! You can't have more than one instance of the ExternalClipboard!");
	}
}



/********************************************************************************************

>	static BOOL ExternalClipboard::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	Initialises the ExternalClipboard ready for operation

	SeeAlso:	ExternalClipboard::Deinit

********************************************************************************************/

BOOL ExternalClipboard::Init(void)
{
	if (!OpClipboardExport::Init())
		return(FALSE);

	if (!OpClipboardImport::Init())
		return(FALSE);

	if (!PasteSpecialDlg::Init())
		return(FALSE);

	ExternalClipboard *TheClipboard = new ExternalClipboard();
	if (TheClipboard == NULL)
		return(FALSE);


	// **** !!!! DEBUG BODGE - add export mapping for text characters - ideally this
	// clipboard mapping should be provided by a proper text import/export filter,
	// but one does not exist yet.
	// Add a UNICODE filter - this only works under NT, as nobody else supports unicode
	BodgeUnicodeClipMap::CreateAndRegister(CLIPMAP_IMPORTEXPORT, CF_UNICODETEXT);

	// 8-bit text (windows implicitly converts between these for us)
	// The first one is the filter, while the second one is an 'alias' for import
	// which will make windows convert the OEMTEXT into TEXT and then we put this through
	// our normal TEXT mapping...
	BodgeTextClipMap::CreateAndRegister(CLIPMAP_IMPORTEXPORT, CF_TEXT);
	BodgeTextClipMap::CreateAndRegister(CLIPMAP_IMPORTONLY, CF_OEMTEXT);

	// Add bitmap/DIB import/export mappings
	// Note that if we export WMF and DIB, there is no need to bother with Bitmap
	// (Corel don't, probably because there is no way of getting a quality image via Bitmaps,
	// and also because some programs are stupid and corrupt the DIB by using the palette
	// which we supply for the Bitmap! Arrgh!)
//	BitmapClipMap::CreateAndRegister(CLIPMAP_IMPORTEXPORT);
	BitmapClipMap::CreateAndRegister(CLIPMAP_IMPORTONLY);

	DIBClipMap::CreateAndRegister(CLIPMAP_IMPORTEXPORT);

	// CMX import and export stuff to make commicating with Corel happier
	CMX16ClipMap::CreateAndRegister(CLIPMAP_IMPORTEXPORT, ::RegisterClipboardFormat("Corel Presentation Exchange Data"));
	CMX32ClipMap::CreateAndRegister(CLIPMAP_IMPORTEXPORT, ::RegisterClipboardFormat("Corel 32-bit Presentation Exchange Data"));

	// native clipboard thing -- for OLE stuff
	//
	// this is export only (although functional code does exist for import)
	// as there are several technical problems with getting the native filter
	// to import into the clipboard document. See the comment near the
	// NativeClipMap::HandleImport() function for more details.
	NativeClipMap::CreateAndRegister(CLIPMAP_IMPORTEXPORT /*CLIPMAP_EXPORTONLY*/, ::RegisterClipboardFormat("CorelXARA Document"));
//	NativeClipMap::CreateAndRegister(CLIPMAP_IMPORTEXPORT /*CLIPMAP_EXPORTONLY*/, ::RegisterClipboardFormat("XaraX Document"));

	// Add a palette export mapping - used in conjunction with Bitmap exporter, it provides
	// the palette for the given bitmap. (not needed now we don't export BMPs)
//	PaletteClipMap::CreateAndRegister();

#if FALSE
	// And add our Quark EPS import filter
	QuarkPictureClipMap::CreateAndRegister();
#endif

	// Read the bug-fix preference for CMX.
	if (!GetApplication()->DeclareSection(TEXT("OLE Local Server"), 5) ||
		!GetApplication()->DeclarePref(TEXT("OLE Local Server"),
									   TEXT("DontExportCMXInIDataObject"),
									   &fNoCMXDataExport, FALSE, TRUE))
	{
		TRACEUSER( "JustinF", _T("Couldn't read CMX pref in ExternalClipboard::Init\n"));
		return FALSE;
	}

	return(TRUE);
}



/********************************************************************************************

>	static void ExternalClipboard::Deinit(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	De-Initialises the ExternalClipboard when you're shutting down

	SeeAlso:	ExternalClipboard::Init

********************************************************************************************/

void ExternalClipboard::Deinit(void)
{
	if (pInstance != NULL)
		delete pInstance;
	pInstance = NULL;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//	The OLE clipboard
//

#if (_OLE_VER >= 0x200)

/********************************************************************************************
>	void ExternalClipboard::SetRenderMemory(HGLOBAL hMem)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/96
	Inputs:		hMem		---		the global memory to delay-render into, or null if
									the rendering functions themselves should allocate
									it.
	Purpose:	Sets the memory that the delay-render functions will render into.
	SeeAlso:	CCamDataSource::OnRenderGlobalData
********************************************************************************************/

void ExternalClipboard::SetRenderMemory(HGLOBAL hMem)
{
	// Record the handle and its size, if any.
	m_hMem = hMem;
	m_cbMemSize = (m_hMem != 0) ? ::GlobalSize(m_hMem) : 0;
}




/********************************************************************************************
>	HGLOBAL ExternalClipboard::GetRenderMemory() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/96
	Returns:	The global memory block that was delay-rendered into.
	Purpose:	
	SeeAlso:	ExternalClipboard::SetRenderMemory
********************************************************************************************/

HGLOBAL ExternalClipboard::GetRenderMemory() const
{
	return m_hMem;
}

#endif



/********************************************************************************************
>	BOOL ExternalClipboard::IsOurs() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/96
	Returns:	TRUE if Camelot currently owns the clipboard, FALSE if it doesn't or the
				clipboard is empty.
	SeeAlso:	COleDataSource::GetClipboardOwner; CWnd::GetClipboardOwner
********************************************************************************************/

BOOL ExternalClipboard::IsOurs() const
{
#if (_OLE_VER >= 0x200)
	// OLE does it with data sources (but paste still does it with window handles).
	return CCamDataSource::GetClipboardOwner() != 0 ||
		   CWnd::GetClipboardOwner() == GetOwnerWindow();
#else
	// Windows does it with window handles.
	return CWnd::GetClipboardOwner() == GetOwnerWindow();
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	static void ExternalClipboard::PrepareForShutdown()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/95

	Purpose:	Asks the user if they want to keep the clipboard content and renders the
				formats immeidately if they said yes. This is called on exit, BEFORE any
				windows close etc, so that the user is aware that Camelot is still running
				and that it is busy exporting the clipboard.

				The user may not be asked at all (e.g. if we don't own the clipboard)

	SeeAlso:	ExternalClipboard::RenderAllFormats; ExternalClipboard::CheckKeepClipboardOnExit

********************************************************************************************/

BOOL fBodgeNoCMX = FALSE;

void ExternalClipboard::PrepareForShutdown()
{
	// Is there a clipboard?
	if (InternalClipboard::IsEmpty(FALSE)) return;

	ExternalClipboard* pClipboard = GetExternalClipboard();
	if (pClipboard == NULL) return;

	pClipboard->CheckKeepClipboardOnExit();

#if (_OLE_VER >= 0x200)

	if (pClipboard->KeepFormatsOnExit)
	{
		// Render all the clipboard formats now (except CMX cos it may crash if there's a
		// bitmap or a fractal about - doh-doh-double-doh?!?!?)
//		TRACEUSER( "JustinF", _T("Rendering clipboard formats now ...\n"));
		fBodgeNoCMX = TRUE;
		CCamDataSource::FlushClipboard();
		fBodgeNoCMX = FALSE;
	}
	else
	{
		// Discard the clipboard contents now.
//		TRACEUSER( "JustinF", _T("Discarding clipboard formats ...\n"));
		COleDataSource* pSource = CCamDataSource::GetClipboardOwner();
		if (pSource) pSource->Empty();
	}

#else

	// This member var. will have been set by someone calling CheckKeepClipboardOnExit()
	if (!pClipboard->KeepFormatsOnExit)
	{
		// They don't want to keep the clipboard formats, so wipe the clipboard.
		CWnd* pParent = GetOwnerWindow();
		if (pParent->OpenClipboard() != NULL)
		{
			::EmptyClipboard();	
			::CloseClipboard();
		}
		return;
	}

	// They do want to keep the clipboard contents, so render all formats to it now
	pClipboard->RenderAllFormats();

#endif

	// And make sure that we ignore any further attempt to make us render formats
	pClipboard->KeepFormatsOnExit = FALSE;
}



/********************************************************************************************

>	void ExternalClipboard::CheckKeepClipboardOnExit(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95

	Purpose:	Called once on exit to check if the user wants the contents of the clipboard
				to be available after camelot is gone. This is supposed to be done when
				RenderAllFormats is called, but this happens after the main frame window has
				departed, so we want someone to call this method sometime between the last
				document being closed and the main frame being removed.

				It flags the user's choice, and this result will be used if/when we get
				a render-all-formats request.

				The user may not be asked at all (e.g. if we don't own the clipboard)

	SeeAlso:	ExternalClipboard::RenderAllFormats

********************************************************************************************/

void ExternalClipboard::CheckKeepClipboardOnExit(void)
{
	if (InternalClipboard::IsEmpty(FALSE))					// No clipboard!
		return;

	// Do we own the clipboard?
	if (!IsOurs()) return;

	// OK, Now let's determine just how complicated the data is. If it looks like it
	// will take an INT32 time or a lot of memory to leave it on the CB, then we'll
	// ask the user - if not, we'll just stay quiet about it and lump it all onto
	// the CB without asking permission.
	// This is done with a very rough guide: A node count of the clipboard content,
	// modified slightly to take account of the fact that blends and grad fills usually
	// cause a lot more output (in WMF format etc). 
	Node *pRoot = InternalClipboard::GetInsertionLayer();
	if (pRoot == NULL)
	{
		KeepFormatsOnExit = TRUE;
		return;
	}

	Node *pNode = pRoot->FindFirstDepthFirst();
	INT32 RoughNodeCount = 0;

	while (pNode != NULL && RoughNodeCount < KeepClipboardThreshold)
	{
		if (pNode->IsAVisibleTextNode())
		{
			RoughNodeCount += 1;			// Text chars export small, so count as 1
		}
		else if (IS_A(pNode, NodeBlend))
		{
											// Each blend step counts as another 8
			RoughNodeCount += 128 * (((NodeBlend *)pNode)->GetNumBlendSteps());
		}
		else if (pNode->IsAnAttribute())
		{
											// Each fill counts as 64 (in case it's a grad fill)
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)))
				RoughNodeCount += 64;
		}
		else if (IS_A(pNode, NodeBitmap))
		{
											// Add Bitmap memory size to the count
											// This will hit the threshold at about 128kB
			KernelBitmapRef *BmpRef = ((NodeBitmap*)pNode)->GetBitmapRef();
			if (BmpRef != NULL)
			{
				KernelBitmap *KernelBmp = BmpRef->GetBitmap();
				if (KernelBmp != NULL)
				{
					OILBitmap *OilBmp = KernelBmp->ActualBitmap;
					if (OilBmp != NULL)
					{
						BitmapInfo BmpInfo;
						OilBmp->GetInfo(&BmpInfo);
						RoughNodeCount += BmpInfo.MemoryUsed / 32;
					}
				}
			}
			
		}
		else
			RoughNodeCount += 10;			// All other nodes count as 10

		pNode = pNode->FindNextDepthFirst(pRoot);
	}

	// Default to saving the clipboard content on exit...
	KeepFormatsOnExit = TRUE;

	if (RoughNodeCount >= KeepClipboardThreshold)
	{
		// It looks a bit complicated, so maybe we'd better ask the user if they want the
		// clipboard contents to stick around once Camelot has quit.
		INT32 ButtonPressed = AskQuestion(_R(IDS_CLIPBOARD_KEEPONQUIT),
										_R(IDS_CLIPBOARD_KEEP), _R(IDS_CLIPBOARD_DISCARD));

		// Then remember whether we should render all available formats to the clipboard
		KeepFormatsOnExit = (ButtonPressed == 1);
	}
}


/********************************************************************************************

>	static BOOL ExternalClipboard::CanImport(UINT32 WindowsFormat)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Inputs:		WindowsFormat - the windows format (e.g. CF_ constant) which you wish to check

	Returns:	TRUE if it can import that data type

	Purpose:	Determines if there is an import mapping that can cope with the given format

********************************************************************************************/

BOOL ExternalClipboard::CanImport(UINT32 WindowsFormat)
{
	if (pInstance == NULL)
		return(FALSE);

	ClipboardMapping *Mapping = (ClipboardMapping *) pInstance->Mappings.GetHead();
	while (Mapping != NULL)
	{
		if (Mapping->Type != CLIPMAP_EXPORTONLY && Mapping->ExternalDataType == WindowsFormat)
			return(TRUE);

		Mapping = (ClipboardMapping *) pInstance->Mappings.GetNext(Mapping);
	}

	return(FALSE);
}



/********************************************************************************************

>	static void ExternalClipboard::RegisterDataType(ClipboardMapping *Mapping)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TypeInfo - info describing the import/export mapping to add

	Purpose:	Called by filters to add import/export mappings to the external clipboard.
				Each mapping indicates which filter to call in order to convert data 
				between a particular internal and external clipboard format.

	Notes:		The Mapping is NOW OWNED by the ExternalClipboard. DO NOT DELETE IT.

********************************************************************************************/

void ExternalClipboard::RegisterDataType(ClipboardMapping *Mapping)
{
	ERROR3IF(Mapping == NULL, "Illegal NULL param");

	if (pInstance == NULL)
	{
		ERROR3("Attempt to register a clipboard mapping when there is no ExternalClipboard");
		delete Mapping;
		return;
	}

	List *TheList = &pInstance->Mappings;
	ClipboardMapping *Ptr = (ClipboardMapping *) TheList->GetHead();

	while (Ptr != NULL)
	{
		if (Ptr->Priority < Mapping->Priority)
		{
			TheList->InsertBefore(Ptr, Mapping);
			return;
		}

		Ptr = (ClipboardMapping *) TheList->GetNext(Ptr);
	}		

	// Haven't found a lower-priority entry, so this one belongs at the tail
	TheList->AddTail(Mapping);
}



/********************************************************************************************

>	static void ExternalClipboard::DescribeContents(String_64 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/95

	Outputs:	Result will be returned with a string describing the current clipboard
				format. This may be the format of the internal clipboard (quickshape,
				line, etc) or could be from the external clipboard (metafile, text, bitmap)

 				If no special information can be given, or if the clipboard is empty, Result
 				will contain an empty string

	Purpose:	To get the textual description of the clipboard format which will be pasted
				if a paste operation is to be applied now.

********************************************************************************************/

void ExternalClipboard::DescribeContents(String_64 *Result)
{
	ERROR3IF(Result == NULL, "Illegal null param");

	// Ensure a safe return value
	*Result = TEXT("");

	if (pInstance == NULL)			// There ain't no clipboard hereabouts, mac!
		return;

 	if (pInstance->IsOurs() || !InternalClipboard::IsEmpty(FALSE))
	{
 		// We own the clipboard, or have a cached result - Use the internal one
		ERROR3IF(InternalClipboard::IsEmpty(FALSE),
				"We own the clipboard but the internal clipboard is empty!!");

		InternalClipboard::DescribeContentsInternal(Result);
		return;
	}

	if (GetOwnerWindow()->OpenClipboard() == NULL)
		return;					// Can't open the clipboard

	UINT32 ThisFormat = 0;
	UINT32 DefaultImportFormat = 0;
	INT32 NumExternalFormats = 0;
	BOOL IncludesTextFormat = FALSE;
	do
	{
		ThisFormat = EnumClipboardFormats(ThisFormat);
		if (ThisFormat != 0 && ExternalClipboard::CanImport(ThisFormat))
		{
			// Bodge - text can be on the clipboard in 3 forms, but we only want to treat
			// it as a single format for the 'paste special' count
			if (ThisFormat == CF_TEXT || ThisFormat == CF_UNICODETEXT || ThisFormat == CF_OEMTEXT)
			{
				if (!IncludesTextFormat)
					NumExternalFormats++;

				IncludesTextFormat = TRUE;
			}
			else
				NumExternalFormats++;

			// Remember which format we will import by default
			if (DefaultImportFormat == 0)
				DefaultImportFormat = ThisFormat;
		}
	} while (ThisFormat != 0);

	::CloseClipboard();

	if (NumExternalFormats > 1)
	{
		// if there are 2 or more external formats, then we'll paste special ("Paste...")
		*Result = _R(IDS_CLIPFORMAT_ELIPSES); // TEXT("...");
	}
	else
	{
		// There is no data, or only one format that we recognise. If there is something we
		// can paste, then lob its name on the end ("Paste blobby"), else leave result blank ("Paste")
		if (DefaultImportFormat != 0)
			GetExternalFormatName(DefaultImportFormat, Result);
	}
}



/********************************************************************************************

>	static void ExternalClipboard::GetExternalFormatName(UINT32 ExternalFormat, String_64 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95

	Inputs:		ExternalFormat - A windows clipboard format ID (e.g. a CF_ constant)

	Outputs:	Result will be returned with a string describing this format

	Purpose:	To get the textual description of a clipboard format

	Notes:		Microsoft, being in a particularly helpful frame of mind, have
				given us a function that does not provide the names of the default
				(CF_ constant) clipboard formats. We must therefore recognise all the
				likely formats and provide our own names. Thanks, guys.

********************************************************************************************/

void ExternalClipboard::GetExternalFormatName(UINT32 ExternalFormat, String_64 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");

	if (ExternalFormat >= 0xC000 && ExternalFormat <= 0xFFFF)
	{
		GetClipboardFormatName(ExternalFormat, (LPTSTR)((TCHAR *)*Result), 63);
		if (*Result==String_64(_T("CorelXARA Document")))
			*Result = String(_R(IDS_CLIPFORMAT_NATIVE));
	}
	else
	{
		switch(ExternalFormat)
		{
			case CF_TEXT:
				*Result = String(_R(IDS_CLIPFORMAT_ATEXT));
				break;

			case CF_BITMAP:
				*Result = String(_R(IDS_CLIPFORMAT_BMP));
				break;

			case CF_METAFILEPICT:
				*Result = String(_R(IDS_CLIPFORMAT_WMF));
				break;

//			case CF_SYLK:
//				*Result = TEXT("Symbolic Link");
//				break;

//			case CF_DIF:
//				*Result = TEXT("Data Interchange Format");
//				break;

			case CF_TIFF:
				*Result = String(_R(IDS_CLIPFORMAT_TIFF));
				break;

			case CF_OEMTEXT:
				*Result = String(_R(IDS_CLIPFORMAT_ATEXT));	// Shares 'unformatted text' with CF_TEXT
				break;

			case CF_DIB:
				*Result = String(_R(IDS_CLIPFORMAT_DIB));
				break;

//			case CF_PALETTE:
//				*Result = TEXT("Palette");
//				break;

//			case CF_PENDATA:
//				*Result = TEXT("Pen data");
//				break;

//			case CF_RIFF:
//				*Result = TEXT("RIFF data");
//				break;

//			case CF_WAVE:
//				*Result = TEXT("WAVE sound");
//				break;

			case CF_UNICODETEXT:
				*Result = String(_R(IDS_CLIPFORMAT_UTEXT));
				break;

			case CF_ENHMETAFILE:
				*Result = String(_R(IDS_CLIPFORMAT_EMF));
				break;

			default:
				*Result = String(_R(IDS_CLIPFORMAT_UNRECOGNISED_FORMAT)); // "Unrecognised format"
				TRACE( _T("WARNING: Unrecognised 'standard' windows clipboard format (%ld), %s,%ld\n"),
						(INT32)ExternalFormat, __FILE__, __LINE__);
				break;
		}
	}


// If you want to hack a clipboard format, this little baby knows
// how to dump it into the file c:\jason.tmp
#if FALSE
#if _DEBUG
/*
	if (!camStrncmp((TCHAR *)(*Result), "QuarkXPress", 10))	// A Quack XPress clipboard format?
	{
		HANDLE Bob = GetClipboardData(ExternalFormat);
		char *buff = (char *) GlobalLock(Bob);

		FILE *fp = _tfopen("c:\\jason.tmp", "wb");
		size_t Size = (size_t)GlobalSize(Bob);
		fwrite(buff, 1, Size, fp);
		fclose(fp);

		GlobalUnlock(Bob);

		ERROR3("Quack XPress data detected and saved!");
	}
*/
#endif
#endif
}



/********************************************************************************************

>	static BOOL ExternalClipboard::IsEmpty(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Returns:	TRUE if the clipboard is ready to accept new data
				FALSE if the copy must be aborted

	Purpose:	To ready the clipboard for a cut/copy operation (copy data TO the clipboard).
				This will interact with the external clipboard in order to claim the global
				clipboard, and readies the clipboard for use.

	Notes:		After you have copied data to the clipboard, you MUST call CopyCompleted

	Scope:		protected (for friend class InternalClipboard only)

	SeeAlso:	ExternalClipboard::CopyCompleted; ExternalClipboard::PrepareForPaste

********************************************************************************************/

BOOL ExternalClipboard::IsEmpty(void)
{
	BOOL IsEmpty = TRUE;

	if (pInstance == NULL)
		return(TRUE);

	CWnd *Parent = GetOwnerWindow();
	if (Parent == NULL) return TRUE;

 	if (pInstance->IsOurs())	// We own the clipboard! Use the internal one
	{
		if (InternalClipboard::IsEmpty(FALSE))
		{
			ERROR3("We own the clipboard but the internal clipboard is empty!!");
		}
		else
		{
			ERROR3("Why are you asking the External clipboard if it's empty when the internal one isn't?");
		}
		return(FALSE);
	}

	if (Parent->OpenClipboard() == NULL)
		return(TRUE);			// Can't open the clipboard, so we're empty

	ClipboardMapping *Mapping;
	UINT32 ThisFormat = 0;		// 0 makes EnumClipboardFormats start from the beginning
	do
	{
		ThisFormat = EnumClipboardFormats(ThisFormat);	

		if (ThisFormat != 0)
		{
			Mapping = (ClipboardMapping *) pInstance->Mappings.GetHead();
			while (Mapping != NULL && IsEmpty)
			{
				// If this mapping is an import mapping and can handle this external format,
				// then we need look no further - there is importable data on the clipboard
				if (Mapping->Type != CLIPMAP_EXPORTONLY && Mapping->ExternalDataType == ThisFormat)
					IsEmpty = FALSE;

				Mapping = (ClipboardMapping *) pInstance->Mappings.GetNext(Mapping);
			}
		}
	} while (ThisFormat != 0 && IsEmpty);

	::CloseClipboard();
	return(IsEmpty);
}



/********************************************************************************************

>	BOOL ExternalClipboard::PrepareForCopy(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Returns:	TRUE if the clipboard is ready to accept new data
				FALSE if the copy must be aborted

	Purpose:	To ready the clipboard for a cut/copy operation (copy data TO the clipboard).
				This will interact with the external clipboard in order to claim the global
				clipboard, and readies the clipboard for use.

	Notes:		After you have copied data to the clipboard, you MUST call CopyCompleted

	Scope:		protected (for friend class InternalClipboard only)

	SeeAlso:	ExternalClipboard::CopyCompleted; ExternalClipboard::PrepareForPaste

********************************************************************************************/

BOOL ExternalClipboard::PrepareForCopy(void)
{
	// Do nothing

	return(TRUE);
}



/********************************************************************************************

>	BOOL ExternalClipboard::CopyCompleted(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Returns:	TRUE if all went well
				FALSE if the copy failed in some way

	Purpose:	Tells the ExternalClipboard that you have finished copying data to it.
				This interacts with the external clipboard in order to make the new data
				available to the outside world.

	Notes:		Before starting to copy data to the clipboard you MUST call PrepareForCopy

				This is called AFTER have updated the internal clipboard. However, if we
				already own the Windows clipboard, Windows very kindly asks us to destroy
				our clipboard when we call EmptyClipboard. To avoid disembowling ourselves
				we thus have to check if we are the owner of the clipboard so that we only
				wipe the internal clipboard if someone else takes control of the
				external clipboard - see DestroyClipboard

	Scope:		protected (for friend class InternalClipboard only)

	SeeAlso:	ExternalClipboard::PrepareForCopy

********************************************************************************************/

BOOL ExternalClipboard::CopyCompleted(void)
{
//	TRACEUSER( "JustinF", _T("In ExternalClipboard::CopyCompleted\n"));
	InternalClipboardFormat VectorFormat(CLIPTYPE_VECTOR);

	ClipboardMapping *Mapping = (ClipboardMapping *) Mappings.GetHead();
	while (Mapping != NULL)
	{
		// Mark all "vector" mappings available for export
		Mapping->Available = Mapping->GetInternalDataType()->IsSameFormat(VectorFormat);
		Mapping = (ClipboardMapping *) Mappings.GetNext(Mapping);
	}	

	// Scan the InternalClipboard document tree to determine which internal formats are available
	Node *pRoot = InternalClipboard::GetInsertionLayer();
	Node *pNode = pRoot->FindFirstDepthFirst();
	BOOL AllDone = FALSE;	// Becomes TRUE when there is no need to continue the scan

	while (pNode != NULL && !AllDone)
	{
		AllDone = TRUE;
		Mapping = (ClipboardMapping *) Mappings.GetHead();
		while (Mapping != NULL)
		{
			if (!Mapping->Available && Mapping->Type != CLIPMAP_IMPORTONLY)
			{
				// Check if this mapping can be used on this node to provide another
				// output data format that was previously unavailable
				Mapping->Available = pNode->SupportsClipboardFormat(Mapping->GetInternalDataType());
				if (!Mapping->Available) AllDone = FALSE;
			}

			Mapping = (ClipboardMapping *) Mappings.GetNext(Mapping);
		}	
		
		pNode = pNode->FindNextDepthFirst(pRoot);
	}


#if (_OLE_VER >= 0x200)

	// Put the contents of the internal clipboard on the OLE clipboard, using the
	// associated CCamSrvrItem to do the hard stuff.
	
	m_fIgnoreChange = TRUE;
	
	// Only put stuff on the external clipboard if we are allowed to ...
	if (m_fUpdateSystemClipboard)
	{
		// If you have been using something like GIF construction set then this may
		// lock the clipboard. At this point, an exception will be thrown. This is an unknown
		// exception as far a Camelot is concerned and so is very bad. We should really stop
		// the main expection handler getting its claws on it and not knowing what to do with it.
		// Our exception handler can just pretend that nothing really happened.
		// Just mention to the user about the problem.
		TRY
		{
			// Get the oil form of the document and the associated server item
			CCamDoc * pCamDoc = InternalClipboard::Instance()->GetOilDoc();
			CCamSrvrItem* pServer = NULL;
			if (pCamDoc != NULL)
				pServer = pCamDoc->GetEmbeddedItem();
			// If that went well then ask it to copy the data to the clipboard
			if (pServer != NULL)
				pServer->CopyToClipboard();
		}
		CATCH(COleException, e)      // Handle all ole exceptions
		{
			// "e" contains information about the exception.
			TRACEUSER( "Neville", _T("ExternalClipboard::CopyCompleted exception handler\n"));

			// Do nothing special, as all this means is that the external clipboard
			// has had a problem. Just report a generic error to the user so that they
			// know that something is not working correctly.
			InformError(_R(IDE_CLIPBOARDCOPYPROBLEM)); // There was a problem copying to the Windows clipboard
		}
		END_CATCH
	}
	
	m_fIgnoreChange = FALSE;
	return TRUE;

#else

	CWnd *Parent = GetOwnerWindow();
	if (Parent->OpenClipboard() != NULL)
	{
		::EmptyClipboard();						// Clear the windows clipboard

		// Add all available export formats onto the clipboard
		Mapping = (ClipboardMapping *) Mappings.GetHead();
		while (Mapping != NULL)
		{

#if FALSE
#if _DEBUG
TRACEUSER( "Jason", _T("Mapping %s available (for ICF %ld)\n"),
  Mapping->Available?"is":"NOT", (INT32)(Mapping->GetInternalDataType()->GetFormatID()));
#endif
#endif

			if (Mapping->Available && Mapping->Type != CLIPMAP_IMPORTONLY)
				::SetClipboardData(Mapping->ExternalDataType, NULL);

			Mapping = (ClipboardMapping *) Mappings.GetNext(Mapping);
		}	

		::CloseClipboard();
	}
	// else another application/window has the clipboard open!

	return(TRUE);

#endif
}


#if (_OLE_VER >= 0x200)

/********************************************************************************************
>	void ExternalClipboard::DelayRenderData(CCamDataSource* pDataSource)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/96
	Inputs:		pDataSource		---		the data source to render into
	Purpose:	Delay-renders the old-style Camelot clipboard formats into the given
				data source.
	SeeAlso:	CCamSrvrItem::OnRenderGlobalData; CCamDataSource
********************************************************************************************/

void ExternalClipboard::DelayRenderData(CCamDataSource* pDataSource)
{
	// JustinF says: bodge alert!  We don't want to export *any* proprietary formats, such
	// as the native format or OLE embeddables, when this bodgey flag is FALSE.  This finds
	// out the flag's value without changing it.
	BOOL fExportNative = CCamSrvrItem::RenderProprietaryFormats(TRUE);
	CCamSrvrItem::RenderProprietaryFormats(fExportNative);

	// Tell the data source that we can render some static formats.
	for (ClipboardMapping* pMap = (ClipboardMapping*) pInstance->Mappings.GetHead();
		 pMap != 0;
		 pMap = (ClipboardMapping*) pInstance->Mappings.GetNext(pMap))
	{
		// JustinF says: bodge alert!  We mustn't render CMX formats on shut-down because they
		// crash if the clipboard doc contains bitmaps or fractals.
		if (pMap->IS_KIND_OF(CMXClipMap) && (fBodgeNoCMX || fNoCMXDataExport))
		{
			TRACEUSER( "JustinF", _T("Skipping CMX mapping at 0x%p\n"), (LPVOID) pMap);
			continue;
		}

		// JustinF says: bodge alert!  Don't render the native format if this flag is FALSE.
		if (pMap->IS_KIND_OF(NativeClipMap) && !fExportNative)
		{
			TRACEUSER( "JustinF", _T("Skipping native mapping at 0x%p\n"), (LPVOID) pMap);
			continue;
		}		

		// Can we export this format?
		if (pMap->Available && pMap->Type != CLIPMAP_IMPORTONLY)
		{
			// Yes, tell the data source to make this format available.
			pDataSource->DelayRenderData(pMap->ExternalDataType);
/*			TRACEUSER( "JustinF", _T("\t- added format 0x%X to COleDataSource at 0x%p\n"),
						(UINT32) pMap->ExternalDataType, (LPVOID) pSource);
*/		}
	}
}

#endif



#if (_OLE_VER >= 0x200)

/********************************************************************************************
>	static CLIPFORMAT ExternalClipboard::GetBestPasteFormat(COleDataObject* pDataObject)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/96
	Inputs:		pDataObject		---		the data object containing the data to be queried
	Returns:	The data format within the data object that is the most suitable for
				pasting, or zero if there isn't one.
	Purpose:	Works out what we would paste from the given data object.
	SeeAlso:	ExternalClipboard::PrepareForOlePaste
********************************************************************************************/

CLIPFORMAT ExternalClipboard::GetBestPasteFormat(COleDataObject* pDataObject)
{
	FORMATETC fmt;
	pDataObject->BeginEnumFormats();
	while (pDataObject->GetNextFormat(&fmt))
	{
		// Ask all the clipboard mappings if they can import this format.
		ClipboardMapping* pMap = (ClipboardMapping*) pInstance->Mappings.GetHead();
		while (pMap)
		{
			// Can this mapping import this type of data?
			if (pMap->Type != CLIPMAP_EXPORTONLY && pMap->ExternalDataType == fmt.cfFormat)
			{
				// Yes it can.  Check for implicit conversions between ANSI and Unicode text.
				if (pMap->ExternalDataType == pMap->RealExternalType ||
					pDataObject->IsDataAvailable(pMap->RealExternalType))
				{
					// We found a format.  Return it.
					return pMap->RealExternalType;
				}
			}

			// Consider the next mapping.
			pMap = (ClipboardMapping*) pInstance->Mappings.GetNext(pMap);
		}
	}

	// No format is suitable, so sorry.
	return 0;
}



/********************************************************************************************
>	BOOL ExternalClipboard::PrepareForOlePaste(COleDataObject* pDataObject)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/96
	Inputs:		pDataObject		---		the data object containing data to be 'pasted' into
										Camelot.
	Returns:	TRUE if successful.
	Purpose:	'Pastes' from an MFC/OLE COleDataObject, rather than simply the clipboard.
	SeeAlso:	ExternalClipboard::PrepareForPaste; ExternalClipboard::GetBestPasteFormat
********************************************************************************************/

BOOL ExternalClipboard::PrepareForOlePaste(COleDataObject* pDataObject)
{
	// Iterate through all the formats in the data object.  We will paste the first that
	// is acceptable.
	ERROR2IF(!pDataObject, FALSE,
				"Null COleDataObject* in ExternalClipboard::PrepareForOlePaste");
	BOOL fOK = FALSE;
	FORMATETC fmt;
	pDataObject->BeginEnumFormats();
	while (pDataObject->GetNextFormat(&fmt))
	{
		// Ask all the clipboard mappings if they can import this format.
		ClipboardMapping* pMap = (ClipboardMapping*) Mappings.GetHead();
		while (pMap)
		{
			// Can this mapping import this type of data?
			if (pMap->Type != CLIPMAP_EXPORTONLY && pMap->ExternalDataType == fmt.cfFormat)
			{
				// Yes it can.  Check for implicit conversions between ANSI and Unicode text.
				if (pMap->ExternalDataType == pMap->RealExternalType ||
					pDataObject->IsDataAvailable(pMap->RealExternalType))
				{
					// A Bodge In Two Parts.  By Markn
					// Firstly, Clipboard docs don't have any doc views.
					// GetGlobalData() has the side-effect of invoking a call to OnActivateView()
					// Our implementation of OnActivateView() is to set the selected view to be a 
					// view in the current doc.
					// Unfortunately, when it can't find a selected view, it sets the selected and current
					// view to NULL, BUT ALSO SETS CURRENT & SELECTED DOC TO NULL just for a laugh.
					// Bodge part 1 remembers the doc, & part 2 ensures it gets set back again.
					// This has been done locally because we are 1 day away from release, and I'm buggered
					// if I'm going to change a part of camelot that's called all over the shop.

					// Bodge part 1: Remember the selected doc
					Document* pSelDoc = Document::GetSelected();

					// Extract the data from the data object into a block of memory.
					HGLOBAL hMem = pDataObject->GetGlobalData(pMap->RealExternalType);
					ERRORIF(!hMem, _R(IDE_NOMORE_MEMORY), FALSE);
					
					// Bodge part 2: Restore the selected doc in case it got changed
					Document::SetSelectedViewAndSpread(pSelDoc);
					pSelDoc->SetCurrent();

					// Make sure the clipboard is empty before we import into it
					// NOTE! Don't do this before the call to pDataObject->GetGlobalData(), because
					// we may be dragging between docs in the same app, which means the clipboard may
					// actually be in used to export data when pDataObject->GetGlobalData() is called.
					InternalClipboard::Clear();

					// OK, now 'import' the data.
					CCRuntimeClass* pClass = CC_RUNTIME_CLASS(OpClipboardImport);
					OpDescriptor* pOp = OpDescriptor::FindOpDescriptor(pClass);
					if (pOp)
					{
						pOp->Invoke(&OpParam((INT32) pMap, (INT32) hMem));
						fOK = TRUE;
					}
					goto Finished;
				}
			}

			// Consider the next mapping.
			pMap = (ClipboardMapping*) Mappings.GetNext(pMap);
		}
	}

Finished:
	// Not sure what to do here!  Do we destroy the data object?
	return fOK;
}

#endif



/********************************************************************************************

>	BOOL ExternalClipboard::PrepareForPaste(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Returns:	TRUE if the clipboard is ready to provide data for pasting
				FALSE if the paste must be aborted (nothing to paste)

	Purpose:	To ready the clipboard for a paste operation (copy data FROM the clipboard).
				This may interact with the external clipboard in order to provide the
				necessary data.

	Notes:		After copying data from the clipboard you MUST call PasteCompleted

	Scope:		protected (for friend class InternalClipboard only)

	SeeAlso:	ExternalClipboard::PasteCompleted

********************************************************************************************/

BOOL ExternalClipboard::PrepareForPaste(void)
{
	// Open the clipboard - if this fails, return immediately
	CWnd *Parent = GetOwnerWindow();
	if (Parent->OpenClipboard() == NULL)
		return(FALSE);

	ClipboardMapping *Mapping;

	UINT32 ForceFormat = 0;
	UINT32 ThisFormat = 0;

#if DO_PASTE_SPECIAL
	// First, count how many external formats are available - if there are less than 2 formats
	// which we can import, then there is no point in showing the paste-special dialogue.
	INT32 NumExternalFormats = 0;
	BOOL IncludesTextFormat = FALSE;
	do
	{
		ThisFormat = EnumClipboardFormats(ThisFormat);
		if (ThisFormat != 0 && ExternalClipboard::CanImport(ThisFormat))
		{
			// Bodge - text can be on the clipboard in 3 forms, but we only want to treat
			// it as a single format for the 'paste special' count
			if (ThisFormat == CF_TEXT || ThisFormat == CF_UNICODETEXT || ThisFormat == CF_OEMTEXT)
			{
				if (!IncludesTextFormat)
					NumExternalFormats++;

				IncludesTextFormat = TRUE;
			}
			else
				NumExternalFormats++;
		}
	} while (ThisFormat != 0);

	// if there are 2 or more external fornmats, then pop up the paste special dialogue automatically
	if (NumExternalFormats > 1)
	{
		ForceFormat = PasteSpecialDlg::InvokeDialog();
		if (ForceFormat == 0)
		{
			// User pressed "cancel"
			::CloseClipboard();
			return(FALSE);
		}
	}

#if _DEBUG
	{
		TRACEUSER( "Jason", _T("\nClipboard formats available:\n"));
		UINT32 ThisFormat = 0;
		do
		{
			ThisFormat = EnumClipboardFormats(ThisFormat);
			if (ThisFormat != 0)
			{
				String_64 name;
				GetExternalFormatName(ThisFormat, &name);
				TRACEUSER( "Jason", _T("  %ld %s %s\n", (INT32)ThisFormat, (TCHAR *)name, CanImport(ThisFormat)?" (supported)":""));
			}
		} while (ThisFormat != 0);
	}
#endif
#endif

	BOOL IKnowWhatType = FALSE;
	ThisFormat = 0;

	// If we know we`ve only one format type and it`s a text object, find out if the clipboard
	// is holding it as a UNICODE format
	if(IncludesTextFormat && NumExternalFormats == 1)
	{
		// To find out if it`s unicode, we need to search the clipboard enumerations to
		// see if it contains the unicode type.
		IKnowWhatType = TRUE;
		UINT32 Type = EnumClipboardFormats(0);
		
		while(Type > 0)
		{
			if(Type == 13)
			{
				ThisFormat = 13;
				break;
			}

			Type = EnumClipboardFormats(Type);
		}

		// If we didn`t find the unicode type set it to the normal type!
		if(ThisFormat == 0)
			ThisFormat = 1;
	}

	do
	{
		if(!IKnowWhatType)
			ThisFormat = EnumClipboardFormats(ThisFormat);

		if (ThisFormat != 0)
		{
			if (ForceFormat == 0 || ForceFormat == ThisFormat)
				Mapping = (ClipboardMapping *) Mappings.GetHead();
			else
			 	Mapping = NULL;

			while (Mapping != NULL)
			{
				// Is this mapping an import mapping for the given format?
				if (Mapping->Type != CLIPMAP_EXPORTONLY && Mapping->ExternalDataType == ThisFormat)
				{
					// Little check here - if this is a simple mapping, then we can go ahead. however,
					// if it says 'unformatted text' but will actually try to use windows' implicit conversion
					// to unicode text, we really need to check if the 'real' data type (unicode) is also
					// available on the clipboard.
					if (Mapping->ExternalDataType == Mapping->RealExternalType ||
						::IsClipboardFormatAvailable(Mapping->RealExternalType))
					{
#if _DEBUG
{
	String_64 name;
	GetExternalFormatName(ThisFormat, &name);
	TRACEUSER( "Jason", _T("I choose to import:  %s\n"), (TCHAR *)name);
	if (Mapping->ExternalDataType != Mapping->RealExternalType)
	{
		GetExternalFormatName(Mapping->RealExternalType, &name);
		TRACEUSER( "Jason", _T("  (Which actually imports via implicit conversion to %s)\n"), (TCHAR *)name);
	}
}
#endif
						OpDescriptor *ImportOp = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpClipboardImport));
						ERROR3IF(ImportOp == NULL, "Clipboard Import OpDesc not found!");

						if (ImportOp != NULL)
						{
							OpParam Bob((INT32)Mapping, 0);
							ImportOp->Invoke(&Bob);
						}

						::CloseClipboard();
						return(!InternalClipboard::IsEmpty(FALSE));
					}

					ERROR3("Windows implicit clipboard conversion unavailable! Trying other formats");
				}

				Mapping = (ClipboardMapping *) Mappings.GetNext(Mapping);
			}
		}
	} while (ThisFormat != 0);

	::CloseClipboard();
	return(FALSE);
}



/********************************************************************************************

>	BOOL ExternalClipboard::PasteCompleted(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/95

	Returns:	TRUE if all went well
				FALSE if the paste failed in some way

	Purpose:	Tells the ExternalClipboard that you have finished copying data from it.
				This provides interaction with the external clipboard.

	Notes:		Before starting to copy data to the clipboard you MUST call PrepareForPaste

	Scope:		protected (for friend class InternalClipboard only)

	SeeAlso:	ExternalClipboard::PrepareForPaste

********************************************************************************************/

BOOL ExternalClipboard::PasteCompleted(void)
{
	// Do nothing

	return(TRUE);
}



/********************************************************************************************

>	void ExternalClipboard::DestroyClipboard(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	Destroys the external and internal clipboards.

				Called in response to Windows WM_DestroyClipboard message, when another
				application takes ownership of the clipboard away from us.

	Notes:		Now scans all registered mappings, and calls their RemoveTempFile methods
				in order to remove any tempfiles left lying about "on" the clipboard.

********************************************************************************************/

void ExternalClipboard::DestroyClipboard(void)
{
	// Whenever this method is called, we only wipe the clipboard if someone else owns it
	// (we're guaranteed to be called with us as the owner in all the situations where
	// we don't want to wipe the internal clipboard! Excellent!)
	if (!IsOurs())
	{
		// And try to remove any temporary files that were hanging around "on" the clipboard
		InternalClipboard::Clear();
		ClipboardMapping* Mapping = (ClipboardMapping*) Mappings.GetHead();
		while (Mapping != NULL)
		{
			Mapping->RemoveTempFile();
			Mapping = (ClipboardMapping*) Mappings.GetNext(Mapping);
		}
	}
}



/********************************************************************************************

>	void ExternalClipboard::WindowsClipboardHasChanged()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	Destroys the internal clipboard.

				Called whenever the contents of the Windows clipboard are changed.
				We use this to destroy the contents of our internal clipboard (cache).

********************************************************************************************/

void ExternalClipboard::WindowsClipboardHasChanged()
{
#if (_OLE_VER >= 0x200)
	// The comment below is no longer true, as now OLE takes possession of the clipboard
	// for us, which the old code thinks is someone else taking possession, and hence it
	// clears out the clipboard.  This bodgy flag stops that happening when we ask OLE
	// to set the clipboard for us.
//	TRACEUSER( "JustinF", _T("In ExternalClipboard::WindowsClipboardHasChanged\n"));
	if (m_fIgnoreChange)
	{
//		TRACEUSER( "JustinF", _T("\t- ignoring this change to the clipboard ...\n"));
		return;
	}
#endif

	// Whenever this method is called, we only wipe the clipboard if someone else owns it
	// (we're guaranteed to be called with us as the owner in all the situations where
	// we don't want to wipe the internal clipboard! Excellent!)
	if (!IsOurs() && m_fUpdateSystemClipboard) InternalClipboard::Clear();
}



/********************************************************************************************

>	void ExternalClipboard::RenderFormat(UINT32 nFmt)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		ClipboardFormat - A CF_ descriptor for a Windows clipboard format

	Purpose:	"Renders" the given format to the clipboard (i.e. exports the
				internal clipboard to the windows clipboard using the specified mapping)

				Called in response to WM_RENDER_FORMAT message

	Notes:		This is the external interface. This then calls the other override of
				RenderFormat to actually do the rendering.

				Despite what some help entries in books online may say, this method
				should NOT OpenClipboard() before doing SetClipboardData()

********************************************************************************************/

void ExternalClipboard::RenderFormat(UINT32 nFmt)
{
	if (InternalClipboard::IsEmpty(FALSE))	// No clipboard! Eek!
		return;

	// Check for all the boring old static formats.
	ClipboardMapping *Mapping = (ClipboardMapping *) Mappings.GetHead();
	while (Mapping != NULL)
	{
		// If this mapping is for export, and matches the desired output format
		if (Mapping->Type != CLIPMAP_IMPORTONLY && Mapping->ExternalDataType == nFmt)
		{
			if (Mapping->Available)
				RenderFormat(Mapping);
			else
			{
				ERROR2RAW("Requested clipboard format is unavailable!");
				InformError();
			}
			
			return;
		}
		Mapping = (ClipboardMapping *) Mappings.GetNext(Mapping);
	}	

	ERROR2RAW("Requested clipboard format is illegal (unknown)!");
	InformError();
}



/********************************************************************************************

>	void ExternalClipboard::RenderFormat(ClipboardMapping *Info)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		Info - points to the information on the available format

	Purpose:	"Renders" the given format to the clipboard (i.e. exports the
				internal clipboard to the windows clipboard using the specified mapping)

				Called in response to WM_RENDER_FORMAT message

	Notes:		This is the internal form of this method; It is called by the other
				form once the mapping has been determined.

				Despite what some help entries in books online may say, this method
				should NOT OpenClipboard() before doing SetClipboardData()

********************************************************************************************/

void ExternalClipboard::RenderFormat(ClipboardMapping* pMapping)
{
	ERROR3IF(pMapping == NULL, "Illegal NULL param");
	ERROR3IF(!pMapping->Available, "Requested clipboard format is not available!");
	ERROR3IF(pMapping->Type == CLIPMAP_IMPORTONLY, "Attempt to use import-only mapping for export");

	if (pMapping == NULL || !pMapping->Available || pMapping->Type == CLIPMAP_IMPORTONLY)
		return;

	// JustinF says: bodge alert!  We mustn't render CMX formats on shut-down because they
	// crash if the clipboard doc contains bitmaps or fractals.
	if (pMapping->IS_KIND_OF(CMXClipMap) && (fBodgeNoCMX || fNoCMXDataExport))
	{
		TRACEUSER( "JustinF", _T("Skipping CMX mapping at 0x%p\n"), (LPVOID) pMapping);
		return;
	}

	// The "export to clipboard" must be done inside an Op so it can call filters, so find the
	// OpDescriptor, and call our Export Op.
	OpDescriptor* pExportOp = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpClipboardExport));
	ERROR3IF(pExportOp == NULL, "Clipboard Export OpDesc not found!");

	// The OLE clipboard can pass you an already allocated memory block of a given size.
	if (pExportOp)
	{
	#if (_OLE_VER >= 0x200)

		// Pass on the mapping and the parameters from SetRenderMemory.
		OpClipboardExportParams prm;
		prm.pMapping = pMapping;
		prm.hMem = m_hMem;
		prm.cbMemSize = m_cbMemSize;
		prm.pDoc = m_pDoc;

		// Invoke the export op for the given format on the given memory.
		OpParam opp((INT32) &prm, 0);
		pExportOp->Invoke(&opp);

		// Maybe the operation allocated the memory, so update these just in case.
		m_hMem = prm.hMem;
		m_cbMemSize = prm.cbMemSize;

	#else

		OpParam opp((INT32) pMapping, 0);
		pExportOp->Invoke(&opp);

	#endif
	}
}



/********************************************************************************************

>	void ExternalClipboard::RenderAllFormats(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	Renders all available formats onto the windows clipboard
				Called when WM_RENDER_ALL_FORMATS is received (if the application quits
				when it owns the clipboard)

	Notes:		Simply calls RenderFormat for all available formats

				Despite what some help entries in books online may say, this method
				should NOT OpenClipboard() before doing SetClipboardData()

	SeeAlso:	ExternalClipboard::RenderFormat

********************************************************************************************/

void ExternalClipboard::RenderAllFormats(void)
{
	if (InternalClipboard::IsEmpty(FALSE))	// No clipboard! Eek!
		return;

	// **** !!!! Check if the user wants us to keep the clipboard contents
	// This should be called from elsewhere so the question box appears before the main frame
	// window has been closed...
//	CheckKeepClipboardOnExit();
	// ****

	// This member var. will have been set by someone calling CheckKeepClipboardOnExit()
	if (!KeepFormatsOnExit)
		return;

	CWnd *Parent = GetOwnerWindow();
	if (Parent->OpenClipboard() != NULL)
	{
		ClipboardMapping *Mapping = (ClipboardMapping *) Mappings.GetHead();
		while (Mapping != NULL)
		{
			if (Mapping->Type != CLIPMAP_IMPORTONLY &&  Mapping->Available)
				RenderFormat(Mapping);		// Only render the available formats

			Mapping = (ClipboardMapping *) Mappings.GetNext(Mapping);
		}

		::CloseClipboard();
	}
}



/********************************************************************************************

>	static ExternalClipboard *ExternalClipboard::GetExternalClipboard(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Returns:	A pointer to the application-wide instance of the ExternalClipboard

	Purpose:	Finds the external clipboard. Only really of any use to the InternalClipboard
				and the main window which has to pass events to the ExternalClipboard.

********************************************************************************************/

ExternalClipboard *ExternalClipboard::GetExternalClipboard(void)
{
	return(pInstance);
}



/********************************************************************************************

>	static CWnd *ExternalClipboard::GetOwnerWindow()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Returns:	A pointer to the CWnd Window which "owns" the clipboard for Camelot.

	Purpose:	Finds the main window of camelot, which is used to handle all communications
				between us and Windows regardingthe clipboard.

	Scope:		protected

********************************************************************************************/

CWnd* ExternalClipboard::GetOwnerWindow()
{
	return GetMainFrame();
}











/********************************************************************************************

>	void OpClipboardExport::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95

	Inputs:		OpDescriptor - Not Used

				pOpParam->Param1 - a (ClipboardMapping *) to control the process
				pOpParam->Param2 - unused (should be 0)

	Purpose:	Exports the current contents of the InternalClipoard onto the
				Windows clipboard.

********************************************************************************************/

void OpClipboardExport::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	// Ensure we put any undo information into the clipboard document, not the selected doc!
	pOurDoc = InternalClipboard::Instance();

	// Get a clipboard view to be used during the export
	View* OldView = View::GetCurrent();
	ClipboardView *View = new ClipboardView;
	if (View == NULL || !View->Init())
	{
		if (View != NULL)
			delete View;

		End();
		return;
	}

	pOurView = View;
	View->SetCurrent();

#if (_OLE_VER >= 0x200)

	// Get the operation parameters and do the export.
	OpClipboardExportParams* pprm = (OpClipboardExportParams*) pOpParam->Param1;
	
	// Tell the clipboard which memory to use, or to allocate, and the document.
	pprm->pMapping->SetRenderMemory(pprm->hMem, pprm->cbMemSize);
	pprm->pMapping->SetDocument(pprm->pDoc);
	
	// Do the export into the memory.  Save the memory handle and size into the parameters
	// in case HandleExport allocated the memory.
	pprm->hMem = pprm->pMapping->HandleExport(this, InternalClipboard::Instance());
	pprm->cbMemSize = ::GlobalSize(pprm->hMem);

#else

	// Export the data to the global memory block
	ClipboardMapping* pMapping = (ClipboardMapping*) pOpParam->Param1;
	HANDLE hData = pMapping->HandleExport(this, InternalClipboard::Instance());

	// And finally, chuck the data (if any) on the external clipboard
	if (hData != NULL) ::SetClipboardData(pMapping->ExternalDataType, hData);

#endif

	// Restore the old view
	pOurView = NULL;

	if (OldView != NULL) OldView->SetCurrent();
	delete View;
	End();
}




/********************************************************************************************

>	virtual void OpClipboardExport::Do(OpDescriptor* pOpDesc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		pOpDesc - the OpDescriptor that called this op
	Purpose:	Saves the Current Doc as the default document

********************************************************************************************/

void OpClipboardExport::Do(OpDescriptor* pOpDesc)
{
	ERROR3("Don't invoke OpClipboardExport with Do. I don't like it");
}



/********************************************************************************************

>	BOOL OpClipboardExport::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95

	Returns:	TRUE if it worked, FALSE if not

	Purpose:	Registers our OpDescriptor

********************************************************************************************/

BOOL OpClipboardExport::Init()
{
	return (RegisterOpDescriptor(
 								0,
 								_R(IDS_CLIPBOARD_OPEXPORT),
								CC_RUNTIME_CLASS(OpClipboardExport),
 								OPTOKEN_CLIPBOARDEXPORT,
 								OpClipboardExport::GetState
								));
}



/********************************************************************************************

>	OpState OpClipboardExport::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Purpose:	Yet another empty OpState function

********************************************************************************************/

OpState OpClipboardExport::GetState(String_256*, OpDescriptor*)
{
	OpState Bob;
	return Bob;
}











/********************************************************************************************

>	void OpClipboardImport::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95

	Inputs:		OpDescriptor - Not Used

				pOpParam->Param1 - a (ClipboardMapping *) to control the process
				pOpParam->Param2 - unused (should be 0)

	Purpose:	Imports data from the Windows clipboard into the InternalClipboard
				document. When it is done, you can tell if it succeeded by checking
				InternalClipboard::IsEmpty(FALSE);

********************************************************************************************/

void OpClipboardImport::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	// Remember the selected doc, in case it gets changed later on in the function
	Document* pSelDoc = Document::GetSelected();

	if (!ExternalClipboard::GetOwnerWindow()->OpenClipboard())
	{
		ERROR3("Couldn't open the clipboard");
		End();
		return;
	}

	InternalClipboard *ClipboardDoc = InternalClipboard::Instance();
	ClipboardMapping *Mapping = (ClipboardMapping*) pOpParam->Param1;

#if (_OLE_VER >= 0x200)

	// If Param2 is not NULL then it is an HGLOBAL extracted from a COleDataObject, in
	// ExternalClipboard::PrepareForOlePaste.  If it is NULL then we have been called
	// from PrepareForPaste and we must extract the handle from the Windows clipboard.
	HANDLE hClipData = (HANDLE) (HGLOBAL) pOpParam->Param2;
	if (!hClipData) hClipData = ::GetClipboardData(Mapping->RealExternalType);

#else

	HANDLE hClipData = ::GetClipboardData(Mapping->RealExternalType);

#endif

	// Ensure we put any undo information into the clipboard document, not the selected doc!
	pOurDoc = ClipboardDoc;

	// Get a new view for use during the import
	View* OldView = View::GetCurrent();
	ClipboardView *View = new ClipboardView;
	if (View == NULL || !View->Init())
	{
		// We failed. Set the data to NULL, and we can drop through the rest of the
		// function safely to exit.
		hClipData = NULL;
		if (View != NULL)
			delete View;
	}

	if (hClipData != NULL)
	{
		pOurView = View;	// Make sure we're using a safe view pointer during import

		View->SetCurrent();
		Camelot.UpdateSelection();

		// Import the data onto the Internal Clipboard
		Mapping->HandleImport(this, hClipData, ClipboardDoc);

		pOurView = NULL;	// For safety, as View is about to be deleted

		// And restore the previous view
		if (OldView != NULL)
			OldView->SetCurrent();
		delete View;
	}

	// Close the windows clipboard
	::CloseClipboard();

	// Restore the selected doc on entry as the selected & current doc
	Document::SetSelectedViewAndSpread(pSelDoc);
	pSelDoc->SetCurrent();		// This extra call is needed if the doc has no doc views (i.e. it's a clipboard)

	// Finished the operation
	End();
}




/********************************************************************************************

>	virtual void OpClipboardImport::Do(OpDescriptor* pOpDesc)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		pOpDesc - the OpDescriptor that called this op
	Purpose:	Saves the Current Doc as the default document

********************************************************************************************/

void OpClipboardImport::Do(OpDescriptor* pOpDesc)
{
	ERROR3("Don't invoke OpClipboardImport with Do. I don't like it");
}



/********************************************************************************************

>	BOOL OpClipboardImport::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95

	Returns:	TRUE if it worked, FALSE if not

	Purpose:	Registers our OpDescriptor

********************************************************************************************/

BOOL OpClipboardImport::Init()
{
	return (RegisterOpDescriptor(
 								0,
 								_R(IDS_CLIPBOARD_OPIMPORT),
								CC_RUNTIME_CLASS(OpClipboardImport),
 								OPTOKEN_CLIPBOARDIMPORT,
 								OpClipboardImport::GetState
								));
}



/********************************************************************************************

>	OpState OpClipboardImport::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Purpose:	Yet another empty OpState function

********************************************************************************************/

OpState OpClipboardImport::GetState(String_256*, OpDescriptor*)
{
	OpState Bob;
	return Bob;
}




/********************************************************************************************
>	virtual ClipboardView::~ClipboardView()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/96
	Purpose:	Destroys a ClipboardView, deallocating it's associated ViewState.
********************************************************************************************/

ClipboardView::~ClipboardView()
{
	delete pVState;
	pVState = 0;
}




/********************************************************************************************

>	BOOL ClipboardView::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	022/4/95

	Returns:	TRUE if initialised ok;
				FALSE if not

	Purpose:	Initialise the ClipboardView for rendering.

********************************************************************************************/

BOOL ClipboardView::Init()
{
	// Think up a particularly stupid dpi value...
	INT32 pixwidth = 150;
	INT32 pixheight = 150;
	
	// Set up our pixel size
	PixelWidth = FIXED16(72000.0 / pixwidth);
	PixelHeight = FIXED16(72000.0 / pixheight);
	ScaledPixelWidth = PixelWidth;
	ScaledPixelHeight = PixelHeight;

	// Get ourselves a ViewState object
	pVState = new ViewState;
	if (pVState == NULL) 
		return FALSE;

	// Connect this view state to this view
	pVState->pView = this;

	// Always uses the internal clipboard document
	pDoc = InternalClipboard::Instance();

	// All ok
	return TRUE;
}



/********************************************************************************************

>	BOOL ClipboardView::ViewStateChanged()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Returns:	TRUE (always).
	Purpose:	

********************************************************************************************/

BOOL ClipboardView::ViewStateChanged()
{
	// Local variables...
	DocCoord	ExtentLo;						// coord of bottom-left of logical extent
	DocCoord	ExtentHi;						// coord of top-right of logical extent
	
	// Read DocCoords extent of document & set extent in platform-indy ViewState struct.
	pDoc->GetExtents(&ExtentLo, &ExtentHi, &PhysExtent, this);
	SetExtent(ExtentLo, ExtentHi);				

	return TRUE;
}



/********************************************************************************************

>	void ClipboardView::SetViewPixelSize()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Purpose:	Sets up the pixel size for this view object.
	SeeAlso:	ClipboardView

********************************************************************************************/

void ClipboardView::SetViewPixelSize()
{
	// Set the scaled pixel size
	ScaledPixelWidth = PixelWidth / Scale;
	ScaledPixelHeight = PixelHeight / Scale;
}



/********************************************************************************************

>	void ClipboardView::ContinueRenderView(RenderRegion*, Spread*, BOOL = TRUE, BOOL = TRUE,
									 BOOL bForceImmediate = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Inputs:		N/A
	Purpose:	None - ClipboardView is a skeleton class that is only used to access screen
				characteristics.
	Errors:		Always => ERROR3

********************************************************************************************/

void ClipboardView::ContinueRenderView(RenderRegion*, Spread*, BOOL, BOOL,
									 BOOL bForceImmediate)
{
	ERROR3("ClipboardView::ContinueRenderView() should not be called!");
}



/********************************************************************************************

>	CDC *ClipboardView::GetRenderDC()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Returns:	NULL (always)
	Purpose:	None - ClipboardView is a skeleton class that is only used to access screen
				characteristics.

********************************************************************************************/

CDC *ClipboardView::GetRenderDC()
{
	return NULL;
}



/********************************************************************************************

>	BOOL ClipboardView::GetForeBackMode()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Returns:	FALSE - dialogs don't do background rendering.
	Purpose:	None - ClipboardView is a skeleton class that is only used to access screen
				characteristics.

********************************************************************************************/

BOOL ClipboardView::GetForeBackMode()
{
	// ClipboardViews don't background render.
	return FALSE;
}



/********************************************************************************************

>	void ClipboardView::SetForeBackMode(BOOL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Inputs:		N/A
	Purpose:	None - ClipboardView is a skeleton class that is only used to access screen
				characteristics.
	Errors:		ERROR3 always - dialogs cannot background render!
	SeeAlso:	ClipboardView

********************************************************************************************/

void ClipboardView::SetForeBackMode(BOOL)
{
	ERROR3("ClipboardView::SetForeBackMode() should not be called!");
}



/********************************************************************************************

>	DocRect ClipboardView::GetDocViewRect(Spread*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Inputs:		N/A
	Returns:	The bounds of the objects in the InternalClipboard
	Purpose:	To determine the docrect area that this view is viewing

********************************************************************************************/

DocRect ClipboardView::GetDocViewRect(Spread*)
{
	return InternalClipboard::GetObjectsBounds();
}



/********************************************************************************************

>	void ClipboardView::SetExtent(DocCoord, DocCoord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Inputs:		Two DocCoords describing the low and high corners of a rectangle in logical
				space describing the full extent of the document. I.E. the bottom-left corner
				of the last chapter and the top-right corner of the first.
	Purpose:	Informs the DocView that the extent of the document has changed in some way and 
				that the view must be altered to take this into account (namely the 
				scrollbars).
	SeeAlso:	ClipboardView

********************************************************************************************/

void ClipboardView::SetExtent(DocCoord lolog, DocCoord hilog)
{
	// Convert the extent given in DocCoord to WorkCoords...
	pVState->WorkAreaExtent.lo = lolog.ToWork(pDoc, this);
	pVState->WorkAreaExtent.hi = hilog.ToWork(pDoc, this);
}



/********************************************************************************************

>	WorkRect ClipboardView::GetViewRect()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (copying Tim code)
	Created:	22/4/95
	Returns:	An empty rectangle (always).
	Purpose:	None - ClipboardView is a skeleton class that is only used to access screen
				characteristics.

********************************************************************************************/

WorkRect ClipboardView::GetViewRect()
{
	WorkRect Empty;
	return Empty;
}











const INT32 PasteSpecialDlg::IDD = _R(IDD_CAM_PASTESPECIAL);
const CDlgMode PasteSpecialDlg::Mode = MODAL;

/********************************************************************************************

>	PasteSpecialDlg::PasteSpecialDlg(): DialogOp(PasteSpecialDlg::IDD, PasteSpecialDlg::Mode) 


	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Purpose:	PasteSpecialDlg constructor.

********************************************************************************************/

PasteSpecialDlg::PasteSpecialDlg() : DialogOp(PasteSpecialDlg::IDD, PasteSpecialDlg::Mode) 
{
}        



/********************************************************************************************

>	MsgResult PasteSpecialDlg::Message( CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the dialog's messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult PasteSpecialDlg::Message(Msg* Message)
{
	BOOL CloseDlg = FALSE;

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				{
					INT32 Index = 0;
					UINT32 ThisFormat = 0;
					BOOL HaveTextAlready = FALSE;
					do
					{
						ThisFormat = EnumClipboardFormats(ThisFormat);
						if (ThisFormat != 0 && ExternalClipboard::CanImport(ThisFormat))
						{
							BOOL AddToList = TRUE;
							if (ThisFormat == CF_TEXT || ThisFormat == CF_OEMTEXT || ThisFormat == CF_UNICODETEXT)
							{
								if (HaveTextAlready)
									AddToList = FALSE;

								HaveTextAlready = TRUE;
							}

							if (AddToList)
							{
								String_64 name;
								ExternalClipboard::GetExternalFormatName(ThisFormat, &name);
								SetStringGadgetValue(_R(IDC_PASTESPECIAL_LIST), (StringBase *) &name, FALSE, Index);
								Index++;
							}
						}
					} while (ThisFormat != 0);

					SetSelectedValueIndex(_R(IDC_PASTESPECIAL_LIST), 0);
				}
				break;
				

			case DIM_SELECTION_CHANGED_COMMIT:
				// Assume it was the listbox!
			case DIM_COMMIT:
			case DIM_SOFT_COMMIT:
				{
					pParam->Param1 = 0;	// Safe default

					INT32 Index = GetSelectedValueIndex(_R(IDC_PASTESPECIAL_LIST));

					BOOL HaveTextAlready = FALSE;
					UINT32 ThisFormat = 0;
					do
					{
						ThisFormat = EnumClipboardFormats(ThisFormat);
						if (ThisFormat != 0 && ExternalClipboard::CanImport(ThisFormat))
						{
							BOOL IsInList = TRUE;
							if (ThisFormat == CF_TEXT || ThisFormat == CF_OEMTEXT || ThisFormat == CF_UNICODETEXT)
							{
								if (HaveTextAlready)
									IsInList = FALSE;

								HaveTextAlready = TRUE;
							}

							if (IsInList)
							{
								Index--;
								if (Index < 0)
								{
									pParam->Param1 = ThisFormat;
									break;
								}
							}
						}
					} while (ThisFormat != 0);
				}

				CloseDlg = TRUE;
				break;

			case DIM_CANCEL:
				pParam->Param1 = 0;
				CloseDlg = TRUE;
				break;
		}
	}

	// Call down to the base class to automatically handle help, etc
	MsgResult Result = DialogOp::Message(Message);

	if (CloseDlg)
	{
		Close();
		End();
	}

	return(Result);
}  



/********************************************************************************************

>	OpState	PasteSpecialDlg::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95
	Purpose:	PasteSpecialDlg GetState method

********************************************************************************************/

OpState	PasteSpecialDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}


		 
/********************************************************************************************

>	BOOL PasteSpecialDlg::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Purpose:	PasteSpecialDlg Init method. Called by sginit.cpp

********************************************************************************************/

BOOL PasteSpecialDlg::Init(void)
{  
	return (RegisterOpDescriptor(
								0,
								_R(IDS_PASTESPECIALDLG),
								CC_RUNTIME_CLASS(PasteSpecialDlg),
								OPTOKEN_PASTESPECIALDLG,
								PasteSpecialDlg::GetState,
								_R(IDST_PASTESPECIALDLG),
								_R(IDBBL_PASTESPECIALDLG)
								)
			); 
}   
 


/********************************************************************************************

>	BOOL PasteSpecialDlg::Create()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95
	Returns:	TRUE if successful, else FALSE
	Purpose:	PasteSpecialDlg create method 

********************************************************************************************/

BOOL PasteSpecialDlg::Create()
{
	return(DialogOp::Create());
}



/********************************************************************************************

>	void PasteSpecialDlg::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95
	Inputs:		pOpParam points to a structure to recieve the return result
				pOpParam->Param1 will contain the result on return

	Purpose:	Creates then opens the dialog 

********************************************************************************************/

void PasteSpecialDlg::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL, "Come on, play by the rules");
	pParam = pOpParam;
	pParam->Param1 = 0;		// Set a safe default return result

	if (!Create())
	{
		InformError();
  		End();
	}
}



/********************************************************************************************

>	UINT32 PasteSpecialDlg::InvokeDialog(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Returns:	The Windows clipboard format chosen, or 0 to cancel

	Purpose:	Opens the paste special dialogue on screen, elicits a response, 
				and returns having chosen the format to paste. This is a MODAL Dlg.

********************************************************************************************/

UINT32 PasteSpecialDlg::InvokeDialog(void)
{
	OpParam Info(0, 0);
	OpDescriptor *PSDlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(PasteSpecialDlg));

	ERROR3IF(PSDlg == NULL,
			"PasteSpecialDlg::InvokeDialog is unable to find the Dlg OpDescriptor");

	if (PSDlg != NULL)
		PSDlg->Invoke(&Info);

	return(Info.Param1);
}




/********************************************************************************************

>	static ExternalClipboard* ExternalClipboard::LinkTempClipboard()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/96
	Inputs:		-
	Outputs:	-
	Returns:	
	Purpose:	Create a new temporary clipboard document and point the statics at it.

********************************************************************************************/

ExternalClipboard*	ExternalClipboard::LinkTempClipboard()
{
	// TODO: Make new internal and external clipboard objects
	// BODGE: Just use the existing clipboards
	return(ExternalClipboard::pInstance);
}

	
	
	
/********************************************************************************************

>	BOOL ExternalClipboard::UnlinkTempClipboard()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/96
	Inputs:		-
	Outputs:	-
	Returns:	
	Purpose:	Delete the temporary clipboard and return any static pointers to normal

********************************************************************************************/

BOOL ExternalClipboard::UnlinkTempClipboard()
{
	return TRUE;
}
