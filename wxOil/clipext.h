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
// External clipboard interface - classes for managing communications with the windows clipboard

#ifndef INC_CLIPEXT
#define INC_CLIPEXT

#include "list.h"
#include "ops.h"
#include "selop.h"
#include "view.h"
#include "clipmap.h"
#include "dialogop.h"
#include "scrcamvw.h"


class ExternalClipboard;
class OpClipboardExport;
class OpClipboardImport;

class Filter;
class PathName;

class CCamDataSource;
class COleDataObject;



/********************************************************************************************

>	class ExternalClipboard : public CC_CLASS_MEMDUMP

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	
	Purpose:	Handlers for external (Windows) clipboard functions.
				Interacts with the internal clipboard document.

	SeeAlso:	class InternalClipboard

*********************************************************************************************/

class CCAPI ExternalClipboard : public CC_CLASS_MEMDUMP
{
friend class InternalClipboard;
friend class CMainFrame;
friend class OpClipboardImport;
friend class PasteSpecialDlg;
friend class ScreenCamView;

CC_DECLARE_MEMDUMP(ExternalClipboard)

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	The OLE clipboard
//

#if (_OLE_VER >= 0x200)

public:

	// ***TEMP: until this obsolete architecture is sorted out
	static void DelayRenderData(CCamDataSource* pDataSource);

	// Call this to set the memory the clipboard should delay-render into.  If hMem is
	// null then the render functions will try to allocate the memory themselves.
	void SetRenderMemory(HGLOBAL hMem);

	// Call this get the handle to the memory block that was rendered into (if you
	// passed null to SetRenderMemory).
	HGLOBAL GetRenderMemory() const;

	// Call this to get the most suitable format for pasting from those in the given
	// data object.  If it returns 0 then there is no suitable format.
	static CLIPFORMAT GetBestPasteFormat(COleDataObject* pDataObject);

protected:

	// This is just like PrepareForPaste but it takes the data out of the given
	// COleDataObject instead.  Pass NULL to use the COleDataObject associated
	// with the Windows clipboard.
	BOOL PrepareForOlePaste(COleDataObject* pDataObject);

private:

	// Implementation.
	HGLOBAL		m_hMem;				// the memory to delay-render into
	DWORD		m_cbMemSize;		// the size of the supplied memory
	BOOL		m_fIgnoreChange;	// whether to ignore clipboard-change messages or not
	Document*	m_pDoc;				// the kernel Document to import/export

public:

	// Call this before importing or exporting to set the document to use for import/export
	void SetDocument(Document* pDoc)			{ m_pDoc = pDoc; }

#endif

public:
	
	// This returns TRUE if the clipboard currently belongs to us.
	BOOL IsOurs() const;

/////////////////////////////////////////////////////////////////////////////////////////////


public:		// Construction/Destruction
	ExternalClipboard();
	virtual ~ExternalClipboard();


public:		// Initialisation/DeInit
	static BOOL Init(void );
	static void Deinit(void);

public:		// Registration
	static void RegisterDataType(ClipboardMapping *TypeInfo);
		// Called by filters to register mappings between internal and external types


public:		// Misc public interface
	static void PrepareForShutdown(void);
		// Asks the user and renders all formats if necessary, on shutdown.
		// Called *before* camelot closes documents and windows so it can do progress indication etc

	void CheckKeepClipboardOnExit(void);
		// Checks if the user wants the clipboard contents kept after exit

	static BOOL CanImport(UINT32 WindowsFormat);
		// Returns TRUE if there is an import mapping which can handle the given format


protected:		// InternalClipboard interface
	static BOOL IsEmpty(void);
			// Returns TRUE if the external clipboard cannot supply any data

	static void DescribeContents(String_64 *Result);
			// Finds the text description for the current clipboard contents


public:
	static void GetExternalFormatName(UINT32 ExternalFormat, String_64 *Result);
			// Finds a text description of a given windows format


protected:		// InternalClipboard interfaces
	BOOL PrepareForCopy(void);
			// Prepare the clipboard for accepting Copied/Cut data
			// Returns FALSE if the clipboard is unavailable

	BOOL CopyCompleted(void);
			// Tidy up after copying data to the clipboard
			// (Grabs ownership of the clipboard and lets windows know what formats
			// of export data are available based on the new InternalClipboard contents)

	BOOL PrepareForPaste(void);
			// Prepare the clipboard for providing data to paste
			// (Interrogates Windows to find clipboard data to grab)
			// Returns FALSE if there is nothing avaiable to paste

	BOOL PasteCompleted(void);
			// Tidy up after pasting data from the clipboard


public:		// Communications from the external (windows) clipboard

	void DestroyClipboard(void);
		// Handles a WM_DESTROYCLIPBOARD (we are no longer the clipboard owner)

	void RenderFormat(UINT32 ClipboardFormat);
	void RenderFormat(ClipboardMapping *Info);
		// Handles the WM_RENDER_FORMAT command

