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
// errors.cpp : implementation file
//

/*
*/



#include "camtypes.h"

#include "errors.h"
#include "camelot.h"
#include "strings.h"
#include "tool.h"
//#include "oilmods.h"
#include "basestr.h"
//#include "alex.h"			// For _R(IDS_SERIOUSERROR)
//#include "timdlg.h"
//#include "tim.h"
//#include "andy.h"
//#include "simon.h"			// for _R(IDS_HELP), the text on help buttons
#include "ensure.h"
//#include "resource.h"
//#include "ctrlhelp.h"
//#include "errorbox.h"
#include "helpuser.h"
//#include "basebar.h"		// For gallery creation error box fix...
//#include "justin2.h"
#include "ralphint.h"
#include "ralphdoc.h"
#include "document.h"
#include "ralpherr.h"
#include "camprofile.h"

// Put the source file into the revision list
DECLARE_SOURCE("$Revision$");

#if 0
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#endif

static BOOL ErrorHasBeenReported = TRUE;
static UINT32	InSetError = 0;			// In either of the SetError routines
#ifndef EXCLUDE_FROM_XARALX
static UINT32 ErrorBoxRecurse = 0;	// Incremented as per how many recursive error boxes we have
#endif

// Used to indicate what kind of error message is currently defined.
typedef enum
{
	ERRORSTAT_NONE,
	ERRORSTAT_TEXT,
	ERRORSTAT_ID
} ErrorStatus;

static ErrorStatus ErrStatus = ERRORSTAT_NONE;


UINT32 Error::RenderThread = 0;	// We're not in the thread

wxString Error::UserName;

/********************************************************************************************

>	ErrorInfo::ErrorInfo()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/94
	Purpose:	Initialises the fields of the error info structure to sensible defaults.

********************************************************************************************/

ErrorInfo::ErrorInfo()
{
	ErrorMsg  = 0;
	Title	  = 0;
	Button[0] = _R(IDS_OK);
	Button[1] = 0;
	Button[2] = 0;
	Button[3] = 0;
	OK        = 1;
	Cancel    = 2;
	Help      = 0;				// by default there is no help button
}

/********************************************************************************************

>	HRESULT Error::ErrIDToHRESULT(UINT32 ErrID)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/94
	Purpose:	Convert our ErrID's to HRESULTS if we're not interested return S_OK

********************************************************************************************/
HRESULT Error::ErrIDToHRESULT(UINT32 ErrID)
{
#ifdef RALPH
	return GetHRESULTFromID(ErrID);
#endif
	return S_OK;
}
/********************************************************************************************

>	HRESULT Error::GetRalphError()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/94
	Purpose:	ERROR1 - No local reporting -pass the mapped Error code to the harness
				ERROR2 - Report the Error locally - don't pass to handler 
				ERROR3 - Report as ERROR2's - won't occur in retails - 

********************************************************************************************/
HRESULT Error::GetRalphError()
{
	
	UINT32 Err = Error::GetRalphErrorNumber();
	wxString ErrStr( Error::GetErrorString() );
	HRESULT hr = S_OK;

	// ERROR2's 3's
	if(Err==0)
	{
		if(!ErrStr.IsEmpty())
		{
#if defined(_DEBUG) && defined(__WXMSW__)
			MessageBox(NULL,ErrStr,"Error",MB_OK);
#endif
			// make sure we clear ERROR2's 'cause we report them now
			Error::ClearError();
		}	
	}

	//ERROR1
	if(Err!=0)
	{
		// Do Mapping
		hr = ErrIDToHRESULT(Err);
	
	}
	// ERROR 2's 3's
	if(Err==0)
	{
		if(!ErrStr.IsEmpty())
		   hr = RALPH_E_INTERNAL;
	}
	
	return hr;
}



/********************************************************************************************

>	INT32 InformGeneral(UINT32 Error, UINT32 ModID, UINT32 ErrorMsg, 
					  UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4,
					  UINT32 OK, UINT32 Cancel)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> (then Andy)
	Created:	15/6/93
	Inputs:		Error - either ERRORTYPE_NORMAL, 
							   ERRORTYPE_ERROR, 
							   ERRORTYPE_WARNING or 
							   ERRORTYPE_SERIOUS
				ModID - Module ID to obtain messages (0=Kernel).
				ErrorMsg - The Error Message to display. 0 means use static Error
				Butt1-4  - The Text for the buttons. Up to 4 buttons can be specified
						   If Butt1 is 0, it means use '_R(IDS_OK)', i.e. "OK".
				OK - which button (1-4) should be the OK (default) button.
				Cancel - button should be the 'Cancel' button (and hence responds to Esc).
	Returns:	The number of the button used to close the dialog.
	Purpose:	The underlying function for [Tool-Module]Inform[Warning-Error].
	SeeAlso:	InformWarning; InformGeneral; ToolInformWarning; ToolInformError; 
				ModuleInformWarning; ModuleInformError

********************************************************************************************/
#ifdef RALPH
// **RALPH** error reporting

