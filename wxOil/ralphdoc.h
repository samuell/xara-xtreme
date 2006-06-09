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



#ifndef RALPHDOC_INC
#define RALPHDOC_INC

class Document;
class RalphView;

//#include <afxole.h>
//#include "ralphint.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "wrkcoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ccafile.h"
//#include "docrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// constant used for claiming timer for window
const INT32				CAM_TIMER_ID = 42;
// constant used for frequency of timer events (in milliseconds)
const INT32				CAM_TIMER_FREQUENCY  = 100;
// When async loading how many clock ticks before we render anything.
const INT32				ASYNC_RENDER_RATE = 3;
																	  
#define TEMP_FILE_PREFIX "XAR"

#define	QUALITY_LEVEL_ANTI		110
#define	QUALITY_LEVEL_NORMAL	100
#define	QUALITY_LEVEL_SIMPLE	50
#define	QUALITY_LEVEL_OUTLINE	10

// must be called at the start of every ralph method
#define START_RALPH_METHOD()	HRESULT hrX = S_OK;\
								pThis->InsideRalphMethod = TRUE;

// must	be used to return a value from a ralph method							
#define RALPH_RETURN(hr) {		ASSERT(pThis->InsideRalphMethod);\
								pThis->InsideRalphMethod = FALSE;\
								return (hr);}

// call to test whether a serious ERROR1 has been set - 
// if so will clear the error condition and return						
#define GET_RALPH_ERROR() 		hrX = Error::GetRalphError();\
								if(hrX!=S_OK && hrX!=RALPH_E_INTERNAL)\
								{	pThis->InsideRalphMethod = TRUE;\
									Error::ClearError();\
									RALPH_RETURN (hrX);}


#define WM_DOWNLOADINIT WM_USER + 0x1105	

#define WM_DOWNLOADCOMPLETE WM_USER + 0x1106

// Sent from the importer viewcomp result in a zoom to rect in the default thread. Hopefully stops
// corruption of RenderRegion stuff..
#define WM_ZOOMTORECT		WM_USER + 0x1107

// The old C++ Ralph Document

/********************************************************************************************

>	class LoadContext : public CCObject
	   		
	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96
	Purpose:	To encapsulate all data local to a download

********************************************************************************************/

class LoadContext :	public CCObject
{
	CC_DECLARE_DYNCREATE( LoadContext )
	
	public:
		LoadContext(); 
		virtual ~LoadContext();
		// asynch stuff
		wxCondition	   *hDataAvailableEvent; 
		volatile INT32   TotalBytes;
		volatile INT32   BytesAvailable;
		volatile BOOL   AsynchDownLoadComplete;

};
// The old C++ Ralph Document

/********************************************************************************************

>	class RalphDoc : public CCObject
	   		
	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/96
	Purpose:	

********************************************************************************************/
/*
class RalphDoc :	public CCObject
{
	CC_DECLARE_DYNCREATE( RalphDoc )
	
	public:
		 RalphDoc() { }; 
		 RalphDoc(HWND ParentWnd);
		~RalphDoc();
		
		// get a pointer to the ralphview window
		CWnd * GetViewWindow();
		
		// Create a RalphDoc as a child of this window
		BOOL Create(HWND ParentWnd);
		
		// Load a Document
		BOOL LoadDocument(TCHAR * pFileToLoad);
		
		// put the document back into a new state
		// delete tree operation history etc..
		BOOL NewDocument();

	private:
		Document * pDocument;
		RalphView * pRalphView;
		HWND hParentWnd;
};
*/

/********************************************************************************************

	class RalphDocument: public CCmdTarget

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96
	Purpose:	The RalphDocument COM object 

********************************************************************************************/


class RalphDocument : public wxEvtHandler
{
	DECLARE_DYNAMIC_CLASS( RalphDocument )

protected:
	RalphDocument();			// protected constructor used by dynamic creation
	virtual ~RalphDocument();
public:
	virtual void OnFinalRelease();

