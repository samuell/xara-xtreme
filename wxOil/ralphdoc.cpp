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

// implementation file for the Camelots COM RalphDoc

/*
*/
#include "camtypes.h"
#include "cversion.h"

#ifdef RALPH

#include "ralphdoc.h"
#include "docview.h"
#include "document.h"
#include "ralphvw.h"
#include "app.h"
#include "toollist.h"

#include "nativeop.h"
#include "zoomops.h"
#include "grndrgn.h"
//#include "mario.h"
#include "product.h"
#include "ccfile.h"
#include "filters.h"
#include "spread.h"
#include "grnddib.h"
#include "grndbmp.h"
#include "ralphprn.h"
#include "ccfile.h"
#include <iostream.h>
#include "filters.h"
#include "progress.h"
#include "camelot.h"
#include "ralphcri.h"


//#include "hotlink.h"  //For the default status bar string

DECLARE_SOURCE( "$Revision$" );


IMPLEMENT_DYNCREATE( RalphDocument, CCmdTarget )

CC_IMPLEMENT_DYNCREATE( LoadContext, CCObject)


DWORD LoadTimerStart =0;

#define new DEBUG_NEW

//--------------------------------------------------------------------------------------
// Statics

// Flag for forcing a palette or not. The quick viewer should set this
// to TRUE so it can force its own palette in the foreground. The plugins
// should set it to FALSE, since they're dependant on their host...
BOOL RalphDocument::ForegroundPalette = FALSE;

// the window to attach the rendering timer to
CWnd * RalphDocument::pTimerWnd = NULL;

CString CurrentStatusText="";
void* RalphDocument::RestoreStackLimit = NULL;

Document* RalphDocument::sm_pImportingDoc = NULL;
DWORD RalphDocument::sm_ImportThreadID = 0;
BOOL RalphDocument::InsideRenderLoop = FALSE;
	// Indicates if we are inside the render loop
BOOL RalphDocument::RenderTimerOn = FALSE;
	// Flag which indicates if the render timer is on/off
// create this event with manual  reset	unsignalled
HANDLE RalphDocument::g_hAbortDownLoadEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
// create this event with auto reset signalled
HANDLE RalphDocument::g_hSafeToEnterLoad = 	CreateEvent(NULL,FALSE,TRUE,NULL);
// an array of events for multiple waits
HANDLE RalphDocument::WakeEvents[2]={RalphDocument::g_hAbortDownLoadEvent,RalphDocument::g_hSafeToEnterLoad};
INT32 RalphDocument::RalphDocCnt = 0;


// Ralph's critical section stuff
CRITICAL_SECTION RalphCriticalSection::sm_Section;
BOOL RalphCriticalSection::sm_bInit = FALSE;

/********************************************************************************************

>	LoadContext::LoadContext()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Constructor for the LoadContext class

********************************************************************************************/

LoadContext::LoadContext()
{
	// Create the data available event
	hDataAvailableEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
	
	// a flag to avoid deadlock in the "network down" case
	AsynchDownLoadComplete = FALSE;
	
	// the total of bytes in the file that have not been read
	BytesAvailable = 0;

	// the total bytes in the file
	TotalBytes = 0;

}

/********************************************************************************************

>	LoadContext::~LoadContext()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Destructor for the LoadContext class

********************************************************************************************/


LoadContext::~LoadContext()
{
	// clean up the event
	if ( hDataAvailableEvent)
	{
		CloseHandle(hDataAvailableEvent );
		hDataAvailableEvent== NULL;
	}
}


/********************************************************************************************
>	void CALLBACK EXPORT TimerProca(HWND, UINT32, UINT32 id, DWORD systime)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/93
	Inputs:		From Windows
	Outputs:	-
	Returns:	-
	Purpose:	Timer procedure to handle timer events sent to MainFrm window.
				Asks Camelot to do background rendering. If there were no spare timers on
				startup, then the CCamApp::OnIdle function is used instead, which doesn't
				work when menus are down or dialogs are up, but is better than nothing.
	Errors:		-
	SeeAlso:	CCamApp::OnIdle;BackgroundRender
********************************************************************************************/

void CALLBACK EXPORT TimerProcA(HWND, UINT32, UINT32 id, DWORD)
{
	RalphDocument::InsideRenderLoop = TRUE; 
	if (id == CAM_TIMER_ID)
	{
		//TRACEUSER( "Simon", _T("D1 In TimerProcA\n")); 
		if (!(RalphDocument::sm_ImportThreadID))
		{
			Camelot.ServiceRendering(); 
		}
		else
		{
			static INT32 Count=ASYNC_RENDER_RATE;
			if (!Count)
			{
				Camelot.ServiceRendering(); // Do stuff
				Count =	ASYNC_RENDER_RATE;
			}
			Count--;
			
			// We are asynchronously loading so go slow
		}
	}
	RalphDocument::InsideRenderLoop = FALSE; 
}


/********************************************************************************************

>	LoadThreadProc(LPVOID pParam)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96
	Inputs:		pParam - a pointer to the ralph document we are about to load into
	Outputs:	-
	Returns:	-
	Purpose:	the asynchronous download thread proc !
	Returns:	0 for success...

********************************************************************************************/


UINT32 LoadThreadProc(LPVOID pParam)
{
	RalphDocument * pThisDoc = (RalphDocument*)pParam;

	if( pThisDoc == NULL || !pThisDoc->IsKindOf(RUNTIME_CLASS(RalphDocument)))
	{
		if(pThisDoc)
			pThisDoc->pLoadThread=NULL;
		return TRUE; //FALSE;	// illegal parameter
	}

	

//	TRACEUSER( "Chris", _T("***** Thread %x Waiting \n"), ::GetCurrentThreadId());

	//-------------------- ENTER CRITICAL  SECTION --------------------
	// we have a problem here, some of the filters (esp the older ones) are liberaly 
	// strewn with statics - about the only thing we can do at the moment is ensure 
	// that only one thread is let in at a time	
	// ** note ** that it is safe to let all blocked thread through in the abort case as
	// they simple bypass the load and clean up

	// wait for a "safe to enter" or an "abort" event
	DWORD we = WaitForMultipleObjects(2,RalphDocument::WakeEvents,FALSE,INFINITE);


	GDrawContext* GD = NULL;

	TRY
	{
		// which event let us through ?
		if(we == WAIT_OBJECT_0)
		{
			// we are aborting so don't bother with the load 
			// just skip past and clean up
//			TRACEUSER( "Chris", _T("***** Enter load from an ABORT EVENT\n"));
			pThisDoc->IsImportOK = FALSE;
		}
		else
		{
			// these globals will be used in set current etc.. if called from this thread
			RalphDocument::sm_pImportingDoc = pThisDoc->GetRalphDocument();
			RalphDocument::sm_ImportThreadID = ::GetCurrentThreadId();
						
			Document * pDocument = pThisDoc->GetRalphDocument();
			
			// Tell the Doc we are not importing the file
			pDocument->SetIsImporting(FALSE);

			// create a new g_draw context to avoid stack problems
			GD = new GDrawAsm(::GetCurrentThreadId());

			// Do the load.
			pThisDoc->IsImportOK = InvokeNativeFileOp(OPTOKEN_NATIVELOAD, pThisDoc->TempReadAsynchFile, FILTERID_GENERIC);
//			TRACEUSER( "Chris", _T("***** Load op Exit - %s\n",pThisDoc->IsImportOK ? "OK!!":"FAILED!!"));	
		}

		// tidy up - delete temp files etc
		//pThisDoc->CleanUpAfterAsynchDownLoad (pThisDoc->IsImportOK);
		SendMessage(pThisDoc->GetRalphView()->GetSafeHwnd(), WM_DOWNLOADCOMPLETE,pThisDoc->IsImportOK ,0);
		if(we != WAIT_OBJECT_0)
		{
			RalphDocument::sm_pImportingDoc = NULL;
			RalphDocument::sm_ImportThreadID = 0;
		}
	}

	CATCH( CFileException, e)
	{
		TRACEUSER( "Gerry", _T("Yikes CFileException in Load thread\n"));	
	}
	END_CATCH
	
	//  delete the g_draw context
	if (GD != NULL)
	{
		delete GD;
		GD = NULL;
	}

	//TRACEUSER( "Chris", _T("**** Thread %x leaving Load thread proc\n"), ::GetCurrentThreadId());

	//-------------------- LEAVE CRITICAL  SECTION --------------------
	// set the SafeToEnterLoad event to allow the next thread in
	TRACEUSER( "Simon", _T("D1 Leaving LoadThreadProc\n")); 
	SetEvent(RalphDocument::g_hSafeToEnterLoad);

	pThisDoc->pLoadThread = NULL;
	return 0; // for success
}

