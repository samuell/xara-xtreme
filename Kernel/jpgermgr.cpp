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
// 


#include "camtypes.h"

namespace JPEG
{
	extern "C"
	{
	#include "jpeglib.h"
	}

#if defined(HAVE_BOOLEAN)
	typedef ::boolean	boolean;
#endif
};
#include "errors.h"
//#include "filtrres.h"		// for Error strings

#include "jpgermgr.h"

// Place any IMPLEMENT type statements here
//CC_IMPLEMENT_MEMDUMP(CCWobJob, CC_CLASS_MEMDUMP)


// We want better memory tracking
#define new CAM_DEBUG_NEW


// Build a list of error messages for trace purposes
#ifdef _DEBUG
#define JMESSAGE(code,string,errorclass)	string ,

const char* const MessageTable[] =
{
#include "jerror.h"
  NULL
};
#else
const char* const MessageTable[] =
{
	NULL
};
#endif

// Build a list of error classes
#define JMESSAGE(code,string,errorclass)	errorclass ,

const JPEGErrorManager::ERROR_CLASS	ErrorClasses[] =
{
#include "jerror.h"
	JPEGErrorManager::ERR_NONE			// to terminate the enum
};
	

// Functions follow


/********************************************************************************************

>	METHODDEF(void) format_message (j_common_ptr cinfo, char * buffer)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (copied from IJG library)
	Created:	02/08/96
	Purpose:	For formatting TRACE strings
	Notes:		From IJG library:
				Format a message string for the most recent JPEG error or message.
				The message is stored into buffer, which should be at least JMSG_LENGTH_MAX
				characters.  Note that no '\n' character is added to the string.
				Few applications should need to override this method.

********************************************************************************************/
METHODDEF(void) format_message (JPEG::j_common_ptr cinfo, char * buffer)
{
  struct JPEG::jpeg_error_mgr * err = cinfo->err;
  INT32 msg_code = err->msg_code;
  const char * msgtext = NULL;
  const char * msgptr;
  char ch;
  JPEG::boolean isstring;

  // Look up message string in proper table 
  if (msg_code > 0 && msg_code <= err->last_jpeg_message) {
    msgtext = err->jpeg_message_table[msg_code];
  } else if (err->addon_message_table != NULL &&
	     msg_code >= err->first_addon_message &&
	     msg_code <= err->last_addon_message) {
    msgtext = err->addon_message_table[msg_code - err->first_addon_message];
  }

  // Defend against bogus message number 
  if (msgtext == NULL) {
    err->msg_parm.i[0] = msg_code;
    msgtext = err->jpeg_message_table[0];
  }

  // Check for string parameter, as indicated by %s in the message text 
  isstring = FALSE;
  msgptr = msgtext;
  while ((ch = *msgptr++) != '\0') {
    if (ch == '%') {
      if (*msgptr == 's') isstring = TRUE;
      break;
    }
  }

  // Format the message into the passed buffer 
  if (isstring)
    sprintf(buffer, msgtext, err->msg_parm.s);
  else
    sprintf(buffer, msgtext,
	    err->msg_parm.i[0], err->msg_parm.i[1],
	    err->msg_parm.i[2], err->msg_parm.i[3],
	    err->msg_parm.i[4], err->msg_parm.i[5],
	    err->msg_parm.i[6], err->msg_parm.i[7]);
}


/********************************************************************************************

>	JPEGErrorManager::JPEGErrorManager()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Default constructor for the JPEGErrorManager providing callbacks for
				error handling in the IJG JPEG Library.
				By default no exceptions will be thrown.

********************************************************************************************/
JPEGErrorManager::JPEGErrorManager()
{
	error_exit		= ErrorExit;
	emit_message	= EmitMessage;
	output_message	= OutputMessage;
	format_message	= ::format_message;
	reset_error_mgr	= ResetErrorManager;

	trace_level		= 0;					/* default = no tracing */
	num_warnings	= 0;					/* no warnings emitted yet */
	msg_code		= 0;		// may be useful as a flag for "no error" */

	/* Initialize message table pointers */
	jpeg_message_table	= ::MessageTable;
	last_jpeg_message	= (INT32) JMSG_LASTMSGCODE - 1;

	addon_message_table	= NULL;
	first_addon_message	= 0;	/* for safety */
	last_addon_message	= 0;

//	ERROR_CLASS m_ErrorClass = ERR_NONE;
}



/********************************************************************************************

>	J_MESSAGE_CODE JPEGErrorManager::GetLastError() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Returns:	Retrieves the last error code
	Errors:		ERROR3 if invalid last error
	Purpose:	Used internally for easy access

********************************************************************************************/
J_MESSAGE_CODE JPEGErrorManager::GetLastError() const
{
	if (msg_code <= 0 || msg_code > last_jpeg_message)
	{
		ERROR3(jpeg_message_table[JMSG_NOMESSAGE]);
		return JMSG_NOMESSAGE;
	}

	return (J_MESSAGE_CODE) msg_code;
}


