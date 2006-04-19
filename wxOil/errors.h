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
// inform.h : header file
//


/*
*/


#ifndef INC_ERRORS
#define INC_ERRORS


//#include "errordlg.h"

// The various error types below are used to pass to InformGeneral by the inline functions InformError,
// InformWarning etc. They are also stored inside the class CInformErrorDialogue
#define	ERRORTYPE_NORMAL	0
#define	ERRORTYPE_QUESTION	1
#define	ERRORTYPE_WARNING	2
#define	ERRORTYPE_ERROR		3
#define	ERRORTYPE_SERIOUS	4
#define	ERRORTYPE_ENSURE	5

/********************************************************************************************

>	class ErrorInfo

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/07/94
	Purpose:	Describe the error/warning/question that is to be displayed.
				Fields are: 
				MonoOn
				ErrorMsg - Resource ID of the error message.
				Title	 - resource ID of the dialogue title string (optional)
				Button   - array of up to 4 button string resource IDs.
				OK       - the button which is the 'OK' or default button, i.e. the one which 
				 	 	   should be activated by Enter (this in the range 1-4).
				Cancel   - the button which is the 'Cancel' button, i.e. the one which should
						   be activated by Esc (this in the range 1-4).
				Help     - the index of the button which is the 'Help' button (this in the 
					       range 1-4).
				MonoOff
	SeeAlso:	InformGeneral

********************************************************************************************/

class ErrorInfo
{
public:
	ErrorInfo();

	UINT32 ErrorMsg;
	UINT32 Title;
	UINT32 Button[4];
	UINT32 OK;
	UINT32 Cancel;
	UINT32 Help;
};

// The function used throughout the program should be moved to its own .h file really
INT32 CCAPI InformGeneral(UINT32 Error, 
						UINT32 modID, UINT32 ErrorMsg,
						UINT32 Butt1, UINT32 Butt2, UINT32 Butt3, UINT32 Butt4,
						UINT32 OK, UINT32 Cancel);

INT32 CCAPI InformGeneral(UINT32 Error, ErrorInfo *pInfo, UINT32 ModID = 0);


void CCAPI InformLastError(UINT32 Error = ERRORTYPE_ERROR);


// These functions are no longer inline as this just caused huge dependancy problems
// Errors first:
INT32 InformError(UINT32 ErrorMsg = 0, 
					   UINT32 Butt1  = 0, 
					   UINT32 Butt2  = 0, 
					   UINT32 Butt3  = 0, 
					   UINT32 Butt4  = 0,
					   UINT32 OK     = 1,
					   UINT32 Cancel = 2);

INT32 ToolInformError(UINT32 ToolID, 
						   UINT32 ErrorMsg = 0,
						   UINT32 Butt1  = 0, 
						   UINT32 Butt2  = 0, 
						   UINT32 Butt3  = 0, 
						   UINT32 Butt4  = 0,
						   UINT32 OK     = 1,
						   UINT32 Cancel = 2);


INT32 ModuleInformError(UINT32 ModID, 
							 UINT32 ErrorMsg = 0,
							 UINT32 Butt1  = 0, 
							 UINT32 Butt2  = 0, 
							 UINT32 Butt3  = 0, 
							 UINT32 Butt4  = 0,
							 UINT32 OK     = 1,
							 UINT32 Cancel = 2);

INT32 InformError(ErrorInfo *pInfo);
INT32 ToolInformError(UINT32 ToolID, ErrorInfo *pInfo);
INT32 ModuleInformError(UINT32 ModID, ErrorInfo *pInfo);

// now serious errors
INT32 InformSeriousError(UINT32 ErrorMsg = 0, 
							  UINT32 Butt1  = 0, 
							  UINT32 Butt2  = 0, 
							  UINT32 Butt3  = 0, 
							  UINT32 Butt4  = 0,
						   	  UINT32 OK     = 1,
						      UINT32 Cancel = 2);

INT32 ToolInformSeriousError(UINT32 ToolID, 
								  UINT32 ErrorMsg = 0,
						   		  UINT32 Butt1  = 0, 
						   		  UINT32 Butt2  = 0, 
						   		  UINT32 Butt3  = 0, 
						   		  UINT32 Butt4  = 0,
								  UINT32 OK     = 1,
								  UINT32 Cancel = 2);

