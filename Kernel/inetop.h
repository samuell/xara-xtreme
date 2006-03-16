// $Id: inetop.h 662 2006-03-14 21:31:49Z alex $
// Webster web clipart gallery - asynchronous operations and support classes
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

#ifndef _INETOP_H_
#define _INETOP_H_

#include "sgliboil.h"
#include "sglib.h"
#include "scrvw.h"
#include "camnet.h"
#include "stl.h"
#include <list>



// File download operation 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/****************************************************************************

>	class DownloadOpParam

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	3/03/97
	Purpose:	Download operation parameter structure - passes the minimum necessary information to the Op
					All operations derived from DownloadOp should be invoked with parameters derived from this class.

	NOTE:		Objects of this type and any derived types MUST BE CREATED ON THE HEAP. Also, never delete
						objects of this type once you passed them to DownloadOp or a derived class

****************************************************************************/

class DownloadOpParam : public OpParam
{
public:
	// Local path of file to download to
	PathName file;		
	// URL to be downloaded (can be http://, ftp://, file://)
	String_256 strURL;	
	// Flag indicating whether the operation should display a progress dialog. If you set it to FALSE you'll have to set up
	// your own progress indicator
	BOOL bHasProgressDlg;
	// Description of the file - this will be displayed in the progress dialog as "Downloading <description>". If empty, the filename 
	// will be used instead
	String_256 strDescription; 
	// Type of file we're downloading - must be set to one of the types enumerated in camnet.h
	FileType type;
	// Priority of the download - can be either AsynchDownload::PRIORITY_NORMAL or AsynchDownload::PRIORITY_HIGH
	AsynchDownload::Priority priority;

protected:
	DownloadOpParam();
	static void* pAllocatedMemory;

public:
	void* operator new(size_t stSize, LPCSTR lpszFileName, INT32 nLine)
	{
		return (pAllocatedMemory = SimpleCCObject::operator new(stSize, lpszFileName, nLine));
	}
	void* operator new(size_t stSize)
	{
		return (pAllocatedMemory = SimpleCCObject::operator new(stSize));
	}
};
						

#define OPTOKEN_OPDOWNLOAD _T("DownloadOp")	// Optoken for the basic download operation 

/****************************************************************************

>	class DownloadOp: public Operation

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/04/97
	Purpose:	File download operation. This class is intended as a base class
					for new Internet-related operations - it has little use on its own.
					The older internet operations used in Webster 1 are derived straight from
					Operation and were a bit difficult to implement
	SeeAlso: -

****************************************************************************/

class DownloadOp : public Operation
{         
	CC_DECLARE_DYNCREATE(DownloadOp)

public:
	DownloadOpParam* pParam;

	DownloadOp();
	virtual ~DownloadOp();
	// Standard operation overrides
	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);
	virtual void	Do(OpDescriptor *);
	virtual BOOL OnIdleEvent();
	virtual void	DoWithParam(OpDescriptor* pOp, OpParam* pDownloadOpParam);
	virtual void End(); 
	virtual BOOL OnDeathMsg() { End(); return TRUE;}
	virtual void OnDownloadSuccess(); 		// called in case the download succeeds
	virtual void OnDownloadFail();			// called in case the download fails through network or I/O errors
	virtual void OnDownloadAbort();			// called in case the download is aborted by the user
	virtual void OnDownloadPending();		// called when the download is just doing it's thang

protected:
	DOWNLOAD_HANDLE m_hDownload;
	INT32 m_nPercentageCompleted;
	virtual void OnDownloadProgress(const INT32 nPercentageCompleted) {}; // called when a new chunk of data arrives 
};





// Web fonts install
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WebFontInstallParam : public DownloadOpParam
{
public:
	String_256 strAltDescription;
	BOOL bIsTemp;
	LibraryGallery* pGallery;
		
	WebFontInstallParam();
	~WebFontInstallParam();
};


				  
#define OPTOKEN_OPASYNCHFONTINSTALL _T("AsynchFontInstall")	// Optoken for the asynchronous  font install 


/****************************************************************************

>	class OpAsynchFontInstall : public DownloadOp

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	3/03/97
	Purpose:	This operation will attempt to download and install fonts
					from a web server to the local machine
	SeeAlso:	OpAsynchClipartImport

****************************************************************************/