INT32 InformGeneral(UINT32 Error, UINT32 modID, UINT32 ErrorMsg, 
				  UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4,
				  UINT32 OK, UINT32 Cancel)
{    
	
	// ** this is the RALPH version of  InformError **
	// it's job is to report the error back to ralph's current harness
	

	// get a pointer to the kernel document
	Document *pkDoc = Document::GetCurrent();
	RalphDocument * pRalphDoc = NULL;
	if(pkDoc==NULL)
		return _R(IDS_OK);
	// get a pointer to it's attached ralphdoc
	pRalphDoc = pkDoc->GetRalphDoc();

	if(pRalphDoc== NULL)
		return _R(IDS_OK);


	if (ErrorMsg != 0)
	{
		// It's a new error message
		Error::SetError(ErrorMsg, modID);
	}

	// Make sure we haven't already reported this error!
/*	ENSURE((!ErrorHasBeenReported), 
		   "Ralph InformGeneral: This error has already been reported!");
*/
	if ( Error::IsInRenderThread() )
		TRACE( _T("Ralph InformGeneral called within RenderThread => serious rendering error"));


	ErrStatus = ERRORSTAT_NONE;

	if ( Error::IsInRenderThread() )
	{
		TRACE( _T("In RenderThread so clearing up system"));
		Error::RenderThreadReset();
		CamProfile::AtBase(CAMPROFILE_OTHER);
	}
	
	// if we're outside a ralph method
	// pass ERROR1's back to the harness
	// and report other errors immediately
	// and clear the error
	HRESULT hr = Error::GetRalphError();
	if(!pRalphDoc->IsInRalphMethod())
	{
		// get the error mapped to an HRESULT
		if(hr !=S_OK && hr!= RALPH_E_INTERNAL)
		{
			if(pRalphDoc)
			{
				pRalphDoc->PassErrorToHarness(hr);
			}
		}
		// and clear the error
		Error::ClearError();
	}

   	// We don't want to report this error again.
	ErrorHasBeenReported = TRUE;
	ErrorBoxRecurse--;
	return _R(IDS_OK) ; //result;
}

#else

// **CAMELOT Error reporting** 
INT32 InformGeneral(UINT32 Error, UINT32 modID, UINT32 ErrorMsg, 
				  UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4,
				  UINT32 OK, UINT32 Cancel)
{    
	PORTNOTETRACE("other", "InformGeneral called but unimplemented");
	TRACE(_T("InformGeneral Error=%d, ErrorMsg=%d"), Error, ErrorMsg);
#ifndef EXCLUDE_FROM_XARALX
	
	// Make sure there is at least one valid button.
	if (Butt1 == 0) Butt1 = _R(IDS_OK);

	if (ErrorBoxRecurse)
	{
		// Oh dear oh dear. Someone wants to put up an error box in the error handler. This is
		// **BAD** news. The most likely cause is an exception within this routine (or the other
		// InformGeneral).
		TRACE( _T("Recursive InformGeneral - Error in error handler! (1)\n"));
		// Don't risk putting up another error box - just return OK & hope for the best
		::MessageBeep(MB_OK); // shouldn't cause too much trouble
		return(OK);
	}

	ErrorBoxRecurse++;
	if (ErrorMsg != 0)
	{
		// It's a new error message
		Error::SetError(ErrorMsg, modID);
	}

	// Make sure we haven't already reported this error!
	ENSURE((!ErrorHasBeenReported), 
		   "InformGeneral: This error has already been reported!");

	// In retail builds, we report the error anyway, just in case - it's better to have
	// two errors reported than none at all!
	CInformErrorDialog	MyErrorBox(NULL);

    // Set the params according to what was supplied
	MyErrorBox.m_StaticTextStr 	= ErrorMsg;
	MyErrorBox.m_ButtonStr[0] 	= Butt1;
	MyErrorBox.m_ButtonStr[1] 	= Butt2;
	MyErrorBox.m_ButtonStr[2] 	= Butt3;
	MyErrorBox.m_ButtonStr[3] 	= Butt4;
	MyErrorBox.m_ErrorBoxType 	= Error;
	MyErrorBox.m_OwnerModule  	= modID;
	MyErrorBox.m_OK 			= OK;
	MyErrorBox.m_Cancel 		= Cancel;

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// See if there is any on-line help associated with the given warning/error message.
	// If there is then we will add a "Help" button to the dialog.
	if (CanHelpUser(Error::GetErrorNumber()))
	{
		// There is some on-line help for this message.  First, find out how many buttons
		// were specified by the caller (we know they specified the first).
		INT32 nButtons = 1 + (Butt2 != 0) + (Butt3 != 0) + (Butt4 != 0);

		// Do we have room for another button?
		if (nButtons <= 4)
		{
			// Make the next available button the help button.
			MyErrorBox.m_ButtonStr[nButtons] = _R(IDS_HELP);
			MyErrorBox.m_Help = nButtons + 1;
		}
#ifdef _DEBUG
		else
		{
			TRACEUSER( "JustinF", _T("Message 0x%lX - no room for help button\n"),
									(UINT32) Error::GetErrorNumber());
		}
#endif
	}
#endif

	CamResource::DoneInit();
 	// Disable the system's functionality for serious errors (i.e. stop rendering etc).
 	if (Error == ERRORTYPE_SERIOUS || Error == ERRORTYPE_ENSURE) CCamApp::DisableSystem();
	if ( Error::IsInRenderThread() )
		TRACE( _T("InformGeneral called within RenderThread => serious rendering error"));

	// Bodge because ReleaseCapture() sometimes doesn't send WM_CANCELMODE (e.g. to custom controls) which
	// appears to be a bug somewhere in the Windows API
	if (GetCapture()) SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
	ReleaseCapture();

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Keep Control Helper system informed
	ControlHelper::InformModalDialogOpened();

	// Bodge so error boxes are given focus when bars/galleries are being created
	BaseBar::StartErrorBox();
#endif

	// 'Do' the dialog
	INT32 result = MyErrorBox.DoModal();

	ErrStatus = ERRORSTAT_NONE;

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Make sure we forget the old help content.
	SetNextMsgHelpContext(0);

	// Keep Control Helper system informed
	ControlHelper::InformModalDialogClosed();

	// Bodge so error boxes are given focus when bars/galleries are being created
	BaseBar::FinishErrorBox();
#endif

	if ( Error::IsInRenderThread() )
	{
		TRACE( _T("In RenderThread so clearing up system"));
		Error::RenderThreadReset();
		CamProfile::AtBase(CAMPROFILE_OTHER);
	}

	// Enable system if necessary.
 	if (Error == ERRORTYPE_SERIOUS || Error == ERRORTYPE_ENSURE) CCamApp::EnableSystem();

	// We don't want to report this error again.
	ErrorHasBeenReported = TRUE;
	Error::ClearError();
		
	ErrorBoxRecurse--;

	// if we were in a drag operation, cancel it (to prevent invalid drag state) fixes #11455
	DragManagerOp::AbortDrag();

	return result;
#else
	return OK;
#endif
}