INT32 ModuleInformSeriousError(UINT32 ModID, 
									UINT32 ErrorMsg = 0,
							 		UINT32 Butt1  = 0, 
							 		UINT32 Butt2  = 0, 
							 		UINT32 Butt3  = 0, 
							 		UINT32 Butt4  = 0,
								    UINT32 OK     = 1,
								    UINT32 Cancel = 2);

INT32 InformSeriousError(ErrorInfo *pInfo);
INT32 ToolInformSeriousError(UINT32 ToolID, ErrorInfo *pInfo);
INT32 ModuleInformSeriousError(UINT32 ModID, ErrorInfo *pInfo);

// now warnings:
INT32 InformWarning(UINT32 ErrorMsg = 0, 
						 UINT32 Butt1  = 0, 
						 UINT32 Butt2  = 0, 
						 UINT32 Butt3  = 0, 
						 UINT32 Butt4  = 0,
						 UINT32 OK     = 1,
						 UINT32 Cancel = 2);


INT32 ToolInformWarning(UINT32 ToolID, 
							 UINT32 ErrorMsg = 0,
							 UINT32 Butt1  = 0, 
							 UINT32 Butt2  = 0, 
							 UINT32 Butt3  = 0, 
							 UINT32 Butt4  = 0,
							 UINT32 OK     = 1,
						     UINT32 Cancel = 2);


INT32 ModuleInformWarning(UINT32 ModID, 
							   UINT32 ErrorMsg = 0,
							   UINT32 Butt1  = 0, 
							   UINT32 Butt2  = 0, 
							   UINT32 Butt3  = 0, 
							   UINT32 Butt4  = 0,
						   	   UINT32 OK     = 1,
						       UINT32 Cancel = 2);

INT32 InformWarning(ErrorInfo *pInfo);
INT32 ToolInformWarning(UINT32 ToolID, ErrorInfo *pInfo);
INT32 ModuleInformWarning(UINT32 ModID, ErrorInfo *pInfo);

// questions:
UINT32 AskQuestion(UINT32 ErrorMsg = 0, 
					   UINT32 Butt1  = 0, 
					   UINT32 Butt2  = 0, 
					   UINT32 Butt3  = 0, 
					   UINT32 Butt4  = 0,
					   UINT32 OK     = 1,
					   UINT32 Cancel = 2);


UINT32 ToolAskQuestion(UINT32 ToolID, 
						   UINT32 ErrorMsg = 0,
						   UINT32 Butt1  = 0, 
						   UINT32 Butt2  = 0, 
						   UINT32 Butt3  = 0, 
						   UINT32 Butt4  = 0,
						   UINT32 OK     = 1,
						   UINT32 Cancel = 2);


INT32 ModuleAskQuestion(UINT32 ModID, 
							 UINT32 ErrorMsg = 0,
							 UINT32 Butt1  = 0, 
							 UINT32 Butt2  = 0, 
							 UINT32 Butt3  = 0, 
							 UINT32 Butt4  = 0,
							 UINT32 OK     = 1,
							 UINT32 Cancel = 2);

INT32 AskQuestion(ErrorInfo *pInfo);
INT32 ToolAskQuestion(UINT32 ToolID, ErrorInfo *pInfo);
INT32 ModuleAskQuestion(UINT32 ModID, ErrorInfo *pInfo);


// now just messages:
INT32 InformMessage(UINT32 ErrorMsg = 0, 
						 UINT32 Butt1  = 0, 
						 UINT32 Butt2  = 0, 
						 UINT32 Butt3  = 0, 
						 UINT32 Butt4  = 0,
						 UINT32 OK     = 1,
						 UINT32 Cancel = 2);


INT32 ToolInformMessage(UINT32 ToolID, 
							 UINT32 ErrorMsg = 0,
							 UINT32 Butt1  = 0, 
							 UINT32 Butt2  = 0, 
							 UINT32 Butt3  = 0, 
							 UINT32 Butt4  = 0,
							 UINT32 OK     = 1,
							 UINT32 Cancel = 2);

INT32 ModuleInformMessage(UINT32 ModID, 
							   UINT32 ErrorMsg = 0,
							   UINT32 Butt1  = 0, 
							   UINT32 Butt2  = 0, 
							   UINT32 Butt3  = 0, 
							   UINT32 Butt4  = 0,
							   UINT32 OK     = 1,
							   UINT32 Cancel = 2);

INT32 InformMessage(ErrorInfo *pInfo);
INT32 ToolInformMessage(UINT32 ToolID, ErrorInfo *pInfo);
INT32 ModuleInformMessage(UINT32 ModID, ErrorInfo *pInfo);