/********************************************************************************************

>	static void RalphDocument::OnRalphAboutToDie()


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	clean up static events etc..

********************************************************************************************/


void RalphDocument::OnRalphAboutToDie()
{

	// close the static events used for thread synchronisation
	if (g_hSafeToEnterLoad)
	{
		CloseHandle(g_hSafeToEnterLoad );
		g_hSafeToEnterLoad== NULL;
	}

	if (g_hAbortDownLoadEvent)
	{
		CloseHandle(g_hAbortDownLoadEvent );
		g_hAbortDownLoadEvent== NULL;
	}
	 
}

/********************************************************************************************

>	BOOL RalphDocument::StartLoadThread()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	start the asynchronous download 

********************************************************************************************/

BOOL RalphDocument::StartLoadThread()
{
	
	TRACEUSER( "Chris", _T("Start the load thread\n"));

	LoadTimerStart = GetTickCount();

	if(pDocument==NULL)
			return FALSE;
	if(pRalphView==NULL)
			return FALSE;
	
	// belt & braces job, this  - make sure we clear the abort condition
	Progress::ClearRalphAbortJob();
	
	// and make sure the abort event is unsignalled
	ResetEvent(g_hAbortDownLoadEvent);
	
	//********************** Create the temp file **********************
	
	BOOL ok = TRUE;
	
	// get a temp file name
	char PathBuffer [_MAX_PATH];  // Tmp directory
	char FileBuffer [_MAX_PATH];	
	
	// Get the path to the temp directory
	ok =GetTempPath(sizeof(PathBuffer), PathBuffer);
	
	// Get a unique filename (full path) 
	static UINT32 Unique = 0; 
	if (ok)	ok = GetTempFileName(PathBuffer, TEMP_FILE_PREFIX, ++Unique, FileBuffer);
	if(!ok)
		return FALSE;

	PathString = FileBuffer;

	// string to hold the filename
	CString URLString ;
	CString ExtString;
	// the full URL
	URLString = GetDocumentURL();
		
	// find the last '.' in the string
	INT32 iPointerToExtension = URLString.ReverseFind('.');
	
	// extract the filename
	if (iPointerToExtension!=-1)
	{
		// chars required = total length - index of last '.' 
		INT32 nCharsToExtract = URLString.GetLength()-iPointerToExtension;
		ExtString=URLString.Right(nCharsToExtract);

		// find the last '.' in the string
		INT32 iPointerToPathExt = PathString.ReverseFind('.');
		CString LeftString;
		if(iPointerToPathExt!=-1)
		{
			LeftString = PathString.Left(iPointerToPathExt);
			LeftString +=ExtString;
		}
		PathString = LeftString;
  	}
	 	
	// create the write file
	TempWriteAsynchFile = new CCAsynchDiskFile();


	if(! TempWriteAsynchFile)
		return FALSE;
	
	// attach a down load context to allow thread synchronisation
	TempWriteAsynchFile->SetDownloadContext(&TheLoadContext);

	// open the asynch file for the download
	if(! TempWriteAsynchFile->open(PathName((LPCSTR)PathString), ios::out| ios::binary ))
		return FALSE;

	// create the read file
	TempReadAsynchFile = new CCAsynchDiskFile(PathName((LPCSTR)PathString),
												ios::in| ios::binary );

	TempReadAsynchFile->SetDownloadContext(&TheLoadContext);

	if(! TempReadAsynchFile)
		return FALSE;
	

   	// Actually do an Import of The Camelot Native EPS format
	IsImportOK = TRUE;

	// will test whether a load error occured
	LoadErrorHasOccured =	FALSE;

   	// Scary multi-threading starts here 

	//******************** start the load thread **********************

	pLoadThread = AfxBeginThread(LoadThreadProc,this,THREAD_PRIORITY_NORMAL);

	return TRUE;
}

/********************************************************************************************

>	STDMETHODIMP RalphDocument::OnDataAvailable (unsigned char* pReadBuffer,DWORD BytesRead)
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96
	Inputs:		pReadBuffer - char buffer containing new data
				BytesRead - size of buffer
	Outputs:	-
	Returns:	-
	Purpose:	write the data to the asynch download file 
				this is thread safe at the streambuf level 
********************************************************************************************/