#endif

/********************************************************************************************

>	INT32 InformGeneral(UINT32 Error, ErrorInfo *pInfo, UINT32 ModID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/07/94
	Inputs:		Error - the type of error (e.g. ERRORTYPE_SERIOUS)
				pInfo - pointer to the ErrorInfo object which desvribes this 'error'.
				ModID - module ID, if required (may be 0).
	Returns:	ID of the button that was pressed (NB. NOT the index of the button - i.e.
				it returns a resource ID).
	Purpose:	The underlying function for [Tool-Module]Inform[Warning-Error].
	SeeAlso:	InformWarning; InformGeneral; ToolInformWarning; ToolInformError; 
				ModuleInformWarning; ModuleInformError

********************************************************************************************/

INT32 InformGeneral(UINT32 Error, ErrorInfo* pInfo, UINT32 ModID)
{
	if (!pInfo) return _R(IDS_OK); // Catch this early
	return InformGeneral(Error,ModID, pInfo->ErrorMsg, 
				  pInfo->Button[1],pInfo->Button[2],pInfo->Button[3],pInfo->Button[4],
				  pInfo->OK,pInfo->Cancel);

	
}



/********************************************************************************************

>	void InformLastError(UINT32 ErrorType = ERRORTYPE_ERROR)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/93
	Inputs:		ErrorType is the error type (e.g. ERRORTYPE_WARNING) Defaults to ERRORTYPE_ERROR
				THIS PARAM IS NOW IGNORED.
	Outputs:	None
	Returns:	None
	Purpose:	Report the last error that was set with ERROR, ERRORIF, TOOLERROR et al.
				Always presents the user with an OK button. If there isn't an unreported
				error, does nothing.
	SeeAlso:	ERROR; ERRORIF; TOOLERROR

********************************************************************************************/

void InformLastError(UINT32)
{
	if (ErrStatus != ERRORSTAT_NONE)
	{
		InformError();
		ErrStatus = ERRORSTAT_NONE;
	}
}



/********************************************************************************************

>	INT32 InformError(UINT32 ErrorMsg = 0, 
					UINT32 Butt1 = 0, UINT32 Butt2 = 0, UINT32 Butt3 = 0, UINT32 Butt4 = 0, 
					UINT32 OK = 1, UINT32 Cancel = 2)

	INT32 ToolInformError(UINT32 ToolID, <as above>)

	INT32	ModuleInformError(UINT32 ModuleID, <as above>)


	INT32 InformSeriousError(UINT32 ErrorMsg = 0, 
						   UINT32 Butt1 = 0, UINT32 Butt2 = 0, UINT32 Butt3 = 0, UINT32 Butt4 = 0,
						   UINT32 OK = 1, UINT32 Cancel = 2)

	INT32 ToolInformSeriousError(UINT32 ToolID, <as above>)

	INT32	ModuleInformSeriousError(UINT32 ModuleID, <as above>)


	INT32 InformMessage(UINT32 ErrorMsg = 0, 
					  UINT32 Butt1 = 0, UINT32 Butt2 = 0, UINT32 Butt3 = 0, UINT32 Butt4 = 0,
					  UINT32 OK = 1, UINT32 Cancel = 2)

	INT32 ToolInformMessage(UINT32 ToolID, <as above>)

	INT32	ModuleInformMessage(UINT32 ModuleID, <as above>)


	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>, then Alex
	Created:	15/6/93
	Inputs:		ErrorMsg - The Error Message to display. 0 means use static Error
				Butt1-4  - The Text for the buttons. Up to 4 buttons can be specified
						   If Butt1 is 0, it means use '_R(IDS_OK)', i.e. "OK".
				OK - which button (1-4) should be the OK (default) button.
				Cancel - button should be the 'Cancel' button (and hence responds to Esc).
	Returns:	The number of the button used to close the dialog
	Purpose:	To display a dialog that gives an error message of some description. Up
				to 3 buttons can be defined for the user to select. It should be used
				when an error occurs that the user could make a decision about eg. Camelot
				failed to save the file, so give them the option to give up, try again or
				try a new filename/path etc.
				If ErrorMsg is zero, the box will use the last error returned by a function 
				with the RETURNERROR macro.
				ToolInformError takes an additional parameter on the front of the Tool ID.
				ModuleInformError takes a module ID. Both of these will try to find the strings
				used for errors and buttons in the owner module first, then the main .exe file.
				These are all in-line function stubs which call InformGeneral.
				The equivalent routines with 'Serious' in their name are the equivalent of those
				without except that they pass ERRORTYPE_SERIOUS and not ERRORTYPE_ERROR
				It's also worth noting tat these are just inline functions that call InformGeneral
				and they live solely in Errors.h
	SeeAlso:	InformWarning; RETURNERROR; InformGeneral

********************************************************************************************/