/***********************************************************************************************

>	macro ERROR(UINT32 ErrID, retvalue)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	This macro should be used to return from a function when an error condition
				has been detected. The rule in Camelot is that every function which might 
				fail should return a BOOL indicating success or failure. Thus, retvalue will
				usually be FALSE. However, functions which return pointers can indicate error
				conditions by returning NULL, so the retvalue parameter is there to give extra
				functionality. Obviously, TRUE or FALSE (or NULL) tells us very little about why
				a function failed, so we have a class containing static variables which must be
				set each time a function fails. A calling routine can look at the error value to
				see what the error was, or use the string to present the error in an error box.
				The macro has two incarnations - within module code in a separate DLL it passes a
				Module ID, which allows the string finding functions to look in another resource, 
				while within kernel code, it just passes the error ID.
				It looks at the symbol MODULE_ID to see if it is being defined within a module,
				and defines the appropriate behaviour. MODULE_ID should be defined as a unique
				module identifier in all code running in a module. It should not be defined in any
				other code.
				Warning: Windows.h defines ERROR as 0 for region functions. Use of this together 
				with errors.h should be treated carefully. 
	SeeAlso:	TOOLERROR; ERRORIF; Error

***********************************************************************************************/

/*********************************************************************************************

>	macro TOOLERROR(UINT32 ErrID, retvalue, toolID)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/93
	Purpose:	Like ERROR, except specially for Tools to use.
				If you are a Tool you can use the TOOLERROR macro which takes a third parameter,
				that of your tool ID. MODULE_ID does not need to be defined for this to work.				
	SeeAlso:	ERROR; TOOLERRORIF; Error

*********************************************************************************************/

#ifdef ERROR
#undef ERROR 
#endif

#ifndef MODULE_ID

#define ERROR(errID, retparam) \
Error::SetError( errID, 0 ); \
return retparam;

#else

#define ERROR(errID, retparam) \
Error::SetError(errID, MODULE_ID); \
return retparam;

#endif

#define	TOOLERROR(errID, retparam, toolid) \
Error::SetErrorTool(errID, toolid); \
return retparam;



/***********************************************************************************************

>	macro ERRORIF(condition, UINT32 ErrID, retvalue)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		condition is a boolean expression, TRUE if error should be returned.
				ErrID is an error number/resource (all the same)
				retvalue is the value that will be returned from the function.
	Purpose:	This is a general purpose macro which is a bit like ENSURE, except that it really
				does return an error to the calling code.
				This macro should be used at the beginning of a member function, to test for
				the known conditions that the function works to. All functions should know what 
				conditions they expect before they start, and they should fail if those conditions 
				are not met. The condition should be a boolean expression which should evaluate
				to TRUE if an error condition has manifested itself, at which time the ERROR macro 
				is executed, making the function return whatever you put in retvalue.
				Usually, retvalue will be FALSE, since most functions that fail are expected to
				return a BOOL. However, some functions can usefully return other error indicators.
				For example, a function which returns a pointer can return NULL to indicate an error.
				Thus, you can use the macro as:
				
					ERRORIF(num != 42, _R(ID_BADPARAM), FALSE);
				or
					ERRORIF(!ptr, _R(ID_BADPOINTER), NULL);
					
	SeeAlso:	ERROR; Error

***********************************************************************************************/

/*********************************************************************************************

>	macro TOOLERRORIF(condition, UINT32 ErrID, retvalue, toolID)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/8/93
	Purpose:	Like ERRORIF, except specially for Tools to use.
				If you are a Tool you can use the TOOLERROR macro which takes a third parameter,
				that of your tool ID. MODULE_ID does not need to be defined for this to work.				
	SeeAlso:	ERRORIF; TOOLERROR; Error

*********************************************************************************************/


#define ERRORIF(cond,returnID, retvalue)\
	if (cond) 							\
	{									\
		ERROR( returnID, retvalue );	\
	}


#define	TOOLERRORIF(cond,returnID,retvalue,toolid)	\
	if (cond)										\
	{												\
		TOOLERROR((UINT32)returnID, retvalue,toolid);	\
	}


// INLINE_MARKERROR non-zero if MarkError function is inline
// this is done in retail builds, except in DLL code
#if defined(_DEBUG) || defined(_AFXDLL)
#define	INLINE_MARKERROR	0
#else
#define	INLINE_MARKERROR	1
#endif
// MARKERROR_BODY is used to expand the actual function as required
#define	MARKERROR_BODY(Line,File)	{ LastErrorLine = Line; LastErrorFile = File; }

