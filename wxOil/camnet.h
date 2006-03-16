// $Id: camnet.h 662 2006-03-14 21:31:49Z alex $
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


#ifndef _CAMNET_H_
#define _CAMNET_H_



#include <wininet.h>
#include <urlmon.h>
#include <winsock.h>
#include <afxtempl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "stl.h"
//#include "webster.h"



#define DOWNLOAD_HANDLE					INT32
#define MAX_HANDLE								1000
#define DOWNLOAD_ERROR						1
#define SIZEOFKILOBYTE	1024

#define NOTIFICATION_QUEUE_SIZE			3	// this value is very experimental

// Downloadable file types
enum FileType {TYPE_HTML = 0, TYPE_CLIPART, TYPE_FONT, TYPE_BITMAP, TYPE_TEXTURE, TYPE_COLOUR, 
		TYPE_FILL, TYPE_THUMBNAIL, TYPE_CATALOG, TYPE_SUPPORT};

// Connection types
enum ConnectionType {CONNECTION_SLOWMODEM = 0, CONNECTION_FASTMODEM, CONNECTION_X2, CONNECTION_ISDN};



typedef struct tagDOWNLOADINFO
{
	String_256 strURL;			// URL we're downloading from
	String_256 strLocalFile;	// Path of file we're downloading to
	INT32 nFileType;				// File type
	INT32 nPriority;				// Priority
	BOOL bHasProgressDlg;		// Flag indicating whether we should display a progress dialog
	String_256 strDescription; 	// ID of description string - only used if we have a progress dialog
	HWND hwndNotifyWindow;		// Window handle for status notifications
	INT32 lNotifyToken;			// Unique token for status notifications
} DOWNLOADINFO, *LPDOWNLOADINFO;






/********************************************************************************************

>	class AsynchDownload: public CCObject

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Purpose:	Asynchronous download class - downloads a url to a local file using HTTP
					Used by the download manager.
	SeeAlso:	InternetManager	

********************************************************************************************/


class AsynchDownload : public CCObject
{
	CC_DECLARE_DYNAMIC(AsynchDownload)

public:
	// Possible priority values 
	enum Priority {PRIORITY_NORMAL = 1, PRIORITY_HIGH};
	// Possible download states
	enum State {STATE_ERROR = 0, STATE_PENDING, STATE_SUCCEEDED, STATE_ABORTED, STATE_FAILED};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	AsynchDownload(DOWNLOAD_HANDLE hDownload, LPDOWNLOADINFO pDownloadInfo); 
	
	~AsynchDownload();
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Attributes
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Returns the download's handle (unique INT32 identifier)
	inline const DOWNLOAD_HANDLE GetHandle() {return m_Handle;}
	
	// Returns the URL of the file to download
	inline const String_256& GetTargetURL() {return m_strTargetURL;}

	// Returns filename of local copy of the remote file 
	inline const String_256& GetLocalFileName() {return m_strLocalFilePath;}

	// Returns the file description as displayed in the progress dialog 
	inline const String_256& GetFileDescription() {return m_strDescription;}

	// Returns the URL of the file to download
	inline const HWND& GetNotifyHWND() {return m_hwndNotifyWindow;}

	// Returns the URL of the file to download
	inline const INT32& GetNotifyToken() {return m_lNotifyToken;}

	// Returns filename of temporary cache file
	inline const String_256& GetCacheFileName() {return m_strCacheFileName;}

	// Returns size of remote file (this is the size reported by the HTTP server - not 100% reliable)
	inline const UINT32 GetRemoteFileSize() {return m_ulFileSize;} 

	// Returns bytes downloaded so far - can be used to monitor the progress of the download 
	inline const UINT32 GetBytesDownloaded() {return m_ulDownloaded;}

	// Returns the percentage downloaded so far - can be used as an alternative to the above 
	inline const INT32 GetPercentageDownloaded() {return m_nPercentageDownloaded;}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Operations
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// Cancel this download
	HRESULT AbortDownload();

	// Start this download
	inline HRESULT StartDownload() {return DoBind();}

