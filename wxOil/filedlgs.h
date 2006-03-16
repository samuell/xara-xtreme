// $Id: filedlgs.h 663 2006-03-15 09:27:39Z phil $
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
// The File Dialog Classes used by Open, Save, SaveAs, Import and Export.


#ifndef INC_FILEDLGS
#define INC_FILEDLGS

// make sure this is defined if you want the "new XP-style" Open/Save dialogs:
#define XPDialog

#include "filters.h" 
#include "monotime.h"

class PathName;
class String_256;
class RenderRegion;
class BitmapExportOptions;

// Useful global functions
// These start up the render regions
RenderRegion* CreateOSRenderRegion(DocRect* pRequiredSize, ReDrawInfoType* ExtraInfo,
												BOOL UseSelViewColContext = FALSE);
BOOL DestroyOSRenderRegion(RenderRegion* pRender);

#ifdef XPDialog
// needed to show the "XP-style" place bar on the left hand side of the File dialogs:
struct OPENFILENAMEEX : public OPENFILENAME
{
	void *       pvReserved;
	DWORD        dwReserved;
	DWORD        FlagsEx;
};
#endif

/********************************************************************************************
>	class BaseFileDialog : public CFileDialog

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Purpose:	To provide a custom File Save / Open Dialog that goes to the correct directory etc
********************************************************************************************/

class BaseFileDialog : public CFileDialog
{
public:
	// Preferences to store the last path for the dialogs
	static String_256 DefaultSaveFilePath;
	static String_256 DefaultOpenFilePath;
	static INT32		  SelectedFilter;
	// Preferences for flag states
	static BOOL		  MakeBakFiles;
	static BOOL		  ShowPreviewBitmap;
	static BOOL		  ThumbNailMode;

	// Functions to declare the file paths etc
	static BOOL Init();

	// Creation & destruction.
	BaseFileDialog(BOOL IsFileOpen, DWORD dwFlags, LPCSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);
	~BaseFileDialog();

	// Initialisation functions.
	virtual BOOL PrepareDialog();

	// Public functions to access the structures such as pathnames etc in a nice kernel fashion
	virtual INT32 GetSelectedFilterIndex(); 
	virtual INT32 SetSelectedFilterIndex(INT32 NewIndex);
	
	virtual BOOL SetTitle(INT32 TitleId); 

	virtual BOOL SetInitialDirectory(const String_256& pDir);

	virtual BOOL SetDefaultFileName(String_256& Name);
	
	virtual BOOL GetChosenFileName(PathName * pName);

	virtual Filter * GetSelectedExportFilter(INT32 TheSelectedFilter); 
	virtual void SetSelectedExportFilter(UINT32 uiFilterID);

	virtual INT32 OpenAndGetFileName();

	// Function to build the filter list for both exporters and importers
	static char *BuildFilterString(BOOL GetImport, UINT32 SelectedFilterID, INT32 *SelectedFilter,
								   CCRuntimeClass *pTypeOfFilter = CC_RUNTIME_CLASS(Filter),
								   UINT32 Bpp = 0, UINT32 BitmapCount = 1);

	// Function to add the extension to a filename e.g. .art or .xar
	virtual void AppendExtension(CString* pFilename);

	// this is a version of OnFileNameOK that helps work around the win 95 bug
	virtual BOOL IsValidFilename();

#ifdef XPDialog
// needed to show the "XP-style" place bar on the left hand side of the File dialogs:
	virtual INT32 DoModal();
#endif

//	WEBSTER-ranbirr-27/03/97
	// Added for Webster - RanbirR. Removes the extension from a file name.
#ifdef WEBSTER
	virtual BOOL RemoveFileExtension(CString* pFilename);
#endif //WEBSTER


	// these need to be public so that the CMainFrame class
	// can access them when the palette changes...
	// This points at the current running instance of a file dialog.
	static BaseFileDialog* m_pCurrentFileDialog;
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);

protected:
	enum
	{
		TITLE_SIZE = 128
	};
	// The title of the dialog
	TCHAR Title[TITLE_SIZE];

	// This is called by the hook proc.
	virtual UINT32 OnHookMsg(HWND hwnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam);

	virtual UINT32 OnFileTypeChange();

	// This is called by explorer style dialog boxes so that they can be set up ok
	virtual BOOL ExplorerInitDialog(HWND hDlg);

	BOOL ExplorerInited;

	// This is used to inform explorer style dialogs of actions
	virtual BOOL ExplorerFileHookNotify(HWND hDlg, LPOFNOTIFY pofn);
	
	// This points at the current running instance of a file dialog.