/********************************************************************************************

>	INT32 InformWarning( UINT32 ErrorMsg, UINT32 Butt1=0, UINT32 Butt2=0, UINT32 Butt3=0, UINT32 Butt4=0 )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/6/93
	Inputs:		ErrorMsg - The Error Message to display - 0 means use static Error
				Butt1-3  - The Text for the buttons. Up to 3 buttons can be specified
	Returns:	The number of the button used to close the dialog
	Purpose:	To display a dialog that gives a warning message of some description. Up
				to 3 buttons can be defined for the user to select. If ErrorMsg is zero
				the error string will be taken from the static Error variables, bringing 
				up the last error that was set by a function calling RETURNERROR.
	SeeAlso:	InformError

********************************************************************************************/


// Define the instances of the static variables belonging to the Error class

TCHAR Error::ErrorString[256];
UINT32  Error::ErrorID = 0;
UINT32  Error::RalphErrorID = 0;
UINT32  Error::ModuleID = 0;
UINT32  Error::LastErrorLine;							// line number of last error (or 0)
const char* Error::LastErrorFile = "Unknown.File";	// ptr to filename of last error - don't translate


/////////////////////////////////////////////////////////////////////////////
// Error handling routines

/***********************************************************************************************

>	void Error::SetError(UINT32 number, TCHAR* errstring, UINT32 module)
	void Error::SetError(UINT32 number, UINT32 module)
	void Error::SetErrorTool(UINT32 number, UINT32 tool)
	void Error::SetErrorSerious( const TCHAR* errstring )

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		number is the error number - usually a resource ID
				errstring pointer to a TCHAR array containing the string for the error
				module is a UINT32 uniquely identifying the module (0 if kernel)
				tool is a tool ID. The SetErrorSerious call is for serious error handlers
				who must succeed in setting the error regardless.
	Outputs:	-
	Returns:	-
	Purpose:	Called before exiting a routine that failed, this function sets up the static 
				variables which allow the calling routine to see what the error was.
				If the errstring parameter is passed, the module UINT32 is stored, but not used. This
				form of the function would usually be used for routines which want to build up an 
				error string on the fly. If the errstring is absent, the routine uses the UINT32 to
				identify the module, and loads a string from the appropriate resources.
	Errors:		-
	SeeAlso:	SmartLoadString

***********************************************************************************************/

void Error::SetError(UINT32 number, const TCHAR* errstring, UINT32 module)
{
	static TCHAR BASED_CODE LastResort[] = wxT("Cannot perform SetError (1)");
	// We now check for a real recursive call. Should only happen if we can't get translate
	// an ID to an error string. Perhaps if we're really low on memory. We must then set the
	// minimum acceptable error string...
	if (InSetError)
	{
		TRACE(FALSE,"SetError really has been called recursively (1)\n");
		RalphErrorID =ErrorID = _R(IDE_EX_BADOP); // should have it's own, but...
		ErrStatus = ERRORSTAT_TEXT;
		ErrorHasBeenReported=FALSE;
		ModuleID = module;
		lstrcpy( ErrorString, LastResort );
		return;
	}
	InSetError++;

// Andy has stubbed this code out as it caused problems, e.g.
// 'save changes' dialog after a GP fault had been caught made the dialog have the 'save
// changes' buttons, with the GP fault text
#if 0
	if (!ErrorHasBeenReported)
	{
		// ENSURE(FALSE, "Recursive SetError call"); IMHO this is pointless - Alex
		TRACE( _T("SetError called twice: ID = %u: %s\n"), number, errstring);
		InSetError--;
		return;
	}
#endif

	RalphErrorID = ErrorID = number;
	lstrcpy(ErrorString, errstring);
	ModuleID = module;
	ErrStatus = ERRORSTAT_TEXT;
	ErrorHasBeenReported = FALSE;
	TRACE( _T("Setting error: ID = %u: %s\n"), ErrorID, ErrorString);
	InSetError--;
}                          

void Error::SetErrorSerious( const TCHAR* errstring )
{
	// this must succeed. In particular it must NOT cause any other errors or ENSUREs
	ErrorID = 0; 
	ModuleID = 0;
	InSetError = 0;
	ErrStatus = ERRORSTAT_TEXT;
	ErrorHasBeenReported = FALSE;
	lstrcpy( ErrorString, errstring );
}