	// Returns TRUE in case of success, FALSE otherwise
	inline BOOL HasSucceeded() { return m_bSuccess;}

	// Returns TRUE if this download was cancelled by the user rather than succeed or failed by itself
	inline BOOL WasAborted() { return m_bAbort;}

	// Get the priority assigned to this download
	inline INT32 GetFileType() { return m_nFileType;} 

	// Get the priority assigned to this download
	inline Priority GetPriority() { return m_Priority;} 

	// Safe way to free memory and resources alocated to this object - the delete operator should never be used 
	// for this purpose as it may lead to access violations
	void Release();

	// Returns the instance count of downloads with a given priority
	inline static INT32 GetInstanceCount(Priority priority) {return (priority == PRIORITY_NORMAL) ? m_nNormalPriorityInstanceCount : m_nHighPriorityInstanceCount;}



	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Implementation
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
	
	DOWNLOAD_HANDLE m_Handle; // Handle of download
	String_256 m_strTargetURL; // URL we're downloading from
	String_256 m_strLocalFilePath; // Path of file we're downloading to
	BOOL m_bHasProgressDlg;	// Flag indicating whether we should display a progress dialog while downloading
	String_256 m_strDescription; // ID of description string - only used if we have a progress dialog
	BOOL m_bAbort; // Flag signaling that the download was terminated at user's request
	BOOL m_bSuccess; // Flag indicating that the download has succeeded
	UINT32 m_ulFileSize;	// Size of file as reported by the server - note that some older servers will report 0
	UINT32 m_ulDownloaded; // Bytes downloaded so far
	INT32 m_nPercentageDownloaded; // Percentage downloaded so far
	String_256 m_strCacheFileName; // Path of cache file used by wininet.dll
    IMoniker* m_pMoniker; // pointer to URL moniker
	IBindCtx* m_pBindContext; // Pointer to bind context
	Priority m_Priority; // Download priority (either PRIORITY_NORMAL or PRIORITY_HIGH)
	HWND m_hwndNotifyWindow;		// Window handle for status notifications
	INT32 m_lNotifyToken;			// Unique token for status notifications
	INT32 m_nFileType; // File type 
	INT32 m_nAttempts; // Attempts made to download the file 
	static INT32 m_nNormalPriorityInstanceCount; // Normal priority downloads instance count
	static INT32 m_nHighPriorityInstanceCount; // High priority downloads instance count
	// Starts the binding operation which downloads the file to local storage
	// (See MS Internet SDK docs for more information)
	HRESULT DoBind(); 
	// Deallocate resources when binding is over 
	void Cleanup();
	// Re-attempt a failed download
	HRESULT Retry();
	// Nested callback class controlling the download operation
	// This is the "heart" of  the class
	class AsynchBindStatusCallback : public IBindStatusCallback, public IAuthenticate
	{
	  public:

		// data members
		DWORD           m_dwRef; // reference count
		AsynchDownload* m_pDownload; // pointer to the owner of this callback 
		IBinding*       m_pBinding; // pointer to the actual binding operation
		HWND m_hProgressDlg; // progress dialog handle
		UINT32 m_nIconID; // ID of icon currently used by the progress dialog
		// Handle of normal priority progress dialog. There is a single progress dialog for all n.p. downloads 
		// (high priority downloads have individual progress dialogs, i.e. each connection has its own)
		static HWND m_hNormalPriorityProgressDlg;
		static UINT32 m_nNormalPriorityIconID; // ID of icon currently used by the normal priority progress dialog
		static HICON m_hiconProgress;

		// Lookup table for callback objects mapping progress dialog handles to the callback
		// pointers they belong to. This is necessary as all progress dialogs share the same 
		// procedure and we may have several of them running simultaneously
		static CTypedPtrMap<CMapPtrToPtr, HWND, AsynchBindStatusCallback*> m_CallbackTable; 

  
		  // constructors/destructors
		AsynchBindStatusCallback(AsynchDownload* pDownload);
		~AsynchBindStatusCallback();
		  