// made public - see above
//	static BaseFileDialog* m_pCurrentFileDialog;

	// This calls your overridden virtual hook proc.
	static UINT32 CALLBACK HookProc(HWND hwnd, UINT32 nMsg, WPARAM wParam, LPARAM lParam);

	// There is no memory stuff (Declare_Memdump etc) as this is an MFC derived class
	DECLARE_DYNAMIC(BaseFileDialog);

	// Preview Bitmap displaying code
	// Handle the redraw of the preview bitmap on the file dialog box.
	afx_msg LRESULT OnRedrawMessage(WPARAM, LPARAM);
	// Handle clicking on the show preview bitmap button
	afx_msg void OnShowPreviewBitmap();
	// Handle clicking on the help button
	afx_msg void OnHelp();
	// Make sure the controls in the template part of the dialog are positioned correctly
	afx_msg void OnSize(UINT32 nType, INT32 cx, INT32 cy);
	// Palette functions
// made public - see above
//	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);

	// WEBSTER - markn 24/1/97
	// Handle clicking on the options
	afx_msg void OnOptions();
	virtual BOOL HandleOptions();

	virtual void OnFileNameChange( );

	// Code to load the preview bitmap from a file.
	virtual BOOL GetPreviewBitmapFromFile   ( PathName * pPath );

	// Helper functions for importing different flavours of previews.
	virtual BOOL OpenFileForPreview	( CCDiskFile	&File,
									  PathName		*pPath );
	virtual BOOL ReadRIFFPreview	( CCDiskFile	&File,
									  PathName		*pPath );
	virtual BOOL ReadNativePreview	( CCDiskFile	&File,
									  PathName		*pPath );
	virtual BOOL ReadBitmapPreview	( CCDiskFile	&File,
									  PathName		*pPath,
									  UINT32			FilterID );
	virtual void OnFolderChange();

	// Member variables.
	KernelBitmap * pBitmapToUse;

	BOOL WantPreviewBitmap;

// needed to show the "XP-style" place bar on the left hand side of the File dialogs:
#ifdef XPDialog
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	OPENFILENAMEEX m_ofnEx;
#endif

private:
	// Added by Craig Hamilton 20/9/00.
	CString		m_previousPathName;
	// End added.
};



/********************************************************************************************
>	class OpenFileDialog : public BaseFileDialog

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Purpose:	Brings up an Open file dialog that goes to the correct directory etc.
	SeeAlso:	BaseFileDialog
********************************************************************************************/

class OpenFileDialog : public BaseFileDialog
{
public:
	// Data members.
	INT32 NativeFilterPos;

	// Creation & destruction.
	OpenFileDialog(LPCTSTR FilterString);
	BOOL PrepareDialog();

	// Function to change the default path
	void SetDefaultPath(const String_256& NewPath);

	// Function to build the filter list
	static char* BuildFilterString(INT32* NativeFilterPos);

protected:
	// This is called by explorer style dialog boxes so that they can be set up ok
	virtual BOOL ExplorerInitDialog(HWND hDlg);

private:
	// Initialisation functions.
	BOOL OnInitDialog();
	void DoDataExchange(CDataExchange* pDX);

	DECLARE_DYNAMIC(OpenFileDialog);
	DECLARE_MESSAGE_MAP()

	//void OnLBSelChangedNotify(UINT32 IDBox, UINT32 CurSel, UINT32 Code);
};


// Identifier of the compression gadget we add to the save dialog on debug versions.
#define IDC_COMPRESS_BUTTON 	(131)

/********************************************************************************************
>	class SaveFileDialog : public BaseFileDialog

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Purpose:	This is the Save Dialog that is brought up in responce to the saveas option.
				It will select the last used directory etc and is generally very fabby.
	SeeAlso:	BaseFileDialog
********************************************************************************************/

class SaveFileDialog : public BaseFileDialog
{
public:
	SaveFileDialog(LPCTSTR FilterString, String_256&, String_256&);
	SaveFileDialog(LPCTSTR FilterString, String_256& DefPath, String_256& DefName, String_256& DocumentName);

	// Initialisation function.
	virtual BOOL PrepareDialog();

	// Function to change the default path
	void SetDefaultPath(const String_256& NewPath);

	// Function to add the .art extension to a filename
	virtual void AppendExtension(CString* pFilename);

protected:
	// Our special variable which allows us to see if we have faked the closure
	// of the dialog box as we are trying to get the web export filename and we
	// cannot have two dialog boxes up at the same time
	//BOOL FakingClosure;

	// Override the baseclass version
	//virtual INT32 GetWebFileName();

	// This is called by explorer style dialog boxes so that they can be set up ok
	virtual BOOL ExplorerInitDialog(HWND hDlg);
	
	// WEBSTER - markn 24/1/97
	// Handle clicking on the options
	virtual BOOL HandleOptions();

protected:
	// Default path and file name
	String_256 DefaultName;
	String_256 DefaultPath;
	String_256 DocName;

	CButton CompressButton;
	CButton Options;
	CStatic CurrentOptions;

	// Implementation.
	BOOL OnInitDialog();
	BOOL OnFileNameOK();
	void DoDataExchange(CDataExchange* pDX);


	DECLARE_DYNAMIC(SaveFileDialog);
	DECLARE_MESSAGE_MAP()
};