void Error::SetError(UINT32 number, UINT32 module)
{
	static TCHAR BASED_CODE LastResort[] = _T("Cannot perform SetError (2)");
	// We now check for a real recursive call. Should only happen if we can't get translate
	// an ID to an error string. Perhaps if we're really low on memory. We must then set the
	// minimum acceptable error string...
	if (InSetError)
	{
		TRACE(FALSE,"SetError really has been called recursively (2)\n");
		ErrorID = _R(IDE_EX_BADOP); // should have it's own, but...
		ErrStatus = ERRORSTAT_TEXT;
		ErrorHasBeenReported=FALSE;
		ModuleID = module;
		lstrcpy( ErrorString, LastResort );
		return;
	}
	InSetError++;
	if (!ErrorHasBeenReported)
	{
		// ENSURE(FALSE, "Recursive SetError call"); IMHO this is pointless - Alex
		TRACE( _T("SetError called twice: ID = %u  Module = %u\n"), number, module);
		InSetError--;
		return;
	}
	RalphErrorID = ErrorID = number;
	ModuleID = module;
	ErrorString[0] = 0;
	ErrStatus = ERRORSTAT_ID;
	ErrorHasBeenReported = FALSE;
	if (!SmartLoadString(module, ErrorID, ErrorString, 256 ) )
	{
		tsprintf( ErrorString, 256, _T("Error Number %u from module ID %u"), ErrorID, ModuleID );
	}
	
	TRACE( _T("Setting error: ID = %d: \"%s\"\n"), ErrorID, ErrorString);
	InSetError--;
}                          

void Error::SetErrorTool(UINT32 number, UINT32 toolID)
{
	Error::SetError(number, Tool::GetModuleID(toolID));
}



/********************************************************************************************

>	static void Error::ClearError()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Resets the error system to "no current error".
	Errors:		-
	SeeAlso:	Error::SetError

********************************************************************************************/

void Error::ClearError()
{
	TRACEUSER( "Chris", _T("Clear Error now \n") );
	RalphErrorID = ErrorID = ModuleID = 0;
	ErrorString[0] = 0;

	// Added 15/2/95 so that the function does as it name suggests and actually does clear
	// the current error completely from the system rather than just clearing out the error
	// id and the error string.
	ErrorHasBeenReported = TRUE;
	ErrStatus = ERRORSTAT_NONE;
}



/********************************************************************************************

>	void Error::MarkError( UINT32 LineNumber, const char *Filename )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Inputs:		Line number and filename, usually generated with __LINE__ and __FILE__
	Purpose:	Used to 'remember' where an error ocurred. Cannot be done with extra params
				to InternalSetError due to vararg strangeness. The function is declared inline
				on retail builds.
	Scope:		Public Static

********************************************************************************************/

/* Tech Note: The MARKERRORBODY macro lives in errors.h. The same macro is used for the inline
	version and out of line version - if you need to change the function body, edit the macro
*/

#if !INLINE_MARKERROR

void Error::MarkError( UINT32 LineNumber, const char *Filename )
MARKERROR_BODY( LineNumber, Filename )

#endif

#define	ResetWhere()		LastErrorLine = 0; LastErrorFile = "Unknown.File";

// \n's needed on the end of these

BOOL Error::IsUserName(const char *wanted)
{
	// CmpNoCase returns 0 for a match

	if (!UserName.CmpNoCase(_T("ALL")))
		return TRUE;

	if (!UserName.CmpNoCase(_T("")))
		return FALSE;

	wxString target (wanted, wxConvUTF8);

	if (!target.CmpNoCase(_T("ALL")))
		return TRUE;

	return (!UserName.CmpNoCase(target));
}

#ifdef _DEBUG

void Error::TraceWrite(const TCHAR * bufp, va_list args)
{
	// wxWidgets doesn't expect newlines in the string, but Camelot source provides them. So we print each bit
	// separately
#if 1
	// replace \n by a space - the real solution is to remove the \n from all the trace statements (yawn)
	TCHAR buf[MAXERRORFORMATLENGTH];
	lstrcpyn(buf, bufp, MAXERRORFORMATLENGTH);
	buf[MAXERRORFORMATLENGTH-1]=0;
	TCHAR * b=buf;
	do
	{
		if (*b == '\n') *b=' ';
	} while(*b++);
	wxVLogDebug(buf, args);

#else
	// this way is bad as it doesn't work with args either side of the newline

	TCHAR * newline;

	do
	{
		newline = _tcschr(bufp, _T('\n'));
		if (newline) *newline++=0;
		// We really should pass only the args before the newline here, but...
		wxVLogDebug(bufp, args);

		bufp=newline;
	} while (bufp && *bufp);
#endif
}	

#if 0 != wxUSE_UNICODE
// wxWidgets does this if we let it use the vsnprintf in wxLogDebug
// so we don't currently do this - this one converts from a char *
// and is only enabled on Unicode builds
void Error::FixFormat (const char * fmt, TCHAR * fmt2)
{
	wxString FString(fmt, wxConvUTF8);
	FixFormat(FString.c_str(), fmt2);
}
#endif

// wxWidgets does this if we let it use the vsnprintf in wxLogDebug
// so we don't currently do this
void Error::FixFormat (const TCHAR * fmt, TCHAR * fmt2)
{
	// Unicode - replace %s with %ls
	INT32 i=0;
	INT32 j=0;
	TCHAR c;

	do
	{
		c = fmt[i++];
		fmt2[j++]=c;
		if ( (c == _T('%')) && (fmt[i]==_T('s')) )
			fmt2[j++]=_T('l'); // this is safe as we know we had at least 2 spare chars
	} while ( c && (j < MAXERRORFORMATLENGTH-1)); // Strict comparison deliberate

	// for safety
	fmt2[MAXERRORFORMATLENGTH-1]=_T('\0');
}