		// Standard IUnknown methods
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Returns S_OK if the interface is supported, E_NOTIMPL otherwise 
		HRESULT _stdcall    QueryInterface(REFIID riid,void ** ppv);
		// Increment reference count
		inline UINT32 _stdcall    AddRef()    { return m_dwRef++; }
		// Decrement reference count; delete object if the reference count reaches 0
		inline UINT32 _stdcall    Release()   { if (--m_dwRef == 0) { delete this; return 0; } return m_dwRef; }

		// IBindStatusCallback methods - the ones we're not interested in will simply return E_NOTIMPL
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Called on start binding, allows us to retrieve the IBinding* pointer
		HRESULT _stdcall    OnStartBinding(DWORD dwReserved, IBinding* pbinding);
		// Not used
		inline HRESULT _stdcall    GetPriority(INT32* pnPriority) {return E_NOTIMPL;}
		// Not used
		inline HRESULT _stdcall    OnLowResource(DWORD dwReserved) {return E_NOTIMPL;}
		// Called when the state of the download changes (connection established/failed, new data has arrived, etc)
		// Also allows us to cancel the download by returning E_ABORT
		HRESULT _stdcall    OnProgress(UINT32 ulProgress, UINT32 ulProgressMax, UINT32 ulStateCode,
							LPCWSTR pwzStateText);
		// Called on stop binding - hrResult will be non-zero if an error occured
		HRESULT _stdcall    OnStopBinding(HRESULT hrResult, LPCWSTR szError);
		// Called just before the binding is started - allows us to request that the binding be asynchronous by setting 
		// the necessary flags in pgrfBINDF
		HRESULT _stdcall    GetBindInfo(DWORD* pgrfBINDF, BINDINFO* pbindinfo);
		// Not used
		HRESULT _stdcall    OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC *pfmtetc,
							STGMEDIUM* pstgmed);
		// Not used
		inline HRESULT _stdcall    OnObjectAvailable(REFIID riid, IUnknown* punk) {return E_NOTIMPL;}

		// IAuthenticate methods
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		STDMETHODIMP		Authenticate(HWND *phwnd, LPWSTR *pszUserName, LPWSTR *pszPassword);

		// Progress dialog functions
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Create a modeless progress dialog box and attach it to this callback 
		BOOL AttachProgressDlg();
		// Detach the callback from its progress dialog and destroy it
		void DetachProgressDlg();
		// Dialog procedure for the progress dialog
		static BOOL CALLBACK DialogProc(HWND hwndDlg, UINT32 message, WPARAM wParam, LPARAM lParam);
	};
    AsynchBindStatusCallback* m_pCallback;

	friend class AsynchBindStatusCallback;
	friend class InternetManager;
};


/********************************************************************************************

>	class DownloadQueue : public CCObject

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/12/96
	Purpose:	InternetManager support class. Represents a queue of asynchronous downloads,
					which are executed in sequence one at a time

********************************************************************************************/

class DownloadQueue : public CCObject
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(DownloadQueue);

public:
	enum QueueType {FIFO = 0, LIFO}; // valid queue types
	// Construction / destruction
	DownloadQueue() {m_enType = FIFO;}
	~DownloadQueue();
	// Add another download object to the queue
	BOOL Queue(AsynchDownload* pDownload);
	// Set the queue type "on the fly". The queue may behave like a LIFO or a FIFO depending on the type of files in the queue
	inline void SetType(QueueType enNewType) { m_enType = enNewType;}
	// Retrieve the current type
	inline QueueType GetType() { return m_enType;}
	// Test for empty
	BOOL IsEmpty() { return m_List.IsEmpty();}
	// Remove a download object from the queue 
	BOOL Remove(AsynchDownload* pDownload);
	// Flush the queue
	void Flush();
	// Get a pointer to a download object in the queue knowing its file name
	AsynchDownload* FindDownload(const String_256& strFileName);
	// Get a pointer to a download object in the queue knowing download handle
	AsynchDownload* FindDownload(DOWNLOAD_HANDLE hDownload);
	// Get a pointer to the next download in the queue and remove it from the queue 
	AsynchDownload* GetNextDownload();