/********************************************************************************************
>	class SaveTemplateDialog : public SaveFileDialog

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/97
	Purpose:	The Save Template dialog. It saves templates, basically.

	SeeAlso:	BaseFileDialog, SaveFileDialog
********************************************************************************************/

class SaveTemplateDialog : public SaveFileDialog
{
public:
	SaveTemplateDialog(PathName& pathToPutInDialog);
	static BOOL m_fUseAsDefault;
	static BOOL m_fDefaultTemplatesFolder;
	void OnUseAsDefault();
	void OnDefaultTemplatesFolder();
	void OnFileNameChange();

protected:
	String_256 m_strTitleOfDialog;

	DECLARE_DYNAMIC(SaveTemplateDialog);
	DECLARE_MESSAGE_MAP()
	
private:
	// Implementation.
	void DoDataExchange(CDataExchange* pDX);

	BOOL ExplorerInitDialog(HWND hDlg);

	BOOL PrepareDialog();
};


// the longest name we can display as a path, before going down to the basename
#define	MAX_ALLOWED_NAME_LENGTH	60
#define TotalControls 60


// Identifier of the "import" layers gadget we add to the import dialog.
#define IDC_LAYERS_BUTTON 	(130)



/********************************************************************************************
>	class ImportFileDialog : public CFileDialog

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	To provide a custom open file dialog, which enables us to resize the drop
				down list of filters.
	SeeAlso:	OpMenuImport
********************************************************************************************/

class ImportFileDialog : public BaseFileDialog
{
public:
	ImportFileDialog(LPCSTR lpszFilter = NULL);

protected:
	// This is called by explorer style dialog boxes so that they can be set up ok
	virtual BOOL ExplorerInitDialog(HWND hDlg);

private:
	CButton LayersButton;

	BOOL OnInitDialog();
	void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnLButtonDblClk(UINT32 nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(ImportFileDialog);
};



/********************************************************************************************
>	class ExportFileDialog : public CFileDialog

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/94
	Purpose:	To provide a custom open file dialog, which enables us to resize the drop
				down list of filters.
	SeeAlso:	OpMenuExport
********************************************************************************************/

class ExportFileDialog : public BaseFileDialog
{
public:
	ExportFileDialog(LPCSTR lpszFilter = NULL);

	// Function to add the extension to a filename
	virtual void AppendExtension(CString* pFilename);
	virtual void AddExtension(CString* pFilename, BOOL Replace = FALSE, BOOL AskAboutReplace = FALSE,
							  UINT32 Selection = 0 );

#if FALSE
	// for the future "Options" button handling
	BitmapExportOptions* GetExportOptions() { return m_pOptions; };
#endif

	void ExportABitmap(BOOL bExportABitmap, KernelBitmap* pBitmap = 0) 
		{ m_bExportABitmap = bExportABitmap; m_pBitmapToExport = pBitmap; }

protected:
	// This is called by explorer style dialog boxes so that they can be set up ok
	virtual BOOL ExplorerInitDialog(HWND hDlg);
	virtual BOOL HandleOptions();				// Handle clicking on the options button.

private:
	BOOL OnInitDialog();
	void DoDataExchange(CDataExchange* pDX);
	BOOL OnFileNameOK();
	
	virtual void OnTypeChange();
	virtual void OnLBSelChangedNotify(UINT32 nIDBox, UINT32 iCurSel, UINT32 nCode);

	virtual BOOL IsValidFilename();
	BOOL SetStateOnFilterChange();

	afx_msg void OnAutoPreview();
	//	afx_msg void OnFileTypeChange(NMHDR * pNotifyStruct, LRESULT * result );
	DECLARE_MESSAGE_MAP()

	DECLARE_DYNAMIC(ExportFileDialog);

protected:
	BOOL m_bExportABitmap;
	KernelBitmap *m_pBitmapToExport;
#if FALSE
	// for the future "Options" button handling
	BitmapExportOptions* m_pOptions; 
#endif
};


/**********************************************************************************************************************

>	class GIFExportFileDialog : public ExportFileDialog

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/07/97
	Purpose:	To provide a custom open file dialog for GIF Animation Export.
				This class has been derived because the help system uses the name of the class - 
				in this case, "ExportFileDialog" - to determine which help page the button should link to.
				In order to get this help button to link to a separate "Export Animated GIF" help page, 
				we have created a derived class of "ExportFileDialog" that the Export Animated GIF command can display.
				
**************************************************************************************************************************/
class GIFExportFileDialog : public ExportFileDialog
{
	DECLARE_DYNAMIC(GIFExportFileDialog);

	public:
	GIFExportFileDialog(LPCTSTR lpszFilter);	//Constructor

	virtual BOOL HandleOptions();				// Handle clicking on the options button.

	// This is called by explorer style dialog boxes so that they can be set up ok
	virtual BOOL ExplorerInitDialog(HWND hDlg);
};

#endif  // !INC_FILEDLGS