class OpAsynchFontInstall : public DownloadOp
{         
	CC_DECLARE_DYNCREATE(OpAsynchFontInstall)

public:
	OpAsynchFontInstall() {};
	virtual ~OpAsynchFontInstall() {};	
	static	BOOL Init();                        

protected:
	virtual void OnDownloadSuccess();
};


// Asynch bitmap import
// Note that the actual import will be performed only if the bitmap is to be set as document background;
// otherwise the operation will limit itself to downloading the fill file
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AsynchBitmapImportParam : public DownloadOpParam
{
public:
	// The next four params mirror those of BitmapImport.  This allows these
	// settings to be copied into a BitmapImport class OnDownloadSuccess().
	Document*			pTargetDoc;			// Document the bitmap is going into
	FillTypeEnum		FillType;			// The type of fill (ApplyToObject, SetBackground, AddToPage)
	UINT32				TagObjectToFill;	// Object to apply the fill to (if fill type == ApplyToObject)
	bool				TagObjectValid;		// Use Tag? (if !TagObjectValid && !pObjectValid use selection)
	NodeRenderableInk*	pObject;			// Pointer to object to fill
	bool				pObjectValid;		// Use pObject? (if !TagObjectValid && !pObjectValid use selection)
	Spread* 			pSpread;			// Spread (for FillType == SetBackground)
	DocCoord 			DropPos;			// DropPos (for FillType == AddToPage)

	AsynchBitmapImportParam();
	~AsynchBitmapImportParam() {};
};


#define OPTOKEN_ASYNCHBITMAPIMPORT _T("AsynchBitmapImport")	// Optoken for asynchronous bitmap import 


/****************************************************************************

>	class OpAsynchBitmapImport : public DownloadOp

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	3/06/97
	Purpose:	This operation will attempt to download and import a bitmap
					located on a web server
	SeeAlso:	DownloadOp

****************************************************************************/


class OpAsynchBitmapImport : public DownloadOp
{         
	CC_DECLARE_DYNCREATE(OpAsynchBitmapImport)

public:
	OpAsynchBitmapImport() {};
	virtual ~OpAsynchBitmapImport() {};	
	static	BOOL Init();                        
	virtual BOOL OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State);

protected:
	virtual void OnDownloadSuccess();
};






// Thumbnail downloading
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class OpThumbDownload;

class CCAPI ThumbDownloadParam
{
public:
	ThumbDownloadParam();
	~ThumbDownloadParam();

public:
	SGLibDisplayItem* pItem;
	OpThumbDownload* pOp;
	SGLibType type;
	BOOL bSuccess;
};
				  
#define OPTOKEN_OPTHUMBDOWNLOAD _T("ThumbnailDownload")	// Optoken for the thumbnail download 


/****************************************************************************

>	class OpThumbDownload: public Operation

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	7/01/97
	Purpose:	Thumbnail download operation
	SeeAlso: -

****************************************************************************/




class OpThumbDownload : public Operation
{         
	CC_DECLARE_DYNCREATE(OpThumbDownload)

public:
	ThumbDownloadParam* pParam;
	
	OpThumbDownload();	
	~OpThumbDownload();	

	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);

	virtual void	Do(OpDescriptor *);
	virtual BOOL OnIdleEvent();
	virtual void	DoWithParam(OpDescriptor* pOp, OpParam* pThumbDownloadParam);
	virtual void End(); 
	void Abort();
	static INT32 GetLastError(SGLibDisplayItem* pItem);
	static void ClearErrors();
	inline static void Suspend() {m_bIsSuspended = TRUE; InternetManager::Suspend();}
	inline static void Resume() {m_bIsSuspended = FALSE; InternetManager::Resume();}

protected:
	DOWNLOAD_HANDLE m_hDownload;
	INT32 m_nPercentageCompleted;
	static std::list<SGLibDisplayItem*> m_lstFailedItems;
	static INT32 ThumbsDownloaded[8];
	static INT32 InstanceCount[8];
	static INT32 ErrorCount[8];
	LibraryGallery* pGallery;
	static BOOL m_bIsSuspended;
};