protected:
	// The actual MFC container object 
	 CTypedPtrList<CPtrList, AsynchDownload*> m_List;
	 QueueType m_enType;
};


/********************************************************************************************

>	struct CacheEntry

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/06/97
	Purpose:	DownloadCache support structure. Encapsulates an entry into the
						cache file table

********************************************************************************************/

struct CacheEntry
{
	// <----DISCARD FIRST---------------------------------------------------------------------------------------------DISCARD LAST---------------->
	enum Priority {PRIORITY_OTHER = 0, PRIORITY_FONT_FILE,  PRIORITY_CLIPART_FILE,  PRIORITY_FILL_FILE,
		PRIORITY_CLIPART_THUMBNAIL, PRIORITY_FONT_THUMBNAIL, PRIORITY_FILL_THUMBNAIL, 
		PRIORITY_OLD_CATALOG_FILE, PRIORITY_CURRENT_CATALOG_FILE};
	struct _tstat m_FileStat;
	TCHAR szFilePath[_MAX_PATH];
	BOOL m_bIsValid;
	Priority m_nPriority;
	CacheEntry() {memset(szFilePath, 0x00, sizeof(szFilePath)); m_bIsValid = FALSE;}
	CacheEntry(const String_256& strPath); 
	inline BOOL IsValid() {return m_bIsValid;}
	inline INT32 Size() const {return m_FileStat.st_size;}
	inline INT32 GetPriority() const {return m_nPriority;}
	inline BOOL IsFolder() {return m_bIsValid ? m_FileStat.st_mode & _S_IFDIR : FALSE;}
	inline BOOL IsFile() {return m_bIsValid ? m_FileStat.st_mode &  _S_IFREG : FALSE;}
	BOOL operator !=(const CacheEntry& entry) { return _tcsicmp(szFilePath, entry.szFilePath);} 
};	

/********************************************************************************************

>	struct CacheRemovalAlgorithm

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/06/97
	Purpose:	DownloadCache support structure. Implements the cache removal
						algorithm. When the cache fills up, files with lower priority will
						be discarded first (see CacheEntry::Priority); for files of equal
						priority we compare the time stamps.

********************************************************************************************/

struct CacheRemovalAlgorithm : public STL::binary_function<CacheEntry, CacheEntry, BOOL> 
{
	inline BOOL operator()(const CacheEntry& first, const CacheEntry& second) const 
	{
		INT32 nFirstEntryPriority = first.GetPriority(), nSecondEntryPriority = second.GetPriority();
		if (nFirstEntryPriority > nSecondEntryPriority)
			return TRUE;
		else if (nFirstEntryPriority < nSecondEntryPriority)
			return FALSE;
		else // entries are of the same priority, use time stamps
			return first.m_FileStat.st_ctime >= second.m_FileStat.st_ctime ? TRUE : FALSE;
	}
};



/********************************************************************************************

>	DownloadCache

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/06/97
	Purpose:	Implements the file cache used by the InternetManager
	Comment: The interface is static so it can be accessed from anywhere
					in the program with the scope resolution operator ::	

********************************************************************************************/