BOOL RalphDocument::OnDataAvailable (unsigned char* pReadBuffer,DWORD BytesRead)
{
	if(!IsImportOK)
		return TRUE;

	// TRACEUSER( "Chris", _T("---- >> %d Bytes now\n"),BytesRead);
	// well !!! this shouldn't happen
	if(pLoadThread==NULL)
		return TRUE;
	// check we've got some data
	if(pReadBuffer==NULL||BytesRead==0)
		return TRUE;

	ASSERT(TempWriteAsynchFile);
	
	// write it to the file
	if(TempWriteAsynchFile)
	{
		TempWriteAsynchFile->write(pReadBuffer,BytesRead);
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL RalphDocument::CleanUpAfterAsynchDownLoad (BOOL LoadOK)
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96
	Input:		LoadOK - load success state			
	Purpose:	set up doc states etc.
				this function is called by the ralphview on receipt of a WM_DOWNLOADCOMPLETE
				message - this is posted by the load thread before it exits - the idea is to 
				keep access to ralph windows within the same thread.
********************************************************************************************/

BOOL RalphDocument::CleanUpAfterAsynchDownLoad (BOOL LoadOK)
{
							 
	if(pDocument==NULL)
		return FALSE;
	if(pRalphView==NULL)
		return FALSE;



	TRACEUSER( "Gerry", _T("Clean up load %s\n",LoadOK?"OK":"Failed"));

	// close the file and delete the file object
	if(TempWriteAsynchFile)
	{
		TempWriteAsynchFile->close();
		delete TempWriteAsynchFile;
		TempWriteAsynchFile = NULL;
	}

	if(TempReadAsynchFile)
	{
		
		TempReadAsynchFile->close();
		delete TempReadAsynchFile;
		TempReadAsynchFile = NULL;
		
		// actually delete the file fromn the disk
		BOOL DeletedOk = DeleteFile((LPCSTR)PathString);

		ENSURE(DeletedOk,"Couldn't delete temp asynch download file");
	}

	if(LoadOK)
	{
		// Set the flag back to its default value
		if (pDocument!=NULL)
			pDocument->SetIsImporting(TRUE);

		// pDocument->SetCurrent();
		if(!pRalphView->IsSafeToDraw())
		{
			pRalphView->SetSafeToDraw(TRUE);
			// Setup the initial view mode
			SelectViewMode(Viewmode);
		}
	
		// Ok All nodes in the document should be safe now, turn off the SafeRender system
		pDocument->GetSafeRenderPointer().SetPointerInValid();
	
	}
	else
	{
		// force a redraw so we can display an error 
		pRalphView->Invalidate();	
	}

	// Kill the previous zoom by setting an invalid index
	DocView* pDocView = NULL;
	pDocView = pRalphView->GetDocViewPtr();

	if(pDocView)
		pDocView->SetPrevZoomIndex(cUninitZoomIndex);

/*	char buf[64];
	sprintf(buf,"Load took..  %f secs",((double)(GetTickCount()-LoadTimerStart)) /1000);
	MessageBox(NULL,buf,"Timer",MB_OK);
  */
 
	// Ok All nodes in the document should be safe now, turn off the SafeRender system


	//Beep(2000,15);

	return TRUE;
}



/********************************************************************************************

>	BOOL RalphDocument::EndAsynchDownLoad(HRESULT Status)
 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96
	Input:		Status - HRESULT indicating success or abort			
	Purpose:	end the asynchronous download 

********************************************************************************************/

BOOL RalphDocument::EndAsynchDownLoad(HRESULT Status)
{
	
	if(!IsImportOK)
		return TRUE;

	TRACEUSER( "Chris", _T("End Download status %d\n"),Status);

	if(pDocument==NULL)
		return FALSE;
	if(pRalphView==NULL)
		return FALSE;
	if(pLoadThread==NULL)
		return FALSE;

	TRACEUSER( "Chris", _T("***** End download thread %x\n"),pLoadThread->m_nThreadID);
 
	
	// Flag the file in the queue that its stream is ended
	if(TempWriteAsynchFile )
		TempWriteAsynchFile->DownloadHasEnded(Status);

	// AfxDebugBreak();
	
 
	// something has gone horribly wrong or the user has aborted
	if(Status != 0)
	{
		TRACEUSER( "Chris", _T("***** Abort the load now !!!\n"));

		// this event will wake 
		// a} any thread waiting for data
		// b} open the floodgates in LoadThreadProc
		SetEvent(g_hAbortDownLoadEvent);
		
		// fake an escape condition in the progress system - 
		// this should abort the load nicely
		Progress::RalphAbortJob();
	
		// wait for the load thread to exit before we return
		// the first attempt to block this thread using WaitForSingleObject()
		// failed as the load thread requires communication with this thread
		// using windows messages... this is bad 'cause WaitForSingleObject
		// blocks our message loop - deadlock.

		//********************** the message loop **************************
		// this message loop is based on the one in CWinThread::Run()
		// but I have removed the idle stuff -we don't need any UI updates
		// etc.. so I reckon the simpler the better
		
		// for tracking the idle time state
		MSG  m_msgCur;
		// acquire and dispatch messages until a WM_QUIT message is received.
		BOOL bDoingBackgroundProcessing =TRUE;

		while (bDoingBackgroundProcessing && pLoadThread) 
		{ 
			// phase2: pump messages while available
			do
			{
			   	// pump message, but break on WM_QUIT
				if (!AfxGetApp()->PumpMessage())
					bDoingBackgroundProcessing = FALSE; 
			} while (::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) && pLoadThread);
		}

		TRACEUSER( "Gerry", _T("thread's dead\n"));

		// make sure we clear the abort condition
		Progress::ClearRalphAbortJob();
		
		return FALSE;
	}

	

	TRACEUSER( "Chris", _T("Normal end download \n"));

	return TRUE;
}






/********************************************************************************************

>	RalphDocument::RalphDocument(HWND ParentWnd) 

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Constructor for the ralphdoc class

********************************************************************************************/

RalphDocument::RalphDocument()
{
	pDocument			=	NULL;
	pRalphView			=	NULL;
	
	// default view mode
	Viewmode			=	DOCUMENT_VIEW;
	Dither				=	ORDERED_DITHER;
	Toolmode			=	ZOOM_TOOL;
	Quality				=	ANTI_ALIASED;
	DocumentViewInit	=	FALSE;
	InsideRalphMethod	=	FALSE;
	pIErrorCallback		=	NULL;
	pIEventCallback		=	NULL;
	TempWriteAsynchFile	=	NULL;
	TempReadAsynchFile	=	NULL;
	pLoadThread			=	NULL;
	IsImportOK			=	TRUE;
	LoadErrorHasOccured =	FALSE;
	CurrentStatusText = "";

	
	if(RalphDocCnt==0)
	{
		//::SetTimer(RalphDocument::pTimerWnd->GetSafeHwnd(),CAM_TIMER_ID, CAM_TIMER_FREQUENCY,TimerProcA);
		SetRenderTimer(TRUE);
	}
	RalphDocCnt++;

	AfxOleLockApp();
}

/********************************************************************************************

>	RalphDocument::~RalphDocument() 

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Destructor for the ralphdoc class

********************************************************************************************/

RalphDocument::~RalphDocument()
{
//	AfxDebugBreak();

	TRACEUSER( "Chris", _T("***** Ralph is dead %x loadthread %x\n"), ::GetCurrentThreadId(),sm_ImportThreadID);

	Progress::ClearRalphAbortJob();
	Error::ClearError();

	if(pDocument != NULL)
		pDocument->SetCurrent();

	// Nuke the view associated with this document
	if(pRalphView != NULL)
	{
		pRalphView->DestroyWindow();
		delete pRalphView;
		pRalphView = NULL;
	}

	// Nuke the document associated with this document
	if(pDocument != NULL)
	{
		delete pDocument;
		pDocument = NULL;
	}

	if(pIErrorCallback)
	{
		//pIErrorCallback->Release();
		pIErrorCallback = NULL;
	}

	if (pIEventCallback)
	{
		pIEventCallback = NULL; 
	}

	RalphDocCnt--;
	if (!RalphDocCnt)
	{
		SetRenderTimer(FALSE);	 // Stop the render timer
	}

	AfxOleUnlockApp();
}


// Vape the ralphdoc
void RalphDocument::OnFinalRelease()
{
	TRACEUSER( "Richard", _T("void RalphDocument::OnFinalRelease()\n"));
	delete this;
}

/********************************************************************************************

>	BOOL RalphDocument::SelectDitherType(DitherType Dither)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96
	Input		Dither - Ordered / Diffused			
	Purpose:	Set the Dither type and force a redraw

********************************************************************************************/

BOOL RalphDocument::SelectDitherType(DitherType Dither)
{
	if(!pRalphView)
		return FALSE;

	// translate to GRender Dither values
	switch(Dither)
	{
	case ORDERED_DITHER:
		if(RalphDocument::ForegroundPalette)
			GRenderRegion::SetViewDither(2);	// Enhanced greyscale dithering
		else
			GRenderRegion::SetViewDither(1);	// Normal netscapey palette thing
		break;
	case ERROR_DIFFUSE_DITHER:
		GRenderRegion::SetViewDither(3);
		break;
	case NO_DITHER:
		GRenderRegion::SetViewDither(4);
		break;
	}

	// and force a redraw of the view
	DocView* pDocView = NULL;
	pDocView = pRalphView->GetDocViewPtr();

	if(pDocView==NULL)
		return FALSE;

	pDocView->ForceRedraw();
	return TRUE;
}

/********************************************************************************************

>	BOOL RalphDocument::SelectTool(ToolMode toolmode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96
	Input		Toolmode - enum defining the tool to switch to			
	Purpose:	Switch into the given tool 

********************************************************************************************/

BOOL RalphDocument::SelectTool(ToolMode toolmode)
{

	UINT32 ToolId =0;
	// translate to camelot tool id's
	switch(toolmode)
	{
	case PUSH_TOOL:
		ToolId = TOOLID_PUSH;
		break;
	case ZOOM_TOOL :
		ToolId = TOOLID_ZOOM;
		break;
	
	// unknown or NO_TOOL
	case NO_TOOL:
	default:
	   ToolId=0;

	}
	
	if (Tool::GetCurrentID() == ToolId)
		return TRUE; // Already the selected tool

	// go into neutral
	if(ToolId == 0)
	{
		Tool::SetNoCurrent();
	}
	// set the tool current
	else
	{
		ToolListItem * pItem = NULL;
		pItem = Tool::Find(ToolId);
		if(pItem == NULL)
			return FALSE;
		
		pItem->pTool->SetCurrent();
	}


	return TRUE;
}

/********************************************************************************************

>	BOOL RalphDocument::SelectViewQuality(INT32 ViewQuality)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96
	Input		ViewQuality	value for new view quality			
	Purpose:	Change the render quality for this document 

********************************************************************************************/

BOOL RalphDocument::SelectViewQuality(INT32 ViewQuality)
{
	if(pRalphView==NULL)
		return FALSE;
	return  pRalphView->SetViewQuality(ViewQuality);
}


/********************************************************************************************

>	BOOL RalphDocument::SelectPaletteMode(BOOL Foreground)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96				
	Purpose:	Change the palette realising mode

********************************************************************************************/

BOOL RalphDocument::SelectPaletteMode(BOOL Foreground)
{
	RalphDocument::ForegroundPalette = Foreground;

/*	if(pRalphView==NULL)
		// Redraw ?
*/
	return TRUE;
}

/********************************************************************************************

>	BOOL RalphDocument::RestoreZoomAndScroll)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Restore the zoom and scroll offsets 
********************************************************************************************/

BOOL RalphDocument::RestoreZoomAndScroll()
{
	DocView* pDocView = NULL;
  	
	if(pRalphView==NULL)
		return FALSE;
	
	pDocView = pRalphView->GetDocViewPtr();
	
	if(pDocView==NULL)
		return FALSE;

	// Restore the saved scale factor and scroll offsets.
	pDocView->SetViewScale(LastViewScale);
	
	pDocView->SetScrollOffsets(LastScrollOffset, FALSE);

	pDocView->SetZoomTableIndex(LastZoomIndex);

	pDocView->ForceRedraw(TRUE);


	return TRUE;
}

/********************************************************************************************

>	BOOL RalphDocument::SaveZoomAndScroll)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Save the zoom and scroll offsets so that we can restore them after 
				view mode changes 
********************************************************************************************/

BOOL RalphDocument::SaveZoomAndScroll()
{
	DocView* pDocView = NULL;
	
	if(pRalphView ==NULL)
		return FALSE;
	
	pDocView = pRalphView->GetDocViewPtr();
	
	if(pDocView==NULL)
		return FALSE;
	
	// Record the current scale and scroll offset.
	LastZoomIndex = pDocView->GetZoomTableIndex();
	LastViewScale = pDocView->GetViewScale();
	LastScrollOffset = pDocView->GetScrollOffsets();
	return TRUE;
}

/********************************************************************************************

>	BOOL RalphDocument::ZoomToFullScale()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Set up initial Scale and scroll offsets for document view 

********************************************************************************************/

BOOL RalphDocument::ZoomToFullScale()
{
	if(pRalphView==NULL)
		return FALSE;
	
	if(pDocument==NULL)
		return FALSE;
	
	// This has been removed by Gerry (again 9/9/96)
	// It actually causes an unecessary redraw so don't do it
//	pRalphView->LockWindowUpdate();

	DocView* pDocView = NULL;
	pDocView = pRalphView->GetDocViewPtr();

	if(pDocView)
	{
		// make sure we save the last zoom
		OpZoomPrevZoomDescriptor::SaveZoom(pDocView);

		// 100 %
		pDocView->SetViewScale(FIXED16(1.0));
		// force OnNewView by setting 0 scroll offsets
		pDocView->SetScrollOffsets(WorkCoord(0,0), FALSE);
		// position the window in the correct place
		pDocView->OnNewView();

		pDocView->ForceRedraw(TRUE);
	}

	// all done... let the redraw begin.
//	pRalphView->UnlockWindowUpdate();

	return TRUE;
}


/********************************************************************************************

>	BOOL RalphDocument::SetDocumentViewMode()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:1	Set up initial Scale and scroll offsets for document view 

********************************************************************************************/

BOOL RalphDocument::SetDocumentViewMode()
{
	
	if(pRalphView==NULL)
		return FALSE;
	
	if(pDocument==NULL)
		return FALSE;

	// if we are going into Document view for the first time we  
	// initialise the view at 100% with the page corner at top left	of view
	if(!DocumentViewInit)
	{
		
		DocView* pDocView = NULL;
		pDocView = pRalphView->GetDocViewPtr();
		
		if(pDocView)
		{
			 OpZoomDescriptor::FakeInvoke(OPTOKEN_ZOOMSPREAD);

	  }
		DocumentViewInit = TRUE;
		// save the zoom factor and scroll offset 
		
		return SaveZoomAndScroll();
		
 	 }
	else
	{
		// restore last zoom factor and scroll offsets
		return RestoreZoomAndScroll();
	}
	return TRUE;
}

/********************************************************************************************
>	BOOL RalphDocument::SelectViewMode(ViewMode Vmode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Change the view mode for this document 

********************************************************************************************/

BOOL RalphDocument::SelectViewMode(ViewMode Vmode)
{
	
	if(pRalphView==NULL)
		return FALSE;
	
	if(pDocument==NULL)
		return FALSE;

	
	// set the doc view state
	Viewmode = Vmode;

	// let the kernel know
	pDocument->SetRalphViewMode(Viewmode);
	
	switch (Vmode)
	{
	case DOCUMENT_VIEW: 
		pRalphView->ShowScrollers(TRUE);
		SetDocumentViewMode();
		break;
	case PAGE_VIEW:
		pRalphView->ShowScrollers(FALSE);
		// save the zoom factor and scroll offset 
		SaveZoomAndScroll();
		break;
	case DRAWING_VIEW:
		pRalphView->ShowScrollers(FALSE);
		// zoom to drawing 
		OpZoomDescriptor::FakeInvoke(OPTOKEN_ZOOMDRAWING);
		break;
	}
	

	DocView * pDocView = NULL;

	// get a pointer to the kernel docview
  	pDocView = pRalphView->GetDocViewPtr();
	
	if(pDocView == NULL)
		return FALSE;
	
	// Kill the previous zoom by setting an invalid index
	pDocView->SetPrevZoomIndex(cUninitZoomIndex);
	
	return TRUE;
}

/********************************************************************************************

> void RalphDocument::SetSelected()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> & Simon
	Created:	08/08/96				
	Purpose:	Make this RalphDocument selected. All Tool operations will act on this document.
********************************************************************************************/

void RalphDocument::SetSelected()
{
	// Make this the selected view
	DocView* pDocVw = pRalphView->GetDocViewPtr();
	ASSERT(pDocVw);
	if (pDocVw)
	{
		if (DocView::GetSelected() != pDocVw)
		{
			Document::SetSelectedViewAndSpread(pDocVw->GetDoc(),
											   pDocVw,
											   NULL);
		}
	}
	SelectTool(Toolmode);
}

/********************************************************************************************

> BOOL RalphDocument::PassErrorToHarness(HRESULT ErrCode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	Pass any error codes back to the harness (Netscape Pluggin etc..)
				so that they can inform the user
********************************************************************************************/

BOOL RalphDocument::PassErrorToHarness(HRESULT ErrCode)
{
	// we won't report the error during the load
	// we'll simply flag it and report it after
	if(pLoadThread)
	{
		LoadErrorHasOccured = TRUE;
	}
	else
	{

		if(!pIErrorCallback)
			return FALSE;
		else
			pIErrorCallback->InformError(ErrCode);
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL RalphDocument::PrintRalphView(HDC hDC, DWORD X, DWORD Y, DWORD Width, DWORD Height, DWORD DPI)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> (in a rumbling state)
	Created:	23/08/96
	Inputs:		hDC		- Handle of device context to which we want to print (metafile dc, etc)
				X, Y, Width, Height - DC co-ords within which we should render
				DPI		- DPI of bitmap to construct for the DC
	Returns:	TRUE if no serious problems... In fact, TRUE all the time...
	Purpose:	Print the current view (in its current state) to the given DC.
				At the moment we only support bitmap printing via GRenderRalphPrint.			
********************************************************************************************/

BOOL RalphDocument::PrintRalphView(HDC hDC, DWORD X, DWORD Y, DWORD Width, DWORD Height, DWORD DPI)
{
	// Create a grenderregion, attach it to the given dc and render the view into it
	if(pDocument != NULL && pRalphView != NULL)
	{
		Spread *pSpread = pDocument->FindFirstSpread();
		DocView* pDocVw = pRalphView->GetDocViewPtr();

		// Force fore-ground redraw mode
		BOOL OldBackgroundRedrawState = pDocVw->GetForeBackMode();
		pDocVw->SetForeBackMode(FALSE);

		// Get the view's clip rect in Spread Coords
		DocRect ClipRect = pDocVw->GetDocViewRect(pSpread);
		pSpread->DocCoordToSpreadCoord(&ClipRect);

		// Get the current view's rendering matrix and view scale
		Matrix ConvMatrix = pDocVw->ConstructRenderingMatrix(pSpread);
		FIXED16 ViewScale = pDocVw->GetViewScale();

		// Construct a Ralph Print render region which is basically a maligned GRenderPrint (GRenderDIB)
		GRenderRalphPrint *pPrintRR = new GRenderRalphPrint(ClipRect, ConvMatrix, ViewScale, 32, (double)DPI);

		if(pPrintRR != NULL)
		{		
			CDC *pDC = CDC::FromHandle(hDC);
			pPrintRR->SetDCRect(X, Y, X+Width, Y+Height);

			// Attach our DC to the view and render region...
			if(pDC != NULL && pPrintRR->AttachDevice(pDocVw, pDC, pSpread))
			{
				// Render the current view to the constructed render region...
				pDocVw->ContinueRenderView(pPrintRR, pSpread);
			}
		}

		// Restore fore-ground redraw mode
		pDocVw->SetForeBackMode(OldBackgroundRedrawState);
	}

	return TRUE;
}


/********************************************************************************************

> BOOL RalphDocument::WebAddressSelected(LPCWSTR WebAddress)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96				
	Purpose:	Call this function to inform an interested harness that a Web address has
				been selected. 
********************************************************************************************/

void RalphDocument::WebAddressSelected(String_256* InAddress)
{
	CString WebAddress=*InAddress;

	// If the interface is registered then inform the harness of the event
	if(pIEventCallback)
		pIEventCallback->WebAddressSelected(WebAddress);
}

/********************************************************************************************

> void RalphDocument::SetStatusBar(String_256* pNewText)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/96				
	Purpose:	Sets the status bar of the browser to the new text
	See Also:	DocView::OnMouseMove; DocView::OnMouseMoveWithNoTool
********************************************************************************************/

void RalphDocument::SetStatusBar(String_256* pNewText)
{
	CString TextToPass=*pNewText;

	//Only change the status bar text if the new text is different
	//from the old text.
	// If the interface is registered then inform the harness of the event
	if(pIEventCallback)
		pIEventCallback->SetStatusBar(TextToPass);

	CurrentStatusText=TextToPass;

}

/********************************************************************************************

> void RalphDocument::SetStatusBar(CString NewText)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/96				
	Purpose:	Sets the status bar of the browser to the new text
********************************************************************************************/

void RalphDocument::SetStatusBar(CString NewText)
{
	//Only change the status bar text if the new text is different
	//from the old text.
	if(NewText!=CurrentStatusText)
	{
		// If the interface is registered then inform the harness of the event
		if(pIEventCallback)
			pIEventCallback->SetStatusBar(NewText);

		CurrentStatusText=NewText;
	}
}

/********************************************************************************************

> void RalphDocument::SetStatusBar()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/96				
	Purpose:	Sets the status bar of the browser to a default
				message.
********************************************************************************************/

void RalphDocument::SetStatusBar()
{
	String_256 DefaultText(_R(IDS_DEFAULTBROWSERSTATUSSTR));

	CString TextToPass(DefaultText);

	//Only change the status bar text if the new text is different
	//from the old text.
//	if(TextToPass!=CurrentStatusText)
//	{
		// If the interface is registered then inform the harness of the event
		if(pIEventCallback)
		{
			pIEventCallback->SetStatusBar(TextToPass);
			CurrentStatusText=TextToPass;
		}
//		else
//		{
//			CurrentStatusText="";
//		}

//	}
}

/********************************************************************************************

> String_256 RalphDocument::GetDocumentURL()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/96				
	Returns:	DocumentURL
	Purpose:	Gets the URL of this Ralph document.
********************************************************************************************/

String_256 RalphDocument::GetDocumentURL()
{
	String_256 ReturnText(DocumentURL);

	return ReturnText;
}

/********************************************************************************************

> String_256 RalphDocument::GetDocumentURL()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/96				
	Returns:	The base URL of this Ralph document
	Purpose:	Gets the base URL of this Ralph document. The base URL is the path...
				that is, the entire string used to access file minus the filename on the end.

				Ideally this function would return the value of the HTML variable "Base" for
				Camelot documents embedded within HTML. But it doesn't.
********************************************************************************************/

String_256 RalphDocument::GetBaseURL()
{
/*	CString ParseString;

	//To get the base URL, we're going to parse the document's URL.
	//First we need to get the characters we're going to search for
	//from the document's resources.

//	CString sPathSeparator;
	//tring sColon;
	//CString sParaSeparator;
	//CString sFragment;
	//CString sQuery;

	String_256 s256PathSeperator(_R(IDS_URL_PATH_SEPARATOR));
	CString sPathSeparator(s256PathSeperator);

	String_256 s256ParaSeparator(_R(IDS_URL_PARAMETER_SEPARATOR));
	CString sParaSeparator(s256ParaSeparator);

	//sPathSeparator.LoadString(_R(IDS_URL_PATH_SEPARATOR));
	//sColon.LoadString(_R(IDS_URL_COLON));
	//sParaSeparator.LoadString(_R(IDS_URL_PARAMETER_SEPARATOR));
	//sFragment.LoadString(_R(IDS_URL_FRAGMENT));
	//sQuery.LoadString(_R(IDS_URL_QUERY));

	char cPathSeparator=sPathSeparator[0];
	char cParaSeparator=sParaSeparator[0];

	//Now we can do some parsing with those characters
	//The parsing rules are usually  unpleasant, and are contained in
	//http://ds.internic.net/rfc/rfc1808.txt. But in our case it's easy.

	//The form of a URL should always be:
	//  <scheme>://<net_loc>/<path>;<params>?<query>#<fragment>

	//And we only want:
	// <scheme>://<net_loc>/<path>

	
	
	//So find the first semicolon in the string

	ParseString=DocumentURL;

	INT32 iEndPointer=ParseString.Find(cParaSeparator);

	//If we found a semicolon, take everything from that semicolon onwards
	//out of the string
	if (iEndPointer!=-1)
	{
		ParseString=ParseString.Left(iEndPointer);
	}

	//Now find the last / in the string

	iEndPointer=ParseString.ReverseFind(cPathSeparator);

	//Did we find a / ?
	if (iEndPointer)
	{
		//Yes. So the string to return is everything up to that slash.
		ParseString=ParseString.Left(iEndPointer+1);
	}
	else
	{
		//No. So there isn't a base URL. Return a blank string.
		ParseString.Empty();
	}

	//And create a string to return
	//String_256 ReturnString(ParseString);*/
		
	return DocumentURL;
}

/********************************************************************************************

>	void RalphDocument::SetRenderTimer(BOOL State)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/10/96				
	Inputs:		State: When TRUE the render timer is started. When FALSE the render timer
					   is stopped. 
	Purpose:	Starts/Stops the render timer. 
********************************************************************************************/

void RalphDocument::SetRenderTimer(BOOL StartTimer)
{
	BOOL ok = TRUE;
		
	if (RenderTimerOn == StartTimer)
	{
		ASSERT(FALSE); 
		return; 
	}
	if (StartTimer)
	{
		// If we don't have a timer window then we must remember to create it 
		if(pTimerWnd==NULL)
		{
			pTimerWnd= new CWnd();
			BOOL WindowCreated =FALSE;

			if(pTimerWnd)
			{
				WindowCreated = pTimerWnd->CreateEx(0,AfxRegisterWndClass(
									0,
									0,
									0,
									0),

						NULL, NULL,
				 		0,0,8,8, 
				 		NULL,   
				 		NULL);
			
				//pTimerWnd->SetTimer(CAM_TIMER_ID, CAM_TIMER_FREQUENCY,TimerProcA);

			}
			ENSURE(WindowCreated,"Failed to create the timer window");
		// Now start the heart
		}
		ok = ::SetTimer(RalphDocument::pTimerWnd->GetSafeHwnd(),CAM_TIMER_ID, CAM_TIMER_FREQUENCY,TimerProcA);
		ASSERT(ok);
	}
	else
	{
		ok = ::KillTimer(RalphDocument::pTimerWnd->GetSafeHwnd(),CAM_TIMER_ID);
#ifdef _DEBUG
		ASSERT(ok); 
#endif
		if (pTimerWnd)
		{
			pTimerWnd->DestroyWindow();		
			delete pTimerWnd;
			pTimerWnd = NULL;
		}
  	}
	RenderTimerOn = StartTimer;
}

//*******************************************************************************************
//								** PLEASE READ **
//	Ralph Document Interface notes 
//	All interface methods MUST start with 
//	   METHOD_PROLOGUE(containing class, interface class) this macro creates pThis
//	   START_RALPH_METHOD() - sets a flag used by the error reportiong system
//	These methods should NEVER return normally !! they MUST use  
//	   RALPH_RETURN() 
//	as this clears the flag set in the START_RALPH_METHOD 
//	NB this does not apply to the standard AddRef, Release and QueryInterface methods
//  TO test for any errors that may have occured -during an Op for instance use
//		GET_RALPH_ERROR()
//  This tests whether a serious ERROR1 has occured and if so clears the InMethod flag
//  and returns the error (mapped to an HRESULT in Error::GetRalphError() 
//*******************************************************************************************
 

// RalphDoc's Interface Map
BEGIN_INTERFACE_MAP(RalphDocument, CCmdTarget)
	INTERFACE_PART(RalphDocument, IID_ICreation, Creation)
	INTERFACE_PART(RalphDocument, IID_ICommandControl,CommandControl)
	INTERFACE_PART(RalphDocument, IID_IViewControl,ViewControl)
	INTERFACE_PART(RalphDocument, IID_IInitRalph,InitRalph)
	INTERFACE_PART(RalphDocument, IID_IAsynchDownLoad,AsynchDownLoad)
END_INTERFACE_MAP()

// We must call this. See Help for IMPLEMENT_OLECREATE
// This is RalphDoc's CLASS_ID
// 27682680-d299-11cf-ab39-0020af71e433
IMPLEMENT_OLECREATE(RalphDocument, "RalphDocument", 
		0x27682680, 0xd299, 0x11cf, 0xab, 0x39, 0x0, 0x20, 0xaf, 0x71, 0xe4, 0x33);


/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCreation::GetVerNum(TCHAR* pVerNum)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Creates a View for the RalphDocument. 
			
********************************************************************************************/
STDMETHODIMP RalphDocument::XCreation::GetVerNum(TCHAR * pVerNum) 

{
    METHOD_PROLOGUE(RalphDocument, Creation)
	START_RALPH_METHOD()
	// ouch..
	if(pVerNum == NULL)
		RALPH_RETURN(E_FAIL);
	// get the ver num and date	
	TCHAR VerText[] = TEXT(PRODUCT_VERSION_NO "  " __DATE__ );
	// the build number
	TCHAR Temp[32];
	wsprintf(Temp," (%d)", CAMELOT_VERSION_MINOR);
	// add them
	CString FullText(VerText);
	FullText +=Temp;
	// and return
	::lstrcpy(pVerNum,FullText);
	// fine..
	RALPH_RETURN (S_OK);

}

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCreation::CreateViewWindow(HWND ParentWnd	
																	ViewMode Vmode,
																	BOOL ShowScrollers,
																	BOOL ShowWindow,
																	BOOL HasBorder,
																	HWND * ViewWindow) 



	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Creates a View for the RalphDocument. 
	Errors:		S_OK				- success
				RALPH_E_OUTOFMEMORY -   
				RALPH_E_BAD_FILE	- load failed
				RALPH_E_LOADFAILED	- load failed
				RALPH_E_UNEXPECTED	- catch all error  
				
********************************************************************************************/
STDMETHODIMP RalphDocument::XCreation::CreateViewWindow(HWND ParentWnd,
																	ViewMode Vmode,
																	BOOL ShowScrollers,
																	BOOL ShowWindow,
																	BOOL HasBorder,
																	HWND * ViewWindow) 

{
    METHOD_PROLOGUE(RalphDocument, Creation)
	START_RALPH_METHOD()

	Progress::ClearRalphAbortJob();
	Error::ClearError();

	Progress::ClearRalphAbortJob();
	Error::ClearError();


	BOOL ok =TRUE;
	
	if(ParentWnd==0)
		RALPH_RETURN (RALPH_E_UNEXPECTED);

	// set this for LoadDocument
	pThis->Viewmode =Vmode;		
	
	// create a document and initialise 
	pThis->pDocument = new Document(NULL);

		
	if(pThis->pDocument==NULL)
		RALPH_RETURN (RALPH_E_OUTOFMEMORY);

	// set a horrible BOOL so that we know were a RALPH Doc
	pThis->pDocument->SetRalphDoc(TRUE);
	pThis->pDocument->ConnectRalphDocument(pThis);

	// Try to initialise the kernel Document
	if (!pThis->pDocument->Init(NULL))
	{
		// Didn't work - delete the Document object and throw an exception
		delete pThis->pDocument;
		RALPH_RETURN (RALPH_E_OUTOFMEMORY);
	}

	// create a test window
	pThis->pRalphView = new RalphView(pThis);
   	if(pThis->pRalphView==NULL)
		RALPH_RETURN (RALPH_E_OUTOFMEMORY);

	// attach it to our kernel doc thingy
	ok = pThis->pRalphView->AttachKernelDoc(pThis->pDocument);
	if(!ok)
		RALPH_RETURN (RALPH_E_UNEXPECTED);

	// create the window
	ok = pThis->pRalphView->Create(ParentWnd,HasBorder ? WS_BORDER : 0);

	TRACEUSER( "Chris", _T("Create Window %s\n",ok ? "OK!!":"FAILED!!"));	

	GET_RALPH_ERROR();

	if(!ok)
		RALPH_RETURN (RALPH_E_UNEXPECTED);
	
	// should we show the window now ?
	if(ShowWindow);
		pThis->pRalphView->ShowWindow(SW_SHOW);
	
	// scrollers ?
	pThis->pRalphView->ShowScrollers(ShowScrollers);

	pThis->hParentWnd = ParentWnd;
	// out put the Hwnd so that harness can do some stuff ( resize us etc..) 
	*ViewWindow = pThis->pRalphView->GetSafeHwnd();
	
	// create a timer to enable background rendering 
//	if(SetTimer(pThis->pRalphView->m_hWnd,CAM_TIMER_ID, CAM_TIMER_FREQUENCY,TimerProcA)==0)
//		RALPH_RETURN (RALPH_E_UNEXPECTED);
	
	// Setup the initial view mode
	//pThis->SelectViewMode(Vmode);
	pThis->Viewmode = Vmode;

	pThis->pDocument->SetRalphViewMode(Vmode);

	RALPH_RETURN (S_OK);
}


/********************************************************************************************

>	STDMETHODIMP_ (BOOL) RalphDocument::XCreation::LoadDocument(TCHAR * pFileToLoad)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96				
	Purpose:	Clean up the document and do a native load
   
********************************************************************************************/

STDMETHODIMP RalphDocument::XCreation::LoadDocument(TCHAR * pFileToLoad)
{
    METHOD_PROLOGUE(RalphDocument, Creation)

	if(pFileToLoad || pThis->pDocument || pThis->pRalphView)
	{
		// Little bit of niceness to handle async and sync loads at the same time...
		DWORD we = 0;
		MSG  m_msgCur;
		do
		{
			// pump message, but break on WM_QUIT
			while(::GetCurrentThreadId() && ::PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE))
			{
				AfxGetApp()->PumpMessage();
			}
			
			// wait for a "safe to enter" or an "abort" event
			we = WaitForMultipleObjects(2,RalphDocument::WakeEvents,FALSE,1);

			// Bad in render xar file and drop a cmx file situations...
			/*if(we == WAIT_OBJECT_0)
			{
				RALPH_RETURN (E_FAIL);
			}*/
		} while(we == WAIT_TIMEOUT);
	}
		
	START_RALPH_METHOD()
	
	if(pFileToLoad==NULL)
			RALPH_RETURN (E_FAIL);
	if(pThis->pDocument==NULL)
			RALPH_RETURN (E_FAIL);
	if(pThis->pRalphView==NULL)
			RALPH_RETURN (E_FAIL);

	// it would be nice to reinit the document here !!
	//pDocument->SetCurrent();
	//pDocument->ReInit();
	
	//AfxDebugBreak();
	
	pThis->pDocument->SetCurrent();
	
	// make it the selected view
	Document::SetSelectedViewAndSpread(pThis->pDocument, pThis->pRalphView->GetDocViewPtr(), NULL);

	// TESTING
	pThis->pRalphView->ShowWindow(SW_SHOW);

	// Make sure these point to this doc and this view.
	if (pThis->pDocument->GetFirstDocView() != NULL) 
		pThis->pDocument->GetFirstDocView()->SetCurrent();

	// Tell the Doc we are not importing the file
	pThis->pDocument->SetIsImporting(FALSE);

	// do a busy cursor
	HCURSOR hWaitCurs = LoadCursor(NULL,_R(IDC_WAIT));
	HCURSOR hOldCurs = SetCursor(hWaitCurs);

	//BOOL IsImportOK = TRUE;

	// Do the load.
//	CCDiskFile file(PathName(pFileToLoad), ios::in | ios::binary);
	CCDiskFile file(1024, FALSE, TRUE);

	TRY
	{
		// Open the xxx.pfm file
		pThis->IsImportOK = file.open(PathName(pFileToLoad), ios::in | ios::nocreate | ios::binary);
	}	
	CATCH(CFileException, e)
	{
		Error::ClearError();
		pThis->IsImportOK = FALSE;
	}
	END_CATCH

	if(pThis->IsImportOK)
	{
//		ERROR3_PF(("RalphDoc::LoadDocument suceeded with '%s' to load", pFileToLoad));	
		pThis->IsImportOK = InvokeNativeFileOp(OPTOKEN_NATIVELOAD, &file, FILTERID_GENERIC);
	}
		
	if(pThis->pRalphView)
		pThis->pRalphView->SetSafeToDraw(pThis->IsImportOK);

	// set the cursor back
	SetCursor(hOldCurs);

	// Set the flag back to its default value
	if (pThis->pDocument!=NULL)
		pThis->pDocument->SetIsImporting(TRUE);

	// See if the Import worked OK
	if (!pThis->IsImportOK)
	{
		TRACE( _T("SetEventA %d\n"), ::GetCurrentThreadId());
		if(pThis->pRalphView)
			pThis->pRalphView->Invalidate();	
		SetEvent(RalphDocument::g_hSafeToEnterLoad);
		RALPH_RETURN (E_FAIL);
	}
	
	// TESTING
	//pThis->pRalphView->ShowWindow(SW_SHOW);
	
	// Document::SetSelectedViewAndSpread(pThis->pDocument, NULL, NULL);

	pThis->pDocument->SetCurrent();
	Document::SetSelectedViewAndSpread(pThis->pDocument, pThis->pRalphView->GetDocViewPtr(), NULL);

	// Setup the initial view mode
	if(pThis->Viewmode!=DEFER_SETTINGVIEW)
		pThis->SelectViewMode(pThis->Viewmode);

	// Were ready to render
	pThis->pDocument->GetSafeRenderPointer().SetPointerInValid();

	TRACE( _T("SetEventB %d\n"), ::GetCurrentThreadId());
	SetEvent(RalphDocument::g_hSafeToEnterLoad);

	GET_RALPH_ERROR();
	RALPH_RETURN (S_OK);
}




/********************************************************************************************

>	STDMETHODIMP_ (BOOL) RalphDocument::XCreation::RenderToDC(HDC TheDC, DWORD X,
									DWORD Y, DWORD Width, DWORD Height, DWORD DPI)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/96				
	Purpose:	Render the current document to the given HDC
   
********************************************************************************************/
STDMETHODIMP RalphDocument::XCreation::RenderToDC(HDC hDC, DWORD X, DWORD Y,
												  DWORD Width, DWORD Height, DWORD DPI)
{
    METHOD_PROLOGUE(RalphDocument, Creation)
	START_RALPH_METHOD()

	pThis->PrintRalphView(hDC, X, Y, Width, Height, DPI);
			
	GET_RALPH_ERROR();
	RALPH_RETURN (S_OK);
}


/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCreation::AddRef()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IRalphDocument's AddRef implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XCreation::AddRef()
{
	METHOD_PROLOGUE(RalphDocument, Creation)
	
	return pThis->ExternalAddRef();
}

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCreation::Release()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IRalphDocument's Release implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XCreation::Release()
{
	METHOD_PROLOGUE(RalphDocument, Creation)
	
	return pThis->ExternalRelease();
}

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCreation::QueryInterface()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IRalphDocument's QueryInterface implementation
********************************************************************************************/

STDMETHODIMP RalphDocument::XCreation::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE(RalphDocument, Creation)
	
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}


//*******************************************************************************************
// ******************* Command Control ******************************************************
//*******************************************************************************************
 
/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::SetToolMode
	( ToolMode toolmode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/96				
	Purpose:	ICommandControl SetToolmode implementation
********************************************************************************************/

STDMETHODIMP RalphDocument::XCommandControl::SetToolMode(
	ToolMode toolmode)
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
 	START_RALPH_METHOD()

	pThis->Toolmode = toolmode;

	if(pThis->SelectTool(toolmode)==FALSE)
		RALPH_RETURN (E_FAIL);
	
	GET_RALPH_ERROR();

	RALPH_RETURN (S_OK);

}  


/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::PreviousZoom()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/96				
	Purpose:	ICommandControl PreviousZoom implementation	- just call the op
********************************************************************************************/

STDMETHODIMP RalphDocument::XCommandControl::PreviousZoom()
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	START_RALPH_METHOD()

	OpZoomDescriptor::FakeInvoke(OPTOKEN_ZOOMPREV);

	GET_RALPH_ERROR();

	RALPH_RETURN (S_OK);
} 

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::ZoomToDrawing()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/96				
	Purpose:	ICommandControl ZommToDrawing implementation - just call the op