	// this state is set whenever a ralph method is entered
	// see START_RALPH_METHOD() RALPH_RETURN()
	BOOL IsInRalphMethod(){ return InsideRalphMethod;};
	
	// pass the error to the IErrorCallback if available
	BOOL PassErrorToHarness(HRESULT ErrCode);
	// Call to pass a web address selection to a harness
	void WebAddressSelected(String_256* InAddress);

	static BOOL	SelectTool(ToolMode toolmode);
	BOOL SelectViewQuality(INT32 ViewQuality);
	BOOL SelectViewMode(ViewMode Viewmode);
	BOOL SelectDitherType(DitherType Dither);
	BOOL SelectPaletteMode(BOOL Foreground);
	void SetSelected();
	// Functions to change the browser's status bar
	void SetStatusBar(String_256* pNewText);
	void SetStatusBar(wxString pNewText);
	void SetStatusBar();


	BOOL PrintRalphView( wxDC *TheDC, DWORD X, DWORD Y, DWORD Width, DWORD Height, DWORD DPI);

	// Ralph Document states
	DitherType			Dither;
	ToolMode			Toolmode;
	QualityLevel		Quality;
	ViewMode			Viewmode;			// PAGE_VIEW, DOCUMENT_VIEW, DRAWING_VIEW

	// filename of the temp file during the asynch download
	//char FileBuffer [_MAX_PATH];
	wxString			PathString;
					  
	CCAsynchDiskFile   *TempWriteAsynchFile;
	CCAsynchDiskFile   *TempReadAsynchFile;

	wxThread		   *pLoadThread ;
	BOOL				IsImportOK;
	BOOL				LoadErrorHasOccured;
	BOOL				CleanUpAfterAsynchDownLoad(BOOL ok);

private:
	DocRect				ViewportRect; 
		// Stores the viewport rectangle we must zoom to in the case of an async render
public:
	void SetViewportRect(DocRect& Viewport) { ViewportRect = Viewport; };
	DocRect* GetViewportRect() { return &ViewportRect; };

	RalphView * GetRalphView(){ return pRalphView; };

	Document * GetRalphDocument(){return pDocument;};

	String_256 GetDocumentURL();
	String_256 GetBaseURL();

	//------------------- Asynch download stuff ----------------
	// statics used in the great GetSelected/Current bodge
	static Document* GetImportingDoc(){
		//if(sm_pImportingDoc != NULL)
			//TRACEUSER( "Richard", _T("### GetImportingDoc() returning NON-NULL doc\n"));
		return sm_pImportingDoc; };
	static DWORD  GetImportingThreadID(){
		//if(sm_ImportThreadID != NULL)
			//TRACEUSER( "Richard", _T("### GetImportingThreadID() returning NON-NULL ID\n"));
		return sm_ImportThreadID;};
   
	// public cause they are used in the loadthread proc - sorry
	static Document* sm_pImportingDoc;
	static DWORD sm_ImportThreadID;
	
	// the event we use to end the download 
	static wxCondition *g_hAbortDownLoadEvent; 
	
	// the safe to enter event
	static wxCondition *g_hSafeToEnterLoad;
	
	// array to hold the above events for multiple waits
	static wxCondition *WakeEvents[2];

	static void OnRalphAboutToDie();

	static wxWindow	   *pTimerWnd;

	// Set to TRUE on entry and FALSE on exit from the timer proc. 
	static BOOL InsideRenderLoop; 

	static void SetRenderTimer(BOOL State);
		// Start/Stop the render timer 
protected:
	static BOOL RenderTimerOn;
protected:
	
	// a load context for this document -it encapsulates
	// all data required for thread synchronisation during downloads
	LoadContext TheLoadContext;

	wxString CurrentStatusText;
	wxString DocumentURL;

PORTNOTE("other","Remove OLE interface table")
#ifndef EXCLUDE_FROM_XARALX
	DECLARE_OLECREATE(RalphDocument)

	DECLARE_INTERFACE_MAP()
	