void CDECL Error::TraceUser(const char *User, LPCTSTR fmt, ...)
{
	if (!IsUserName(User)) return;

	va_list marker;
	va_start( marker, fmt );
//	wxVLogDebug(fmt, marker);
	TraceWrite(fmt, marker);
	va_end( marker );
}

void CDECL Error::TraceAll(LPCTSTR fmt, ...)
{
	va_list marker;
	va_start( marker, fmt );
//	wxVLogDebug(fmt, marker);
	TraceWrite(fmt, marker);
	va_end( marker );
}

void CDECL Error::TraceTime (TCHAR * t)
{
	TCHAR buf[256];
	CamProfile::GetTimeString(buf, 256);
	TraceAll(_T("[%s] %s"),buf, t);
}

#endif

void CDECL Error::ReleaseTrace(LPCTSTR fmt, ...)
{
	TCHAR				buf[256];
	va_list				marker;
	va_start( marker, fmt );
	tvsprintf( buf, 256, fmt, marker );

#if defined( __WXMSW__ )
	OutputDebugString( buf );
#elif defined( __WXGTK__ )
	wprintf( buf );
#else
	#pragma error( "Not support on this architechure" )
#endif

	va_end( marker );
}



/********************************************************************************************

>	void CalcInternalMessage( LPTCHAR result, UINT32 Line, const TCHAR* Filename )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Inputs:		Line number and filename, usually generated with __LINE__ and __FILE__
	Outputs:	result is updated with a suitable string
	Purpose:	Used to translate something such as 1234 in kernel\blobby.cpp into something
				less readable (in fact 1234@k.bl.by). This is so source & line numbers can be
				reported to an ordinary user without giving too much away.

********************************************************************************************/

static void CalcInternalMessage( LPTCHAR result, UINT32 Line, const TCHAR* Filename )
{
	const TCHAR* Slash    = _tcsrchr( Filename, TEXT('\\') );
	const TCHAR* Dot      = _tcsrchr( Filename, TEXT('.')  );
	TCHAR CodedFile[20];

	if (Slash && Dot)
	{
		TCHAR* p = CodedFile;

		*p++ = Filename[0];					// 1st char is first letter of dirname
		*p++ = TEXT('.');					// seperators
		*p++ = Slash[1];					// 1st 2 chars of filename
		*p++ = Slash[2];
		*p++ = TEXT('.');
		*p++ = Dot[-2];						// last 2 chars of filename
		*p++ = Dot[-1];
		*p = 0;

		Filename = CodedFile; 
	}

	// this message should not be translated
	String_256 jcf(_R(IDS_INTERNAL_ERROR_MSG));
	tsprintf(result, 256, jcf, (UINT32) Line, (LPCTSTR) Filename);
}



/********************************************************************************************

>	void CDECL Error::XSetError( const TCHAR *fmt, ...)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Inputs:		printf-style arguments.
	Purpose:	Do NOT call directly. Called via the ERROR2xx macros. In debug builds, puts
				up an ensure box. In all builds, sets an error. Do NOT put \n's or \r's on the
				end.
	Scope:		Public Static

********************************************************************************************/
#ifndef RALPH
// ** normal XSetError **
void CDECL Error::XSetError( const TCHAR *fmt, ...)
{
#ifdef _DEBUG
	TCHAR				buf[256];
#if 0 != wxUSE_UNICODE
	TCHAR	fmt2[MAXERRORFORMATLENGTH];
	FixFormat(fmt, fmt2);
#else
	const TCHAR * fmt2=fmt;
#endif

	va_list marker;
	va_start( marker, fmt );
	tvsprintf( buf, 256, fmt2, marker );
	va_end( marker );
	
	// in debug builds we put up an ensure box
	EnsureFailedLine( buf, LastErrorFile, LastErrorLine );		// put up box

#endif

	XSetErrorC();
	return;
}
#if 0 != wxUSE_UNICODE
// ** normal XSetError ** extra version for Unicode build
void CDECL Error::XSetError( const char *fmt, ...)
{
#ifdef _DEBUG
	TCHAR				buf[256];
	TCHAR	fmt2[MAXERRORFORMATLENGTH];
	FixFormat(fmt, fmt2);

	va_list marker;
	va_start( marker, fmt );
	tvsprintf( buf, 256, fmt2, marker );
	va_end( marker );
	
	// in debug builds we put up an ensure box
	EnsureFailedLine( buf, LastErrorFile, LastErrorLine );		// put up box

#endif

	XSetErrorC();
	return;
}
#endif
#else
// ** RALPH XSetError() **
void CDECL Error::XSetError( const TCHAR *fmt, ...)
{
	TCHAR buf[256];
	TRACEUSER( "Chris", _T("oOoOo Ralph Set Error \n"));

#ifdef _DEBUG
#if 0 != wxUSE_UNICODE
	TCHAR	fmt2[MAXERRORFORMATLENGTH];
	FixFormat(fmt, fmt2);
#else
	const TCHAR * fmt2=fmt;
#endif

	va_list marker;
	va_start( marker, fmt );
	tvsprintf( buf, 256, fmt2, marker );
	va_end( marker );
	// in debug builds we put up an ensure box
	EnsureFailedLine( buf, LastErrorFile, LastErrorLine );		// put up box

#else
	XSetErrorC();
	InformError();
#endif
	return;
}
#if 0 != wxUSE_UNICODE
// Extra version for Unicode build
void CDECL Error::XSetError( const char *fmt, ...)
{
	TCHAR buf[256];
	TRACEUSER( "Chris", _T("oOoOo Ralph Set Error \n"));

#ifdef _DEBUG
	TCHAR	fmt2[MAXERRORFORMATLENGTH];
	FixFormat(fmt, fmt2);

	va_list marker;
	va_start( marker, fmt );
	tvsprintf( buf, 256, fmt2, marker );
	va_end( marker );
	// in debug builds we put up an ensure box
	EnsureFailedLine( buf, LastErrorFile, LastErrorLine );		// put up box

#else
	XSetErrorC();
	InformError();
#endif
	return;
}
#endif
#endif