********************************************************************************************/

STDMETHODIMP RalphDocument::XCommandControl::ZoomToDrawing()
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	START_RALPH_METHOD()
	
	// To enable ZoomToFit during async downloading we will
	// zoom to the stored viewport rectangle 
	// if the standard ZOOMDRAWING is used we zoom to a random rectangle 
	// according to the state of the tree
	DocRect* ViewportRect = pThis->GetViewportRect() ;
	RalphView *  pRalphView = pThis->GetRalphView();
	if(!ViewportRect->IsEmpty() && pRalphView)
	{
		SendMessage(pRalphView->GetSafeHwnd(),WM_ZOOMTORECT,NULL,(INT32)(ViewportRect));
	}
	else
	{
		OpZoomDescriptor::FakeInvoke(OPTOKEN_ZOOMDRAWING);
	}

	GET_RALPH_ERROR();

	RALPH_RETURN (S_OK);
}

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::ZoomToFullScale()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/96				
	Purpose:	ICommandControl ZoomToFullScale implementation
********************************************************************************************/

STDMETHODIMP RalphDocument::XCommandControl::ZoomToFullScale()
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	START_RALPH_METHOD()

	// this will set the scale to  100% and reset the origin
	pThis->ZoomToFullScale();
	GET_RALPH_ERROR();

	RALPH_RETURN (S_OK);
}  
  

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::SetQuality(QualityLevel Quality)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/96				
	Purpose:	ICommandControl SetQualityLevel implementation - set the render quality for
				this document