	BEGIN_INTERFACE_PART(Creation, ICreation)
		STDMETHOD(CreateViewWindow)	(	HWND ParentWnd,
										ViewMode Viewmode,
										BOOL ShowScrollers,
										BOOL ShowWindow,
										BOOL HasBorder,
										HWND * ViewWindow);
 
		STDMETHOD(LoadDocument) (TCHAR* pFileToLoad);
		STDMETHOD(GetVerNum) (TCHAR* pVerNum);
		STDMETHOD(RenderToDC) (HDC TheDC, DWORD X, DWORD Y, DWORD Width, DWORD Height, DWORD DPI);
	END_INTERFACE_PART(Creation)


	BEGIN_INTERFACE_PART(CommandControl, ICommandControl)
		STDMETHOD (SetToolMode)		(ToolMode toolMode);
		STDMETHOD(PreviousZoom)		();
		STDMETHOD(ZoomToDrawing) 	();
		STDMETHOD(ZoomToFullScale) 	();
		STDMETHOD(SetQuality)		(QualityLevel Quality);
		STDMETHOD(SetDitherType)	(DitherType Dither) ;
		STDMETHOD(SetPaletteMode)	(BOOL Foreground) ;
		STDMETHOD(HasPreviousZoom)	() ;
		STDMETHOD(SetURL)			(wxString InURL);
 
	END_INTERFACE_PART(CommandControl)

	BEGIN_INTERFACE_PART(ViewControl, IViewControl)
		STDMETHOD (ShowScrollers) ( BOOL State);
		STDMETHOD (SetViewMode ) (ViewMode Viewmode);
	END_INTERFACE_PART(ViewControl)

	BEGIN_INTERFACE_PART(InitRalph, IInitRalph)
		STDMETHOD (SetErrorCallback) (IErrorCallback* pCallbackInterface);
		STDMETHOD (SetEventCallback) (IEventCallback* pCallbackInterface);
	END_INTERFACE_PART(InitRalph)

	BEGIN_INTERFACE_PART(AsynchDownLoad, IAsynchDownLoad)
		STDMETHOD(StartAsynchDownLoad) ();
		STDMETHOD(OnDataAvailable) (unsigned char* pReadBuffer,DWORD BytesRead);
		STDMETHOD(EndAsynchDownLoad) (HRESULT Status);
	END_INTERFACE_PART(AsynchDownLoad)
#endif


private:
	
	Document		   *pDocument;		// A pointer to the document associated with an instance of a 
										// RALPH Document. 
	RalphView		   *pRalphView;		// The one and only view that can be associated with the document. 
	wxWindow		   *hParentWnd;
	BOOL DocumentViewInit;
	BOOL ZoomToFullScale();
	BOOL SaveZoomAndScroll();
	BOOL RestoreZoomAndScroll();
	BOOL SetDocumentViewMode(); // set up initial zoom and scroll offsets

	// used to save the last zoom and scroll offsets
	INT32 LastZoomIndex ;
	FIXED16 LastViewScale ;
	WorkCoord LastScrollOffset ;
	IErrorCallback * pIErrorCallback;
	IEventCallback * pIEventCallback;

	
	// flag set on entering ralph methods - cleared on exit
	// used by the error reporting system
	BOOL InsideRalphMethod ; 



	BOOL StartLoadThread();
	BOOL EndAsynchDownLoad(HRESULT Status);
	BOOL OnDataAvailable(unsigned char* pReadBuffer,DWORD BytesRead);
	
	static INT32 RalphDocCnt;

	wxCursor		   *hOldCurs;
	
public:
	// Flag for forcing a palette or not. The quick viewer should set this
	// to TRUE so it can force its own palette in the foreground. The plugins
	// should set it to FALSE, since they're dependant on their host...
	static BOOL ForegroundPalette;

	static void* RestoreStackLimit;

	// this will be called from the destructor of RalphView
	// to ensure that it isn't deleted twice
	void MakepRalphViewNULL() {pRalphView = NULL;};
};


#endif