/********************************************************************************************

>	void CDECL Error::XSetErrorC()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/95
	Inputs:		None
	Purpose:	Do NOT call directly. Sets up the internal error
	Scope:		Public Static

********************************************************************************************/

void CDECL Error::XSetErrorC()
{
	TCHAR				buf[256];
	// the error we set features a coded version of where the problem was
	CalcInternalMessage( buf, LastErrorLine, String_256( LastErrorFile ) );
	SetErrorSerious( buf );
	ResetWhere();
	return;
}

/********************************************************************************************

>	void CDECL Error::XSetError( UINT32 errID, ...)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Inputs:		An error ID, followed by _MakeMsg-style variable arguments
	Purpose:	Do NOT call directly. Called via the ERROR1xx macros.
	Scope:		Public Static

********************************************************************************************/

void CDECL Error::XSetError( UINT32 errID, ...)
{


	if ( (errID==FALSE) || (errID==TRUE) )
	{
		// someone probably used the wrong macro parameters e.g. TRUE and FALSE instead of ID
		// This call will set an _R(IDE_INTERNAL_ERROR) for us
		ERROR2RAW( "ERROR1 macro used with invalid parameters" );
		return;
	}

	TCHAR				buf[256];

	va_list marker;

	va_start( marker, errID );

	String_256 result;

	// load the format string as a resoure (note no module ID yet)
	if (!SmartLoadString(0, errID, buf, sizeof(buf)))
	{
		tsprintf( buf, 256, wxT("Error<%u>"), errID ); // keep inline
	}

	// now do _MakeMsg type formatting
	result.CCvsprintf(buf, marker);

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Set the help context.
	SetNextMsgHelpContext(errID);
#endif
	// ralph needs this so that he can map the ID to a HRESULT before passing it
	// back to a harness
	TRACEUSER( "Chris", wxT("oOoOo Ralph Set Error %d \n"), RalphErrorID );

	RalphErrorID =errID;

	// and copy result into ErrorString
	SetErrorSerious( result );

	// trace output because SetErrorSerious doesn't bother
	TRACE( wxT("Setting error: ID = %d: \"%s\"\n"), errID, ErrorString);

	// then tidy up	
	va_end( marker );

	ResetWhere();
}

/********************************************************************************************

>	void CDECL Error::XComplain( const TCHAR *fmt, ...)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Inputs:		printf-style arg list.
	Purpose:	Do NOT call directly. This func is used by the ERROR3xx macros. It does the
				_stprintf-thing, then puts up an ensure-type dialog box. It exists only in
				debug builds. Error:MarkError should be called beforehand.
	Scope:		Public Static

********************************************************************************************/

#ifdef _DEBUG
void CDECL Error::XComplain( const TCHAR *fmt, ...)
{
	TCHAR				buf[256];

#if 0 != wxUSE_UNICODE
	TCHAR	fmt2[MAXERRORFORMATLENGTH];
	FixFormat(fmt, fmt2);
#else
	const TCHAR * fmt2=fmt;
#endif

	va_list marker;
	va_start( marker, fmt );
	tvsprintf( buf, 256, fmt2, marker );								// convert the args
	va_end( marker );

	EnsureFailedLine( buf, LastErrorFile, LastErrorLine );		// put up box
	ResetWhere();
}

#if 0 != wxUSE_UNICODE
void CDECL Error::XComplain( const char *fmt, ...)
{
	TCHAR				buf[256];
	TCHAR	fmt2[MAXERRORFORMATLENGTH];
	FixFormat(fmt, fmt2);

	va_list marker;
	va_start( marker, fmt );
	tvsprintf( buf, 256, fmt2, marker );								// convert the args
	va_end( marker );

	EnsureFailedLine( buf, LastErrorFile, LastErrorLine );		// put up box
	ResetWhere();
}
#endif

#endif



INT32 InformError(UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK,	UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_ERROR, 0, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4,
						 OK, Cancel);

}


INT32 ToolInformError(UINT32 ToolID, UINT32 ErrorMsg,	UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4,	UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_ERROR, Tool::GetModuleID(ToolID), ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);

}


INT32 ModuleInformError(UINT32 ModID, UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_ERROR, ModID, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);

}


INT32 InformError(ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_ERROR, pInfo);
}


INT32 ToolInformError(UINT32 ToolID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_ERROR, pInfo, Tool::GetModuleID(ToolID));
}


INT32 ModuleInformError(UINT32 ModID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_ERROR, pInfo, ModID);
}


INT32 InformSeriousError(UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_SERIOUS, 0, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}


INT32 ToolInformSeriousError(UINT32 ToolID, UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_SERIOUS, Tool::GetModuleID(ToolID), ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}


INT32 ModuleInformSeriousError(UINT32 ModID, UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_SERIOUS, ModID, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}


INT32 InformSeriousError(ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_SERIOUS, pInfo);
}