#define MAXERRORFORMATLENGTH 256

/***********************************************************************************************

>	class Error

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Purpose:	The Error class consists only of static variables, one for the ID of the error,
				another for the string, and another for the ID of the module that generated the 
				error (0 if the error happened in the kernel). The string is a simple TCHAR array
				to avoid the need for dynamic memory allocation during error handling. These 
				variables must be set whenever an error condition manifests itself. The class 
				provides static functions to do this, which are called by the ERRORIF and 
				NERROR macros
	SeeAlso:	ERRORIF; ERROR

***********************************************************************************************/

class CCAPI Error
{
private:
	static UINT32 ErrorID;

	// Chris introduced RalphErrorID during ralph error handling
	// ralph needs this so that he can map the ID to a HRESULT before passing it
	// back to a harness - the current ErrorId is cleared in SetSeriousError maybe by design
	// To be safe we'll keep our own copy.

	static UINT32 RalphErrorID;
	static UINT32 ModuleID;
	static TCHAR ErrorString[256];
	static UINT32	RenderThread;			// This is incremented each time we start something that looks
										// like a render, and decremented as we thread out. So if an error
										// occurs, we know we have to be very careful

	static UINT32 LastErrorLine;			// line number of last error (or 0)
	static const char *LastErrorFile;	// ptr to filename of last error

	static wxString UserName;

	static void TraceWrite(const TCHAR * buf, va_list args);
	static void FixFormat (const TCHAR * fmt, TCHAR * fmt2);
#if 0 != wxUSE_UNICODE
	// In UNICODE builds ONLY we allow an additional char * version of which copes with old code
	// that does ERROR3PF("foo", ...), not ERROR3PF(_T("foo", ....). We don't include these
	// in the non-Unicode version as they clash with the TCHAR stuff
	static void FixFormat (const char * fmt, TCHAR * fmt2);
#endif

public:
	Error()
	{
		ErrorString[0] = 0;
		RalphErrorID =ErrorID = ModuleID = 0;
	}

	static BOOL IsUserName(const char *wanted);

	static void SetUserName(wxString User) { UserName = User; }
	
	// convert ID's to HRESULT for ralph harness's
	static HRESULT ErrIDToHRESULT(UINT32 ErrID);

	static HRESULT GetRalphError();

	~Error();
	static void SetError(UINT32 number, const TCHAR* errstring, UINT32 module);
	static void SetError(UINT32 number, UINT32 module = 0);
	static void SetErrorTool(UINT32 number, UINT32 toolid);
	static void SetErrorSerious( const TCHAR* );

	static void ClearError();

	inline static TCHAR* GetErrorString();
	
	inline static UINT32 GetErrorNumber();
	inline static UINT32 GetRalphErrorNumber();

	inline static UINT32 GetErrorModule();

	inline static void RenderThreadIn() { RenderThread++; }
	inline static void RenderThreadOut() { RenderThread--; }
	inline static void RenderThreadReset() { RenderThread = 0; }
	inline static BOOL IsInRenderThread() { return ( RenderThread != 0 ); }

	static void DirectStatus( BOOL = FALSE );

	static void MarkError( UINT32 LineNumber, const char *Filename )
	#if INLINE_MARKERROR
		MARKERROR_BODY( LineNumber, Filename )
	#else
		;
	#endif


	static void CDECL XSetErrorC();
	static void CDECL XSetError(const TCHAR *fmt, ...);
#if 0 != wxUSE_UNICODE
	// In UNICODE builds ONLY we allow an additional char * version of which copes with old code
	// that does ERROR3PF("foo", ...), not ERROR3PF(_T("foo", ....). We don't include these
	// in the non-Unicode version as they clash with the TCHAR stuff
	static void CDECL XSetError(const char *fmt, ...);
#endif
	static void CDECL XSetError(UINT32, ...);
	static void CDECL ReleaseTrace(LPCTSTR, ...);
	#ifdef _DEBUG
	static void CDECL XComplain(const TCHAR *fmt, ...);
#if 0 != wxUSE_UNICODE
	// In UNICODE builds ONLY we allow an additional char * version of which copes with old code
	// that does ERROR3PF("foo", ...), not ERROR3PF(_T("foo", ....). We don't include these
	// in the non-Unicode version as they clash with the TCHAR stuff
	static void CDECL XComplain(const char *fmt, ...);
#endif
	static void CDECL TraceUser(const char *, LPCTSTR, ...);
	static void CDECL TraceAll( LPCTSTR, ...);
	static void CDECL TraceTime (TCHAR * t);
	#else
	static void CDECL TraceUser(const char *, LPCTSTR, ...) { }
	static void CDECL TraceAll( LPCTSTR, ...) { }
	static void CDECL TraceTime (TCHAR *) { }
	#endif