/********************************************************************************************

>	StringID JPEGErrorManager::GetStringIDForError() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Returns:	A resource StringID for the last error thrown.
	Purpose:	Allows class users to obtain a resource string id suitable for reporting to
				the user (usually via Error::SetError(...))

********************************************************************************************/
StringID JPEGErrorManager::GetStringIDForError() const
{
	ERROR_CLASS lastClass = GetLastErrorClass();

	if (lastClass == ERR_STANDARD)
	{
		return m_MessageID;
	}
	else
	{
		switch (lastClass)
		{
			case ERR_NONE:			return (StringID)-1; break;
			case ERR_STANDARD: 		return 0; break; // should never happen
			case ERR_TRACE: 		return 1; break; // what's this for?
			case ERR_STRUCTURE:		return(_R(IDS_JPEG_ERROR_STRUCTURE)); break;
			case ERR_FORMAT:		return(_R(IDS_JPEG_ERROR_FORMAT)); break;
			case ERR_LIMITS:		return(_R(IDS_JPEG_ERROR_LIMITS)); break;
			case ERR_UNSUPPORTED:	return(_R(IDS_JPEG_ERROR_UNSUPPORTED)); break;
			default:				return(_R(IDS_JPEG_ERROR_INTERNAL));
		}
	}
}


/********************************************************************************************

>	void JPEGErrorManager::ThrowError(StringID UserErrorMessage)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Inputs:		UserErrorMessage : A resource ID for a string to be used when reporting
				this error to the user.
	Purpose:	Classes external to the IJG Library should use this entry point to throw
				exceptions.

********************************************************************************************/
void JPEGErrorManager::ThrowError(StringID UserErrorMessage)
{
	m_ErrorClass	= ERR_STANDARD;
	m_MessageID		= UserErrorMessage;

	throw 0;
}


/********************************************************************************************

>	JPEGErrorManager::ERROR_CLASS JPEGErrorManager::GetLastErrorClass() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Returns:	The error class of the last error thrown via a call to ErrorExit
				or ThrowError
	Errors:		ERROR3 if class is ERR_NONE or ERR_TRACE
	Purpose:	Used internally to provide easy access to m_ErrorClass

********************************************************************************************/
JPEGErrorManager::ERROR_CLASS JPEGErrorManager::GetLastErrorClass() const
{
	if (m_ErrorClass == ERR_NONE || m_ErrorClass == ERR_TRACE)
	{
		ERROR3("JPEGErrorManager::GetLastErrorClass() - Trying to get dummy error class");
		return ERR_INTERNAL;
	}
	return m_ErrorClass;
}


/********************************************************************************************

>	void JPEGErrorManager::ErrorExit(j_common_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	The IJG library requires a callback to exit once an error has occurred.
				This is that callback. It throws an exception.

********************************************************************************************/
void JPEGErrorManager::ErrorExit(JPEG::j_common_ptr cinfo)
{
	JPEGErrorManager* pThis = (JPEGErrorManager*)cinfo->err;

	J_MESSAGE_CODE msg = pThis->GetLastError();

	pThis->m_ErrorClass = ErrorClasses[msg];

#ifdef _DEBUG
	EmitMessage(cinfo, -1);
#endif

	throw 0;
}


/********************************************************************************************

>	void JPEGImportFilter::OutputMessage(j_common_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/96
	Purpose:	Actual output of an error or trace message.
				Overrides standard implementation of sending errors to stderr.

********************************************************************************************/
void JPEGErrorManager::OutputMessage(JPEG::j_common_ptr cinfo)
{
  char buffer[JMSG_LENGTH_MAX];

  /* Create the message */
  (*cinfo->err->format_message) (cinfo, buffer);

  /* Send it to stderr, adding a newline */
  TRACE( _T("%s\n"), buffer );
}


/********************************************************************************************

>	void JPEGImportFilter::EmitMessage(j_common_ptr cinfo, INT32 msg_level)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (based on IJG emit_message)
	Created:	02/08/96
	Purpose:	Decide whether to emit a trace or warning message.

	Notes:		msg_level is one of:

				-1: recoverable corrupt-data warning, may want to abort.
				0: important advisory messages (always display to user).
				1: first level of tracing detail.
				2,3,...: successively more detailed tracing messages.

********************************************************************************************/
void JPEGErrorManager::EmitMessage(JPEG::j_common_ptr cinfo, INT32 msg_level)
{
	JPEGErrorManager* pThis = (JPEGErrorManager*)cinfo->err;

	if (msg_level < 0)
	{
		/*
		* It's a warning message.  Since corrupt files may generate many warnings,
		* the policy implemented here is to show only the first warning,
		* unless trace_level >= 3.
		*/
		if (pThis->num_warnings == 0 || pThis->trace_level >= 3)
		{
			(*pThis->output_message) (cinfo);
		}
		// Always count warnings in num_warnings.
		pThis->num_warnings++;
	}
	else
	{
		// It's a trace message.  Show it if trace_level >= msg_level.
		if (pThis->trace_level >= msg_level)
		{
			(*pThis->output_message) (cinfo);
		}
	}
}



/********************************************************************************************

>	void JPEGErrorManager::ResetErrorManager(j_common_ptr cinfo)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (based on IJG emit_message)
	Created:	02/08/96
	Purpose:	Resets the the error manager prior to starting the filter proper

********************************************************************************************/
void JPEGErrorManager::ResetErrorManager(JPEG::j_common_ptr cinfo)
{
	JPEGErrorManager* pThis = (JPEGErrorManager*)cinfo->err;

	pThis->num_warnings = 0;
	/* trace_level is not reset since it is an application-supplied parameter */
	pThis->msg_code = 0;	/* may be useful as a flag for "no error" */
}