********************************************************************************************/

STDMETHODIMP RalphDocument::XCommandControl::SetQuality(QualityLevel Quality)
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	START_RALPH_METHOD()

	pThis->Quality = Quality;
	
	INT32 ViewQuality = QUALITY_LEVEL_ANTI;
	 
	// translate our QualityLevel enum into a value that Camelot understands
	switch(Quality)
	{
	case ANTI_ALIASED:
		ViewQuality = QUALITY_LEVEL_ANTI;
		break;
	case NORMAL:
		ViewQuality = QUALITY_LEVEL_NORMAL;
		break;
	case SIMPLE:
		ViewQuality = QUALITY_LEVEL_SIMPLE;
		break;
	case OUTLINE:
		ViewQuality = QUALITY_LEVEL_OUTLINE;
	}
	
	
	

	// Set the render quality on this doc	
	if(pThis->SelectViewQuality(ViewQuality)==FALSE)
		RALPH_RETURN(E_FAIL);

	GET_RALPH_ERROR();
	RALPH_RETURN(S_OK);

}  

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::SetQuality(QualityLevel Quality)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/96				
	Purpose:	ICommandControl SetQualityLevel implementation - set the render quality for
				this document
********************************************************************************************/

STDMETHODIMP RalphDocument::XCommandControl::SetURL(CString InURL)
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	START_RALPH_METHOD()

	pThis->DocumentURL = InURL;