	// Stack walking stuff
public:
	static void DumpStack(UINT32 frames=0);

// FreeBSD does not provide backtrace()
#if !defined(__WXMAC__) && !defined(__FreeBSD__)
	class StackWalker : public wxStackWalker
	{
		public:
			virtual void OnStackFrame(const wxStackFrame & frame);
	};
#endif

};

/***********************************************************************************************

>	void Error::RenderThreadIn()
	void Error::RenderThreadOut()
	void Error::RenderThreadReset()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Notes the threading in or out of rendering (i.e. whether the program is somewhere
				within a render loop). Each RenderThreadIn should be paired with a RenderThreadOut
				and placed around rendering code. They may be nested. RenderThreadReset notes an
				abnormal termination of the thread (for instance an exception). Whilst in render
				threads, errors are handled slightly differently. Hence the need to know.
	Errors:		-
	SeeAlso:	Error::IsInRenderThread

***********************************************************************************************/

/***********************************************************************************************

>	BOOL Error::IsInRenderThread()
   
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/2/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if in a render thread, else FALSE
	Purpose:	Tells the caller whether or not Camelot is in a render thread. If so, if an error
				occurs, it is likely to be (a) nasty, and (b) repeated many times, so more forceful
				action is taken to get rid of it.
	Errors:		-
	SeeAlso:	Error::RenderThreadIn, Error::RenderThreadOut, Error::RenderThreadReset

***********************************************************************************************/

/***********************************************************************************************

>	TCHAR* Error::GetErrorString()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	pointer to TCHAR array containing last error
	Purpose:	Gets the string associated with the last error from the static error variables.
				Usually used by an operation to report why the operation failed.
	Errors:		-
	SeeAlso:	Error::GetErrorNumber;Error::GetErrorModule; Error::SetError

***********************************************************************************************/

TCHAR* Error::GetErrorString()
{
	return ErrorString;
}              

/***********************************************************************************************

>	UINT32 Error::GetRalphErrorNumber()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/96
	Inputs:		-
	Outputs:	-
	Returns:	Error number of last error that occurred
	Purpose:	As GetERrorNumber EXCEPT this returns the ralph copy of ErrorID that is not 
				cleared in SetSeriousError()
	Errors:		-
	SeeAlso:	Error::GetErrorString; Error::GetErrorModule; Error::SetError

***********************************************************************************************/

UINT32 Error::GetRalphErrorNumber()
{
	// if ErrorId is zero (it may have been cleared in SetSeriousError )
	// return RalphErrorID
	if(	ErrorID == 0)
		return RalphErrorID;
	else
		return ErrorID;
} 	

/***********************************************************************************************

>	UINT32 Error::GetErrorNumber()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	Error number of last error that occurred
	Purpose:	If a function returns failure, this function will read the error number.
				This is usually associated with a resource of some kind - in Win16 it will
				be the 16bit resource ID, in RISC OS it will be a 32bit resource ID, etc.
				There is always an associated string (reached by GetErrorString) so routines
				wanting to report the error do not need to do any nasty resource loading.
	Errors:		-
	SeeAlso:	Error::GetErrorString; Error::GetErrorModule; Error::SetError

***********************************************************************************************/

UINT32 Error::GetErrorNumber()
{
	return ErrorID;
}                     


	
/***********************************************************************************************

>	UINT32 Error::GetErrorModule()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/93
	Inputs:		-
	Outputs:	-
	Returns:	Unique UINT32 identifying the module that generated the last error
				(zero if the last error was in the kernel).
	Purpose:	Mainly for debugging - the UINT32 is a unique identifier which will tell a
				programmer which module generated the last error. This will never be visible 
				to a user.
	Errors:		-
	SeeAlso:	Error::GetErrorNumber; Error::GetErrorString; Error::SetError

***********************************************************************************************/

UINT32 Error::GetErrorModule()
{
	return ModuleID;
}

// all-new error macros

#define	MARKWHERE	Error::MarkError( __LINE__, __FILE__ )
#ifdef _DEBUG
#define	MARKWHEREDB	MARKWHERE
#else
#define	MARKWHEREDB
#endif