class DownloadCache : public CCObject
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(DownloadCache);
public:
	// Set the size (in bytes) of the cache. By default the cache size is 5000 Kb
	static BOOL SetSize(UINT32 lSize);
	// Returns the cache size 
	inline static UINT32 GetSize() {return m_lMaxSize;}
	// Returns percentage of cache space actually used
	inline static INT32 GetUsage() {return m_lMaxSize ? 100 * m_lCurrentSize/m_lMaxSize : 0;}
	// Set the cache root directory (currently a subdirectory called /Cache in the app's directory)
	static BOOL SetPath(const String_256& strCachePath);
	// Empty the cache - note that the cache directory structure and catalog files WILL NOT be removed
	static void Flush();
	// Delete a file from the cache
	static INT32 RemoveFile(const TCHAR* szFilePath);
	// Rename a file in the cache
	static INT32 RenameFile(const TCHAR* szOldName, const TCHAR* szNewName);
	// Add a file to the cache
	static void InsertFile(const TCHAR* szFilePath);
	// Update cache information by scanning it - n.b. this can be SLOW
	static void Refresh();
	// Returns TRUE if the given path points to a file/folder in the cache
	static BOOL IsCachePath(const TCHAR* szPath);
	static class CacheMonitor
	{
	public:
		enum EVENTS {EVENT_DEACTIVATE = 0, EVENT_CHANGE};
		CacheMonitor();
		~CacheMonitor();
		BOOL Activate(String_256& strCachePath);
		BOOL Deactivate();
		inline void IgnoreEvents(INT32 nEvents) {m_nIgnoreCount += nEvents;}  
	private:
		static HANDLE rgEvents[2];
		static CRITICAL_SECTION m_CacheDataCriticalSection;
		static volatile INT32 m_nIgnoreCount;
		HANDLE m_hMonitorThread;
		unsigned m_dwMonitorThreadID;
		static void OnCacheEvent();
		static unsigned __stdcall Monitor(LPVOID lpParam);
		friend class DownloadCache;
	} m_CacheMonitor;

private:
	static UINT32 m_lMaxSize;
	static UINT32 m_lCurrentSize;
	static String_256 m_strCachePath;
	static INT32 m_nInstanceCount;
	static STL::priority_queue< CacheEntry, STL::vector<CacheEntry>, CacheRemovalAlgorithm> m_CacheData;
	static STL::priority_queue< CacheEntry, STL::vector<CacheEntry>, CacheRemovalAlgorithm> m_TemporaryCacheData;
	inline static void RemoveEntry(CacheEntry& entry)
	{
		while (!m_TemporaryCacheData.empty()) 
			m_TemporaryCacheData.pop();
		while (!m_CacheData.empty()) 
		{
			if (entry != m_CacheData.top()) 
				m_TemporaryCacheData.push(m_CacheData.top());
			m_CacheData.pop();
		}
		m_CacheData = m_TemporaryCacheData;
	}
#ifdef _DEBUG
	static void AssertCacheDataValid()
	{
		UINT32 lRealCacheSize = 0;
		while (!m_TemporaryCacheData.empty()) 
			m_TemporaryCacheData.pop();
		while (!m_CacheData.empty()) 
		{
			lRealCacheSize += m_CacheData.top().Size();
			m_TemporaryCacheData.push(m_CacheData.top());
			m_CacheData.pop();
		}
		m_CacheData = m_TemporaryCacheData;
		ERROR3IF(lRealCacheSize != m_lCurrentSize, "WARNING: Download cache data not valid");
	}
#endif
	static BOOL Traverse(const String_256& strPath, BOOL bFlush = FALSE);
	
	friend class InternetManager;
	friend class LibraryGallery;
	friend class LibClipartSGallery;
};









/********************************************************************************************

>	class InternetManager : public CCObject

	Author:		Adrian_Stoicar (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/12/96
	Purpose:	Webster connection/download manager.
					It is responsible for schedulling downloads for execution according to the priority declared by the client on registration,
					ensuring that no more than a preset number of connections are open at any one time (important for
					users with limited bandwidth), and otherwise keeping track of their fate.
					The sheduling sheme is as follows:
					- downloads with AsynchDownload::PRIORITY_NORMAL will be executed one at a time, as long as there are
						no high priority downloads queued up and the maximum number of connections is not exceeded. They all
						share the same progress dlg, which is hidden when there are no downloads scheduled
					- downloads with AsynchDownload::PRIORITY_HIGH will be executed simultaneously, up to the maximum no
						of connections. Each download gets its own progress dlg.

					In the present system thumbnail downloads should be registered with PRIORITY_NORMAL, clipart and catalog files 
					should use PRIORITY_HIGH. Maybe types should be used instead of priorities, things might be a bit clearer if TYPE_THUMB
					and TYPE_CLIPART - for instance - were used instead of priorities. However, this is supposed to be a general purpose class
					not limited to downloading clipart.
	Comment: The interface is static so it can be accessed from anywhere in the program with the scope resolution operator ::	

********************************************************************************************/