//	pThis->Quality = Quality;
	
//	if(pThis->SelectViewQuality(ViewQuality)==FALSE)
//		RALPH_RETURN(E_FAIL);

//	GET_RALPH_ERROR();
	RALPH_RETURN(S_OK);

}  

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::SetDitherType(DitherType )

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/96				
	Purpose:	ICommandControl SetDitherType implementation
********************************************************************************************/

STDMETHODIMP RalphDocument::XCommandControl::SetDitherType(DitherType Dither)
{
	
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	START_RALPH_METHOD() ;

	if(pThis->SelectDitherType( Dither )==FALSE)
		RALPH_RETURN(E_FAIL);

	GET_RALPH_ERROR();

	RALPH_RETURN(S_OK);
}  


/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::SetPaletteMode(BOOL Foreground)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96				
	Purpose:	ICommandControl SetPaletteMode implementation
				(Plugins rely on their hosts to setup fabby palettes, so should use
				background palettes. Quickviewers should do their own palettes, so
				should call this with Foreground = TRUE)
********************************************************************************************/

STDMETHODIMP RalphDocument::XCommandControl::SetPaletteMode(BOOL Foreground)
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	START_RALPH_METHOD() ;

	pThis->SelectPaletteMode(Foreground);

	GET_RALPH_ERROR();

	RALPH_RETURN(S_OK);
}  