#define	ERROR1RAW( errorID )						do { MARKWHEREDB; Error::XSetError( errorID ); } while(0)
#define	ERROR1( retvalue, errorID )					do { MARKWHEREDB; Error::XSetError( errorID ); return (retvalue); } while(0)
#define	ERROR1IF( condition, retvalue, errorID )	do { if (condition) ERROR1( retvalue, errorID ); } while(0)
#define	ERROR1_MSG( retvalue, args )				do { MARKWHEREDB; Error::XSetError args ; return (retvalue); } while(0)
#define	ERROR1IF_MSG( condition, retvalue, args )	do { if (condition) ERROR1_MSG( retvalue, args ); } while(0)

#ifdef _DEBUG
#define	ERROR2RAW( literal )						do { MARKWHERE; Error::XSetError( literal ); } while(0)
#define	ERROR2( retvalue, literal )					do { MARKWHERE; Error::XSetError( literal ); return (retvalue); } while(0)
#define	ERROR2IF( condition, retvalue, literal )	do { if (condition) ERROR2( retvalue, literal ); } while (0)
#define	ERROR2_PF( retvalue, args )					do { MARKWHERE; Error::XSetError args ; return (retvalue); } while(0)
#define	ERROR2IF_PF( condition, retvalue, args )	do { if (condition) ERROR2_PF( retvalue, args ); } while (0)

#define	ERROR3(literal)								do { MARKWHERE; Error::XComplain( literal ); } while(0)
#define	ERROR3IF(condition, literal)				do { if (condition) ERROR3(literal); } while(0)
#define	ERROR3_PF(args)								do { MARKWHERE; Error::XComplain args; } while(0)
#define	ERROR3IF_PF(condition, args)				do { if (condition) ERROR3_PF(args); } while(0)
#define	TRACEUSER									Error::TraceUser
#define	TRACEALL									TRACE
#define RELTRACE									Error::ReleaseTrace
#define TRACE										Error::TraceAll
#define TRACET										Error::TraceTime
#define TRACE0										Error::TraceAll
#define ASSERT										wxASSERT

#else // _DEBUG

#define	ERROR2RAW( literal )						do { MARKWHERE; Error::XSetErrorC(); } while(0)
#define	ERROR2( retvalue, literal )					do { MARKWHERE; Error::XSetErrorC(); return (retvalue); } while(0)
#define	ERROR2IF( condition, retvalue, literal )	do { if (condition) ERROR2( retvalue, literal ); } while(0)
#define	ERROR2_PF( retvalue, args )					do { MARKWHERE; Error::XSetError args ; return (retvalue); } while(0)
#define	ERROR2IF_PF( condition, retvalue, args )	do { if (condition) ERROR2_PF( retvalue, args ); } while(0)

#define	ERROR3(literal)								do { } while (0)
#define	ERROR3IF(condition, literal)				do { } while (0)
#define	ERROR3_PF(args)								do { } while (0)
#define	ERROR3IF_PF(condition, args)				do { } while (0)
#define TRACEUSER									1 ? (void)0 : Error::TraceUser
#define	TRACEALL									TRACE
#define RELTRACE									Error::ReleaseTrace

#define TRACE										1 ? (void)0 : Error::TraceAll
#define TRACE0										1 ? (void)0 : Error::TraceAll
#define TRACET										1 ? (void)0 : Error::TraceAll
#define ASSERT(condition)							do { } while (0)

#endif // _DEBUG



#if defined(_DEBUG) && defined(SHOWPORTNOTE) && defined(EXCLUDE_FROM_XARALX)
#ifdef __GNUG__
#define PORTNOTE(section, text)
//The following is what we want to do, but it doesn't work
//#define PORTNOTE(section, text) #warning [PORTNOTE] section ": " text
#else // __GNUG__
#define PORTNOTE(section, text)
//The following is what we want to do, but it doesn't work
//#define PORTNOTE(section, text) #pragma message( __LOCMSG__ section ": " text )
#endif // __GNUG__
#define PORTNOTETRACE(section, text) \
PORTNOTE(section,text) \
TRACE(_T("PORTNOTE[") _T(section) _T("]: ") _T(text))
#else //_DEBUG, SHOWPORTNOTE
#define PORTNOTE(section, text)
#define PORTNOTETRACE(section,text)
#endif //_DEBUG, SHOWPORTNOTE


#endif	//INC_ERRORS