// Asynchronous clipart import
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CCAPI AsynchClipartImportParam
{
public:
	AsynchClipartImportParam();
	~AsynchClipartImportParam();

public:
	PathName File;
	String_256 strURL;
	String_256 strDescription;
	BOOL Import;
	PageDropInfo DropInfo;
	BOOL bDropped;
};
				  
#define OPTOKEN_OPASYNCHCLIPARTIMPORT _T("AsynchClipartImport")	// Optoken for the asynch clipart import 


/****************************************************************************

>	class OpAsynchClipartImport: public OpClipartImport

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com> (based on Richard's OpClipartImport)
	Created:	7/01/97
	Purpose:	This class represents the Undo-able operation to import and
				open clipart files over the Internet
	SeeAlso: OpClipartImport

****************************************************************************/




class OpAsynchClipartImport : public OpClipartImport
{         
	CC_DECLARE_DYNCREATE( OpAsynchClipartImport )

public:
	AsynchClipartImportParam* pParam;

	OpAsynchClipartImport();	
	~OpAsynchClipartImport();	

	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);

	virtual void	Do(OpDescriptor *);
	virtual BOOL OnIdleEvent();
	virtual void	DoWithParam(OpDescriptor* pOp, OpParam* pAsynchClipartImportParam);
	virtual BOOL OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State);
	virtual void End(); 

protected:
	DOWNLOAD_HANDLE m_hDownload;
	Document* m_pTargetDoc;
	Document* m_pCurrentSelDoc;
};


// Web clipart folders operations 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define FOLDER_UNMODIFIED	0
#define FOLDER_UPDATED	1
#define FOLDER_NEW	2

class CCAPI AddWebFoldersParam
{
public:
	AddWebFoldersParam() { pLibGal = NULL;}

public:
	PathName localFile;
	String_256 strURL;
	String_256 strFolderName;	
	LibraryGallery* pLibGal;
	SGLibType type;
};


#define OPTOKEN_OPADDWEBFOLDERS _T("AddWebFolders")	// Optoken for the asynch web folders insertion into the gallery


class OpAddWebFolders: public Operation
{         
	CC_DECLARE_DYNCREATE(OpAddWebFolders)

public:
	AddWebFoldersParam* pParam;
	// SGLibType-indexed vector indicating the result of the operation for each gallery that may use it 
	static BOOL Success[8];

	OpAddWebFolders();	
	~OpAddWebFolders();	

	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);

	virtual void	Do(OpDescriptor *);
	virtual BOOL OnIdleEvent();
	virtual void	DoWithParam(OpDescriptor* pOp, OpParam* pAddWebFoldersParam);
	virtual BOOL OnDeathMsg(); 

protected:
	DOWNLOAD_HANDLE m_hDownload;	
};




#define OPTOKEN_OPADDWEBLIBRARY _T("AddWebLibrary")	// Optoken for the asynch web folders insertion into the gallery


class OpAddWebLibrary: public Operation
{         
	CC_DECLARE_DYNCREATE(OpAddWebLibrary);

public:
	AddWebFoldersParam* pParam;

	OpAddWebLibrary();	
	~OpAddWebLibrary();	

	static	BOOL Init();                        
	static	OpState GetState(String_256*, OpDescriptor*);

	virtual void	Do(OpDescriptor *);
	virtual BOOL OnIdleEvent();
	virtual void	DoWithParam(OpDescriptor* pOp, OpParam* pAddWebFoldersParam);
	virtual BOOL OnDeathMsg(); 

protected:
	DOWNLOAD_HANDLE m_hDownload;
	static BOOL m_bIsSuspended;
	// class-wide list of pending download handles 
	static std::list<DOWNLOAD_HANDLE> m_lstPendingDownloads;
	// SGLibType-indexed counter vectors
	static INT32 InstanceCount[8];
	static INT32 FoldersAdded[8];
	static INT32 ErrorCount[8];
	static INT32 CachedFilesUsed[8];
	inline static void Suspend() {m_bIsSuspended = TRUE; InternetManager::Suspend();} 
	inline static void Resume() {m_bIsSuspended = FALSE; InternetManager::Resume();} 
	friend class OpAddWebFolders;
};






#endif