/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::AddRef()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	ICommandControl's AddRef implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XCommandControl::AddRef()
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)

	return pThis->ExternalAddRef();
}

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::Release()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	ICommandControl's Release implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XCommandControl::Release()
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	return pThis->ExternalRelease();
}

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XCommandControl::QueryInterface()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	ICommandControl's QueryInterface implementation
********************************************************************************************/

STDMETHODIMP RalphDocument::XCommandControl::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}


/********************************************************************************************

>	STDMETHODIMP RalphDocument::XCommandControl::HasPreviousZoom()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	ICommandControl's HasPreviousZoom implementation
********************************************************************************************/
STDMETHODIMP RalphDocument::XCommandControl::HasPreviousZoom()
{
	METHOD_PROLOGUE(RalphDocument, CommandControl)
	START_RALPH_METHOD()
	HRESULT hr =S_FALSE;
	DocView * pDocView = pThis->pRalphView->GetDocViewPtr();
	
	if(pDocView==NULL)
		RALPH_RETURN(E_FAIL);
	
	if((pDocView != NULL && pDocView->GetPrevZoomIndex() != cUninitZoomIndex)==FALSE)
		hr=S_OK;
		
	RALPH_RETURN(hr);
	
}

//*******************************************************************************************
// ******************* View Control *********************************************************
//*******************************************************************************************

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XViewControl::AddRef()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IViewControl's AddRef implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XViewControl::AddRef()
{
	METHOD_PROLOGUE(RalphDocument, ViewControl)

	return pThis->ExternalAddRef();
}

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XViewControl::Release()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IViewControl's Release implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XViewControl::Release()
{
	METHOD_PROLOGUE(RalphDocument, ViewControl)
	return pThis->ExternalRelease();
}