	void RenderAllFormats(void);
		// Handles the WM_RENDER_ALL_FORMATS command

	void WindowsClipboardHasChanged(void);
		// Called to notify us when the external clipboard has been changed by another
		// application copying to it - we must clear the internal clipboard cache


public:		// Access methods
	static ExternalClipboard* GetExternalClipboard(void);

	// Make temporary clipboards and set static pointers
	static ExternalClipboard*	LinkTempClipboard();
	BOOL						UnlinkTempClipboard();


protected:
	static ExternalClipboard* pInstance;	// The one and only instance of the ExtClipboard
	List Mappings;							// A list of the supported data mappings

	BOOL KeepFormatsOnExit;					// TRUE if we should put al formats on the clipboard
											// when we exit, FALSE if we just wipe the clipboard

protected:		// Internal/Friend-class methods
	static CWnd *GetOwnerWindow(void);

private:
	static BOOL m_fUpdateSystemClipboard;		// if FALSE then don't notify Windows etc

public:
	
	// Quick and dirty hack so that drag'n'drop doesn't get put on the Windows clipboard
	// as well!
	static void UpdateSystemClipboard(BOOL fState)
								{ m_fUpdateSystemClipboard = fState; }
};




/********************************************************************************************

>	class OpClipboardExport : public Operation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Purpose:	Exports the InternalClipboard to the external clipboard according to the
				ClipboardMapping info passed in the DoWithParam info.

********************************************************************************************/

#define OPTOKEN_CLIPBOARDEXPORT _T("ExportClipboard")

class OpClipboardExport : public Operation
{
	CC_DECLARE_DYNCREATE(OpClipboardExport)

public:
	
	virtual void DoWithParam(OpDescriptor*, OpParam* pOpParam);
	virtual void Do(OpDescriptor* pOpDesc);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
};


#if (_OLE_VER >= 0x200)

	// Use this to pass parameters to this Operation.
	struct OpClipboardExportParams
	{
		ClipboardMapping* pMapping;		// the format mapping to 'export' into the memory
		HGLOBAL			  hMem;			// the memory to export to, or null to allocate
		DWORD			  cbMemSize;	// the size of the memory to export to
		Document*		  pDoc;			// the document to export from
	};

#endif





/********************************************************************************************

>	class OpClipboardImport : public SelOperation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/95
	Purpose:	Imports the InternalClipboard from the external clipboard according to the
				ClipboardMapping info passed in the DoWithParam info.

********************************************************************************************/

#define OPTOKEN_CLIPBOARDIMPORT _T("ImportClipboard")

class OpClipboardImport : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpClipboardImport)

public:
	virtual void DoWithParam(OpDescriptor*, OpParam* pOpParam);
	virtual void Do(OpDescriptor* pOpDesc);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
};






/********************************************************************************************

>	class ClipboardView : public View

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/95

	Purpose:	Simple view class for use when rendering (exporting) the clipboard

	SeeAlso:	View

********************************************************************************************/

class ClipboardView : public View
{
	CC_DECLARE_DYNAMIC(ClipboardView);
public:
	virtual ~ClipboardView();	
	BOOL Init();

public:
	virtual BOOL ViewStateChanged();			// informs us that ViewState has changed

public:
	virtual void SetViewPixelSize();			// Set pixel size according to this view.

public:
	virtual void ContinueRenderView(RenderRegion*, Spread*, BOOL = TRUE, BOOL = TRUE,
									 BOOL bForceImmediate = FALSE);
	virtual CDC *GetRenderDC();

public:
	virtual BOOL GetForeBackMode();
	virtual void SetForeBackMode(BOOL);

public:
	virtual DocRect GetDocViewRect(Spread*);
	virtual void SetExtent(DocCoord, DocCoord);	// Reset the extent of the document
	virtual WorkRect GetViewRect();				// The physical rect of the view
};







#define OPTOKEN_PASTESPECIALDLG _T("PasteSpecialDlg")

/********************************************************************************************

>	class PasteSpecialDlg : public DialogOp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Purpose:	Allows the user to choose the clipbaord format to be pasted

	Notes:		Call InvokeDialog to show the dialogue

********************************************************************************************/

class PasteSpecialDlg : public DialogOp
{         
CC_DECLARE_DYNCREATE(PasteSpecialDlg)

public:		// External inteface
	static UINT32 InvokeDialog(void);

public:		// DialogOp interface
	PasteSpecialDlg();

	void DoWithParam(OpDescriptor*, OpParam*);
	BOOL Create(); 

	static const INT32 IDD;
	static const CDlgMode Mode;

	static OpState GetState(String_256*, OpDescriptor*);	
	static BOOL Init();                        

	virtual MsgResult Message(Msg* Message);

protected:
	OpParam *pParam;
}; 


#endif