class InternetManager : public CCObject
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(InternetManager);

public:
	// Initialization/release
	static void Initialize();
	static void OnExitInstance();
	// Public interface
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Try to connect to the Internet. Returns FALSE if no connection could be started, in which case no Internet-related functions should be called 
	inline static BOOL AttemptConnection() { return (InternetAttemptConnect(NULL) == ERROR_SUCCESS);}
	// Called by a client object to register a download - returns a non-zero handle if successful, 0 otherwise
	static DOWNLOAD_HANDLE RegisterDownload(LPDOWNLOADINFO pDownloadInfo);
	// Unregister a previously registered download - called if the file is not required anymore, for instance if the client object
	// is about to be destroyed. If the download has already started it will be aborted and deleted as soon as possible - otherwise
	// it will simply be removed from its queue and deleted.
	static BOOL UnregisterDownload(DOWNLOAD_HANDLE hDownload);
	// Returns the state of a download based on its handle 
	static AsynchDownload::State GetDownloadState(DOWNLOAD_HANDLE handle);
	// Cancel all downloads and flush the queues
	static void CancelAllDownloads();
	// Stops downloading as soon as possible - downloads already in progress will be allowed to complete
	inline static void Suspend() {m_bIsSuspended = TRUE;}
	// Resumes downloading after a call to Suspend()
	static void Resume();
	// Return the percentage downloaded on a specific handle
	static INT32 GetPercentageDownloaded(DOWNLOAD_HANDLE hDownload);
	// Tests for suspended state
	inline static BOOL IsSuspended() {return m_bIsSuspended;}
	// Sets the type of connections we use
	static void SetConnectionType(ConnectionType type);
	// Gets the type of connections we use
	static ConnectionType GetConnectionType();
//	// Gets a SERVENT pointer to the currently set proxy server 
	static bool GetProxyServer(SERVENT* pProxyEntry, bool* pbProxyEnabled);
//	// Returns TRUE if there are any pending downloads
	inline static BOOL HasDownloadsPending() {return (BOOL) (AsynchDownload::m_nNormalPriorityInstanceCount + AsynchDownload::m_nHighPriorityInstanceCount);}
	
private:
	// Implementation
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	static DOWNLOAD_HANDLE m_NextHandle; // next free download handle
	static BOOL m_bIsSuspended;
	static INT32 rgConnections[4];
	static INT32 nMaxConnections; // maximum no. of symultaneous connections/downloads (default is 32)
	static CTypedPtrArray<CPtrArray, AsynchDownload*> m_CurrentDownloads; // array of currently executing downloads
	static String_256 strTempFileURL; // URL of  the temporary file used by the last download process
	static DownloadQueue m_NormalPriorityQueue; // normal queue
	static DownloadQueue m_HighPriorityQueue; // fast-track queue
	static AsynchDownload::State m_StateTable[MAX_HANDLE]; // table of download states - used to keep track of their progress
	// Callback function called when the object pointed to by pDownload has completed downloading
	static void OnDownloadComplete(AsynchDownload* pDownload);
	// Returns the number of current connections with the given priority
	static INT32 GetOpenConnections(AsynchDownload::Priority priority);
	// Returns a pointer to an AsynchDownload object given a local filename the object is downloading to
	static AsynchDownload* GetPendingDownload(const String_256& strFileName);
	// Returns a pointer to an AsynchDownload object given a download handle
	static AsynchDownload* GetPendingDownload(DOWNLOAD_HANDLE handle);
	static AsynchDownload* GetDownload(DOWNLOAD_HANDLE handle);
	// Returns the current normal priority download, if any
	static AsynchDownload* GetCurrentNormalPriorityDownload();
	friend class AsynchDownload;
	friend class AsynchDownload::AsynchBindStatusCallback;
	friend class DownloadQueue;
	static void SetState(DOWNLOAD_HANDLE hDownload, AsynchDownload::State state);

public:
	// preferences
	static UINT32 g_ConnectionType;
	static UINT32 g_CacheSize;
};

#endif