INT32 ToolInformSeriousError(UINT32 ToolID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_SERIOUS, pInfo, Tool::GetModuleID(ToolID));
}

INT32 ModuleInformSeriousError(UINT32 ModID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_SERIOUS, pInfo, ModID);
}

INT32 InformWarning(UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{

	return InformGeneral(ERRORTYPE_WARNING, 0, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}

INT32 ToolInformWarning(UINT32 ToolID, UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_WARNING, Tool::GetModuleID(ToolID), ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}

INT32 ModuleInformWarning(UINT32 ModID, UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_WARNING, ModID, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}

INT32 InformWarning(ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_WARNING, pInfo);
}


INT32 ToolInformWarning(UINT32 ToolID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_WARNING, pInfo, Tool::GetModuleID(ToolID));
}


INT32 ModuleInformWarning(UINT32 ModID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_WARNING, pInfo, ModID);
}



INT32 AskQuestion(UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_QUESTION, 0, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}

INT32 ToolAskQuestion(UINT32 ToolID, UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_QUESTION, Tool::GetModuleID(ToolID), ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}



INT32 ModuleAskQuestion(UINT32 ModID, UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_QUESTION, ModID, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}


INT32 AskQuestion(ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_QUESTION, pInfo);
}


INT32 ToolAskQuestion(UINT32 ToolID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_QUESTION, pInfo, Tool::GetModuleID(ToolID));
}


INT32 ModuleAskQuestion(UINT32 ModID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_QUESTION, pInfo, ModID);
}



INT32 InformMessage(UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_NORMAL, 0, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);

}


INT32 ToolInformMessage(UINT32 ToolID, UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_NORMAL, Tool::GetModuleID(ToolID), ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);

}


INT32 ModuleInformMessage(UINT32 ModID, UINT32 ErrorMsg, UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4, UINT32 OK, UINT32 Cancel)
{
	return InformGeneral(ERRORTYPE_NORMAL, ModID, ErrorMsg, 
						 Butt1, Butt2, Butt3, Butt4, OK, Cancel);
}


INT32 InformMessage(ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_NORMAL, pInfo);
}


INT32 ToolInformMessage(UINT32 ToolID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_NORMAL, pInfo, Tool::GetModuleID(ToolID));
}


INT32 ModuleInformMessage(UINT32 ModID, ErrorInfo *pInfo)
{
	return InformGeneral(ERRORTYPE_NORMAL, pInfo, ModID);
}



#if 0
// old errors test code

INT32 test1(INT32 which)
{
	if (which==0)
	{
		ERROR1RAW( _R(IDE_INTERNAL_ERROR) );
		return 0;
	}
	if (which==1)
		ERROR1( 10, _R(IDE_INTERNAL_ERROR) );
	if (which==2)
		ERROR1IF( TRUE, 20, _R(IDE_INTERNAL_ERROR) );
	if (which==3)
		ERROR1_MSG( 30, (_R(IDT_EPS_BADSYNTAX), 2, 200) );
	if (which==4)
		ERROR1IF_MSG( TRUE, 40, (_R(IDT_EPS_BADSYNTAX), 1, 100) );
	return 0;
}



INT32 test2(INT32 which)
{
	if (which==0)
	{
		ERROR2RAW( "Simple raw error" );
		return 0;
	}
	if (which==1)
		ERROR2( 10, "Simple error" );
	if (which==2)
		ERROR2IF( TRUE, 20, "Simple error if");
	if (which==3)
		ERROR2_PF( 30, ("String (%s)", "blobby") );
	if (which==4)
		ERROR2IF_PF( TRUE, 40, ("Numbers (%d,%d)" , 42,43) );
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void TestErrorStuff()
{
	test1(0);
	test1(1);
	test1(2);
	test1(3);
	test1(4);

	InformError();

	test2(0);
	test2(1);
	test2(2);
	test2(3);
	test2(4);

	InformError();

	ERROR3("simple");
	ERROR3IF( TRUE, "simple" );
	ERROR3_PF( ("doc is %lx", 0x123456) );
	ERROR3IF_PF( TRUE, ("doc %s is %lx", "blobdoc", 0x123456) );

	TRACEUSER( "Andy", _T("simple trace\n"));
	TRACEUSER( "Andy", "complex %d %s trace", 42, _T("blobby\n") );
}

#endif


/********************************************************************************************

>	void Error::DumpStack(UINT32 frames)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14-Mar-2006
	Inputs:		None
	Purpose:	Dumps the stack
	Scope:		Public Static

********************************************************************************************/

void Error::DumpStack(UINT32 frames)
{
#ifdef _DEBUG
#ifndef __WXMAC__
	Error::StackWalker s;
	s.Walk(frames);
#else
	TRACE( _T("Request to dump stack not supported on WXMAC") );
#endif
#endif
}

/********************************************************************************************

>	void Error::StackWalker::OnStackFrame(const wxStackFrame & frame)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	14-Mar-2006
	Inputs:		None
	Purpose:	Dumps the stack
	Scope:		Public

********************************************************************************************/

#ifndef __WXMAC__
void Error::StackWalker::OnStackFrame(const wxStackFrame & frame)
{
#ifdef _DEBUG
	wxString l;
	l.Printf(_T("%d %s:%d %s"), frame.GetLevel(), frame.GetFileName().c_str(), frame.GetLine(), frame.GetName().c_str());
//	l = wxString(frame.GetName().c_str();
	wxLogDebug(l);
#endif
}
#endif