/********************************************************************************************

>	STDMETHODIMP_ BOOL RalphDocument::XViewControl::QueryInterface()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IViewControl's QueryInterface implementation
********************************************************************************************/

STDMETHODIMP RalphDocument::XViewControl::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE(RalphDocument, ViewControl)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}
 
/********************************************************************************************

>	STDMETHODIMP RalphDocument::XViewControl::ShowScrollers(BOOL state)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/96				
	Purpose:	IViewControl ShowScrollers implementation - just show/hide the scrollers
********************************************************************************************/

STDMETHODIMP RalphDocument::XViewControl::ShowScrollers(BOOL state)
{
	METHOD_PROLOGUE(RalphDocument, ViewControl)
	START_RALPH_METHOD();

	if( pThis->pRalphView == NULL)
		RALPH_RETURN(E_FAIL);

	pThis->pRalphView->ShowScrollers(state);
 	
	GET_RALPH_ERROR();
	RALPH_RETURN(S_OK);
}
 
/********************************************************************************************

>	STDMETHODIMP RalphDocument::XViewControl::SetViewMode(ViewMode Viewmode)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/96				
	Purpose:	IViewControl SetViewMode - PAGE_VIEW,DOCUMENT_VIEW or DRAWING_VIEW
********************************************************************************************/

STDMETHODIMP RalphDocument::XViewControl::SetViewMode(ViewMode Viewmode)
{
	METHOD_PROLOGUE(RalphDocument, ViewControl)
	START_RALPH_METHOD()
	
	if(pThis->SelectViewMode(Viewmode)==FALSE)
		RALPH_RETURN(E_FAIL);

	RALPH_RETURN(S_OK);

} 


//*******************************************************************************************
// ******************* InitRalph        *****************************************************
//*******************************************************************************************

/********************************************************************************************

>	STDMETHODIMP RalphDocument::XInitRalphk::AddRef()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IInitRalph's AddRef implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XInitRalph::AddRef()
{
	METHOD_PROLOGUE(RalphDocument,InitRalph)

	return pThis->ExternalAddRef();
}

/********************************************************************************************

>	STDMETHODIMP RalphDocument::XInitRalph::Release()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IInitRalph's Release implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XInitRalph::Release()
{
	METHOD_PROLOGUE(RalphDocument, InitRalph)
	return pThis->ExternalRelease();
}

/********************************************************************************************

>	STDMETHODIMP RalphDocument::XInitRalph::QueryInterface()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IInitRalph's QueryInterface implementation
********************************************************************************************/

STDMETHODIMP RalphDocument::XInitRalph::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE(RalphDocument, InitRalph)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

/********************************************************************************************

>	STDMETHODIMP RalphDocument::XInitRalph::SetErrorCallback
									( IErrorCallback* pCallbackInterface)


	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IInitRalph SetErroCallback implementation
********************************************************************************************/

STDMETHODIMP RalphDocument::XInitRalph::SetErrorCallback(
	 IErrorCallback* pCallbackInterface)
{
	METHOD_PROLOGUE(RalphDocument, InitRalph)
	START_RALPH_METHOD()
	
	pThis->pIErrorCallback = pCallbackInterface;
//	pThis->pIErrorCallback->AddRef();
	
	RALPH_RETURN(S_OK);
}



//*******************************************************************************************
// ******************* AsynchDownLoad   *****************************************************
//*******************************************************************************************

/********************************************************************************************

>	STDMETHODIMP RalphDocument::XAsynchDownLoad::AddRef()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IInitRalph's AddRef implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XAsynchDownLoad::AddRef()
{
	METHOD_PROLOGUE(RalphDocument,AsynchDownLoad)

	return pThis->ExternalAddRef();
}

/********************************************************************************************

>	STDMETHODIMP RalphDocument::XAsynchDownLoad::Release()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IInitRalph's Release implementation
********************************************************************************************/


STDMETHODIMP_(UINT32) RalphDocument::XAsynchDownLoad::Release()
{
	METHOD_PROLOGUE(RalphDocument, AsynchDownLoad)
	return pThis->ExternalRelease();
}

/********************************************************************************************

>	STDMETHODIMP RalphDocument::XAsynchDownLoad::QueryInterface()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	IInitRalph's QueryInterface implementation
********************************************************************************************/

STDMETHODIMP RalphDocument::XAsynchDownLoad::QueryInterface(
	REFIID iid, LPVOID* ppvObj)
{
	METHOD_PROLOGUE(RalphDocument, AsynchDownLoad)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
}

/*
	STDMETHOD(StartAsynchDownLoad) ();
	STDMETHOD(OnDataAvailable) (CString &Buffer);
	STDMETHOD(EndAsynchDownLoad) (HRESULT Status);
*/
/********************************************************************************************

>	STDMETHODIMP RalphDocument::XAsynchDownLoad::StartAsynchDownLoad()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	Create the file for the download and start the thread
********************************************************************************************/

STDMETHODIMP RalphDocument::XAsynchDownLoad::StartAsynchDownLoad()
{
	METHOD_PROLOGUE(RalphDocument, AsynchDownLoad)
	START_RALPH_METHOD()
	

	if(!pThis->StartLoadThread())
		RALPH_RETURN(RALPH_E_LOADFAILED);
	
	RALPH_RETURN(S_OK);
}

/********************************************************************************************

>	STDMETHODIMP RalphDocument::XAsynchDownLoad::OnDataAvailable) (CString &Buffer)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	write the data to the asynch down load file
********************************************************************************************/

STDMETHODIMP RalphDocument::XAsynchDownLoad::OnDataAvailable (unsigned char* pReadBuffer,DWORD BytesRead)
{
	METHOD_PROLOGUE(RalphDocument, AsynchDownLoad)
	START_RALPH_METHOD()

	pThis->OnDataAvailable(pReadBuffer,BytesRead);

	RALPH_RETURN(S_OK);
}

/********************************************************************************************

>	STDMETHODIMP RalphDocument::XAsynchDownLoad::EndAsynchDownLoad(HRESULT Status)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96				
	Purpose:	End the download according to the status
********************************************************************************************/

STDMETHODIMP RalphDocument::XAsynchDownLoad::EndAsynchDownLoad(HRESULT Status)
{
	METHOD_PROLOGUE(RalphDocument, AsynchDownLoad)
	START_RALPH_METHOD()
	
	if(!pThis->EndAsynchDownLoad(Status))
		RALPH_RETURN(RALPH_E_LOADFAILED);	
	
	RALPH_RETURN(S_OK);
}


/********************************************************************************************

>	STDMETHODIMP RalphDocument::XInitRalph::SetEventCallback
									( IEventCallback* pCallbackInterface)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Inputs:		pCallbackInterface: A pointer to an event callback interface
	Returns:	S_OK ( for now)
	Purpose:	Called to register a Harness interface to receive event notifications. 
********************************************************************************************/

STDMETHODIMP RalphDocument::XInitRalph::SetEventCallback(
	 IEventCallback* pCallbackInterface)
{
	METHOD_PROLOGUE(RalphDocument, InitRalph)
	START_RALPH_METHOD()
	pThis->pIEventCallback = pCallbackInterface;
	RALPH_RETURN(S_OK);
}


#endif
