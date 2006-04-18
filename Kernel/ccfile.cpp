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

/********************************************************************************************
	
	CCFile.cpp	- Contains the function declarations of the Kernel CCFile derived classes.
	These include: CCDiskFile, CCMemFile and CCMemTextFile. 

********************************************************************************************/


#include "camtypes.h"

//#include "costream.h"
#include "ccfile.h"
#include "zstream.h"
// WEBSTER-Martin-29/12/96 no accusoft stuff
#ifndef WEBSTER
//#include "extfilts.h"
#endif //WEBSTER

#include <errno.h>
#include "ensure.h"
#include "errors.h"

//#include "mario.h"
//#include "nev.h"
//#include "tim.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(CCFile, CCObject);
CC_IMPLEMENT_DYNAMIC(CCLexFile, CCFile);
CC_IMPLEMENT_DYNAMIC(CCStreamFile, CCLexFile);
CC_IMPLEMENT_DYNAMIC(CCDiskFile, CCStreamFile);
CC_IMPLEMENT_DYNAMIC(CCMemFile, CCLexFile);
CC_IMPLEMENT_DYNAMIC(CCMemTextFile, CCMemFile);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	CCFile::CCFile(BOOL ReportErrors, BOOL ThrowExceptions)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/94
	Inputs:		Flags to specify whether reporting errors or throwing exceptions.
				Defaults specified to backward compatible. Set to report errors but not to
				throw exceptions.
	Outputs:	None
	Returns:	None
	Purpose:	Default CCFile class constructor. Does nothing but set the reporting flags.
	Errors:		None	

********************************************************************************************/

CCFile::CCFile(BOOL ErrorReporting, BOOL ExceptionThrowing)
{
	// Set our class variables from the passed in values
	ReportErrors = ErrorReporting;
	ThrowExceptions = ExceptionThrowing;

	// Default to no compression
	CompressData = FALSE;
	// Default to no GZipErrorID
	GZipErrorID = 0;
}

/********************************************************************************************

>	BOOL CCFile::IsCompressionSet( )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/94
	Inputs:		None
	Outputs:	None
	Returns:	The state of the compression flag, TRUE if compression is on.
	Purpose:	Allow the reading of the compression flag to see if all output data is
				compressed or not.

********************************************************************************************/

BOOL CCFile::IsCompressionSet( )
{
	return CompressData;
}	

/********************************************************************************************

>	BOOL CCFile::SetCompression( BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/94
	Inputs:		TRUE if want compression, false otherhwise.
	Outputs:	None
	Returns:	True, if it was successful, False otherwise.
	Purpose:	Allow the setting of the compression flag so that all output data is
				compressed.

********************************************************************************************/

BOOL CCFile::SetCompression( BOOL NewState)
{
//	BOOL OldState = CompressData;

	CompressData = NewState;

	BOOL ok = TRUE;
	
	// If turning compression On then start the compression up
	// else turn it off.
	if (NewState)
	{
		// Get the class itself to start the compression process up
		ok = StartCompression();
		if (!ok)
		{
			CompressData = FALSE;	
		}
	}
	else
	{
		// Get the class itself to start the compression process up
		ok = StopCompression();
	}

	return ok;	
}

/********************************************************************************************

>	CCFile::GotError( UNIT errorID )
>	CCFile::GotError( UNIT errorID , const TCHAR*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/94
	Inputs:		Unsigned interger which is the error id and possible error message string
	Outputs:	None
	Returns:	None
	Purpose:	Error handler for the CCFile class. It uses flags set in the constructor
				to determine if the user requires errors to be reported or exceptions
				thrown. 
	Errors:		None
	SeeAlso:	SetReportErrors; SetThrowExceptions

********************************************************************************************/

void CCFile::GotError( UINT32 errorID , const TCHAR* errorString)
{
	ERROR3IF(errorString == 0, "errorString is null. Oh dear.");

	Error::SetError(errorID, errorString, 0);

	// Flag that no more reads/writes should happen by forcing bad() to return True/.
	// So that if the user continues to access the file then they should get an error.
	SetBadState();
	
	// If the ReportErrors flag is set then report the error now.
	if (ReportErrors)
		InformError();

	// If the ThrowExceptions flag is set then throw an exception.
	// Use the CFileException with a generic or unknown error and make the ioserror
	// our error number.
	if (ThrowExceptions)
	{	
		TRACE( _T("CCFile GotError call to throw exception\n") );
	
		throw( CFileException( errorID ) );
//		AfxThrowFileException(CFileException::generic, errorID);
	}
}

void CCFile::GotError( UINT32 errorID )
{
	// Expand certain types of error messages to make them more informative
	if ((errorID == _R(IDE_OPEN_ERROR)) ||
		(errorID == _R(IDE_FILE_READ_ERROR)) ||
		(errorID == _R(IDE_FILE_WRITE_ERROR)) )
		// Make the error message contain handy information.
		SetFileErrorMessage(errorID);
	else if ((errorID == _R(IDE_ZOPEN_ERROR) ) ||
		(errorID == _R(IDE_ZFILE_READ_ERROR)) ||
		(errorID == _R(IDE_ZFILE_WRITE_ERROR)) )
		// Make the error message contain handy information.
		SetZipFileErrorMessage(errorID);
	else
		Error::SetError(errorID, 0);

	// Flag that no more reads/writes should happen by forcing bad() to return True.
	// So that if the user continues to access the file then they should get an error.
	SetBadState();
	
	// If the ReportErrors flag is set then report the error now.
	if (ReportErrors)
		InformError();
	
	// If the ThrowExceptions flag is set then throw an exception.
	// Use the CFileException with a generic or unknown error and make the ioserror
	// our error number.
	if (ThrowExceptions)
	{	
		TRACE( _T("CCFile GotError call to throw exception\n") );
		//TRACEUSER( "Chris", _T("EXCEPTION!!!!!"));
		//AfxDebugBreak();
		throw( CFileException( errorID ) );
	}
}



/********************************************************************************************

>	BOOL CCFile::SetFileErrorMessage(UINT32 ErrorID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/11/94
	Inputs:		ErrorID - the ID of the string resource to use as the error message.
	Returns:	TRUE if the error message was constructed ok;
				FALSE if not (e.g. string resource did not load).
	Purpose:	Looks at the 'errno' variable to determine what kind of error occured, and
				uses the specified error resource string to make a helpful error message
				out of it.
				NB. The string must have two string fields, to be replaced as follows:
					#1%s  =  description of error
					#2%s  =  name of the file that went wrong.

				An example is _R(IDE_FILE_READ_ERROR):

					"A read error occured (#1%s) with the file: #2%s"

	SeeAlso:	CCFile::GotError

********************************************************************************************/

BOOL CCFile::SetFileErrorMessage(UINT32 ErrorID)
{
	UINT32 FileErrorID;

	// Work out which error description string to use...
	switch (errno)
	{
		case EACCES:
			// Permission denied
			FileErrorID = _R(IDE_FILE_PERM_DENIED);
			break;

		case EBADF:
			// Bad file number
			FileErrorID = _R(IDE_FILE_BAD_FILE);
			break;

		case EEXIST:
			// File already exists
			FileErrorID = _R(IDE_FILE_EXISTS);
			break;

		case EMFILE:
			// Too many open files
			FileErrorID = _R(IDE_FILE_TOO_MANY_OPEN);
			break;

		case ENOENT:
			// No such file or directory
			FileErrorID = _R(IDE_FILE_NO_EXIST);
			break;

		case ENOSPC:
			// No space left on device
			FileErrorID = _R(IDE_FILE_NO_SPACE);
			break;

		default:
			// Unknown error
			FileErrorID = _R(IDE_FILE_UNKNOWN);
	}

	// Load in this resource
	String_64 FileError;

	if (!FileError.Load(FileErrorID))
	{
		// Error occured - just use the error number
		if( FileError._MakeMsg( _T("#1%d"), errno ) == 0 )
			// Couldn't do this (we must be severely shafted by now!) - return error
			return FALSE;
	}

	// Get a shortened name for the file
	String_64 Filename;

	if (!GetName(&Filename))
	{
		// Unable to get a name - substitute with "unknown"
		if (!Filename.Load(_R(IDE_FILE_UNKNOWN)))
		{
			// Error occured - just use the hard-coded literal "unknown"
			// (This was sanctioned by Phil)
			Filename = TEXT("unknown");
		}
	}

	// Use this information to build an error string and pass it to the SetError() function.
	String_256 ErrorMsg;

	if (ErrorMsg.MakeMsg(ErrorID, (LPCTSTR) FileError, (LPCTSTR) Filename) == 0)
		// Error occured - maybe resource would not load...not much we can do now.
		return FALSE;

	Error::SetError(ErrorID, (TCHAR *) ErrorMsg, 0);

	// Error string was constructed ok
	return TRUE;
}

/********************************************************************************************

>	BOOL CCFile::SetZipFileErrorMessage(UINT32 ErrorID)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		ErrorID - the ID of the string resource to use as the error message.
	Returns:	TRUE if the error message was constructed ok;
				FALSE if not (e.g. string resource did not load).
	Purpose:	Looks at the 'GZipErrorID' variable to determine what kind of error occured, and
				uses the specified error resource string to make a helpful error message
				out of it.
				NB. The string must have two string fields, to be replaced as follows:
					#1%s  =  description of error
					#2%s  =  name of the file that went wrong.

				An example is _R(IDE_FILE_READ_ERROR):

					"A read error occured (#1%s) with the file: #2%s"

				Exactly the same as above but is designed for zip file errors.

	SeeAlso:	SetFileErrorMessage; CCFile::GotError

********************************************************************************************/

BOOL CCFile::SetZipFileErrorMessage(UINT32 ErrorID)
{
	UINT32 ZipFileErrorID;

	// Work out which error description string to use...
	switch (GZipErrorID)
	{
		case Z_ERRNO:
			// Bad file number
			ZipFileErrorID = _R(IDE_ZFILE_ERRORNO);
			break;

		case Z_STREAM_ERROR:
			// Permission denied
			ZipFileErrorID = _R(IDE_ZFILE_STREAMERROR);
			break;

		case Z_DATA_ERROR:
			// Bad file number
			ZipFileErrorID = _R(IDE_ZFILE_DATAERROR);
			break;

		case Z_MEM_ERROR:
			// Bad file number
			ZipFileErrorID = _R(IDE_ZFILE_MEMORYERROR);
			break;

		case Z_BUF_ERROR:
			// Bad file number
			ZipFileErrorID = _R(IDE_ZFILE_BUFFERERROR);
			break;

		default:
			// Unknown error
			ZipFileErrorID = _R(IDE_ZFILE_BASEEERROR);
	}

	// Load in this resource
	String_64 FileError;

	if (!FileError.Load(ZipFileErrorID))
	{
		// Error occured - just use the error number
		if( FileError._MakeMsg( _T("#1%d"), errno ) == 0 )
			// Couldn't do this (we must be severely shafted by now!) - return error
			return FALSE;
	}

	// Get a shortened name for the file
	String_32 Filename;

	if (!GetName(&Filename))
	{
		// Unable to get a name - substitute with "unknown"
		if (!Filename.Load(_R(IDE_FILE_UNKNOWN)))
		{
			// Error occured - just use the hard-coded literal "unknown"
			// (This was sanctioned by Phil)
			Filename = TEXT("unknown");
		}
	}

	// Use this information to build an error string and pass it to the SetError() function.
	String_256 ErrorMsg;

	if (ErrorMsg.MakeMsg(ErrorID, (LPCTSTR) FileError, (LPCTSTR) Filename) == 0)
		// Error occured - maybe resource would not load...not much we can do now.
		return FALSE;

	Error::SetError(ErrorID, (TCHAR *) ErrorMsg, 0);

	// Error string was constructed ok
	return TRUE;
}

/********************************************************************************************

>	BOOL CCFile::SetReportErrors( BOOL )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/94
	Inputs:		BOOL which is the TRUE/FALSE state required.
	Outputs:	None
	Returns:	Old state of the flag
	Purpose:	Public way of setting the internal error handling flag to say whether
				errors are going to be reported or not.
	Errors:		None
	SeeAlso:	GotError; IsReportingSet; IsThrowingSet		

********************************************************************************************/
BOOL CCFile::SetReportErrors( BOOL newState )
{
	BOOL OldState = ReportErrors;

	ReportErrors = newState;

	return OldState;
}

/********************************************************************************************

>	BOOL CCFile::SetThrowExceptions( BOOL )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/94
	Inputs:		BOOL which is the TRUE/FALSE state required.
	Outputs:	None
	Returns:	Old state of the flag
	Purpose:	Public way of setting the internal error handling flag to say whether
				errors are going to throw an exception or not.
	Errors:		None	
	SeeAlso:	GotError; IsReportingSet; IsThrowingSet	

********************************************************************************************/
BOOL CCFile::SetThrowExceptions( BOOL newState )
{
	BOOL OldState = ThrowExceptions;

	ThrowExceptions = newState;

	return OldState;
}

/********************************************************************************************

>	BOOL CCFile::IsReportingSet()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/09/94
	Inputs:		None.
	Outputs:	None
	Returns:	True if error reporting flag is set.
	Purpose:	Public way of checking the internal error reporting flag to say whether
				InformError is going to be called or not.
	Errors:		None	
	SeeAlso:	GotError; SetReportErrors; SetThrowExceptions	

********************************************************************************************/
BOOL CCFile::IsReportingSet()
{
	return ReportErrors;
}

/********************************************************************************************

>	BOOL CCFile::IsThrowingSet()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/09/94
	Inputs:		None.
	Outputs:	None
	Returns:	True if exception throwing flag is set.
	Purpose:	Public way of checking the internal error throwing flag to say whether
				errors are going to throw an exception or not.
	Errors:		None	
	SeeAlso:	GotError; SetReportErrors; SetThrowExceptions	

********************************************************************************************/
BOOL CCFile::IsThrowingSet()
{
	return ThrowExceptions;
}

/********************************************************************************************

>	BOOL CCFile::GetName( StringBase* name) const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/09/94
	Inputs:		Pointer to a CCFile name 
	Outputs:	None
	Returns:	True if a name has been set otherwise False. At present always returns False.
	Purpose:	Given a CCFile*, you can ask for some sort of name which is associated with
				that file. For filenames it might be a filename, or a pathname, for resource
				files it might be "Default Bitmap" etc. These names could then be used for
				making error messages have some extra useful information in.
	Errors:		None	

********************************************************************************************/

BOOL CCFile::GetName( StringBase* name) const
{
	return FALSE;
}



/********************************************************************************************
>	virtual PathName CCFile::GetPathName() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Returns:	The path-name of this file.  If the file doesn't support the notion of a
				file-path (as, for instance, CCMemFiles and CCStreamFiles), then returns
				an empty string.
********************************************************************************************/

PathName CCFile::GetPathName() const
{
	PathName pn;
	return pn;
}



/********************************************************************************************

>	CCFile& CCFile::operator>>(StringBase* buf)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		String buffer.
	Returns:	Reference to the CCFile object.
	Purpose:	Reads file until string buffer is full or end of line or end of file is met.
	SeeAlso:	CCFile::read

********************************************************************************************/

CCFile& CCFile::operator>>(StringBase* buf)
{
	return read(buf);
}

/********************************************************************************************

>	CCFile& CCFile::operator<<(const StringBase& buf)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		buf - the string to write out to the file.
	Returns:	Reference to the CCFile object.
	Purpose:	Writes the charactors found in the String buffer.
	SeeAlso:	CCFile::write

********************************************************************************************/

CCFile& CCFile::operator<<(const StringBase& buf)
{
	return write(buf);
}

// These should be deleted soon..?
CCFile &CCFile::get( char &buf )
{
	return read(buf);
}

CCFile &CCFile::put( const char &buf )
{
	return write((void *) &buf);
}

/********************************************************************************************

>	virtual CCFile&	CCFile::flush()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Returns:	Reference to the CCFile object.
	Purpose:	Flushes a file's buffers.  By default it does nothing - override it if you	
				want it to do anything.  (The CCDiskFile class overrides this function.)
	SeeAlso:	CCDiskFile::flush

********************************************************************************************/

CCFile&	CCFile::flush()
{
	return (CCFile&) *this;
}

/********************************************************************************************

>	BOOL CCFile::good()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Returns:	TRUE  => file stream is ok.
				FALSE => an error has occured.
	Purpose:	Simple base implentation of this function - just examines EOF status.
				It allows checking of the current file status to see if we should continue
				using the file or not.
				Override this function to provide more sophisticated error checking
				(the CCDiskFile class overrides this function).

********************************************************************************************/

BOOL CCFile::good() const
{
	return !eof();
}

/********************************************************************************************

>	BOOL CCFile::bad()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Returns:	TRUE  => An error has occured.
				FALSE => The file stream is ok.
	Purpose:	Simple base implementation of this function - just examines EOF status.
				It allows checking of the current file status to see if we should continue
				using the file or not.
				Override this function to provide more sophisticated error checking
				(the CCDiskFile class overrides this function).

********************************************************************************************/

BOOL CCFile::bad() const
{
	return eof();
}

/********************************************************************************************

>	BOOL CCFile::fail()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Returns:	TRUE  => An error has occured.
				FALSE => The file stream is ok.
	Purpose:	Simple base implementation of this function - just examines EOF status.
				It allows checking of the current file status to see if we should continue
				using the file or not.
				Override this function to provide more sophisticated error checking
				(the CCDiskFile class overrides this function).

********************************************************************************************/

BOOL CCFile::fail() const
{
	return eof();
}





//-----------------------
/********************************************************************************************

										CCLexFile

********************************************************************************************/
//-----------------------




/********************************************************************************************

>	CCLexFile::CCLexFile(BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>				(Altered by Neville 01/08/94) 
	Created:	05/04/94
	Purpose:	Initialise a CCLexFile object.

********************************************************************************************/

CCLexFile::CCLexFile(BOOL ErrorReporting, BOOL ExceptionThrowing)
  :	CCFile(ErrorReporting, ExceptionThrowing),
	CharsRead(0)
{
	// Ensure that the buffers are NULL so that DeinitLexer() can be called
	// safely even if called after a failed InitLexer() call.
	LineBuf  = NULL;
	Buf      = NULL;
	TokenBuf = NULL;

	LexerInitialised = FALSE;
	TokenIsCached 	 = FALSE;

	SeekingRequired  = FALSE;
	IgnoreStringEscapeCodes = FALSE;

	DontFail = FALSE;

	//Graham's HTML parsing variables
	m_pcHTMLBuffer=NULL;
}

/********************************************************************************************

>	CCLexFile::~CCLexFile()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Purpose:	Deinitialises a CCLexFile object (calls DeinitLexer() for you).

********************************************************************************************/

CCLexFile::~CCLexFile()
{
	DeinitLexer();
}

// The size of the token buffer used for lexical analysis in the CCLexFile class.
const INT32 TokenBufSize = 256;

/********************************************************************************************

>	BOOL CCLexFile::InitLexer(BOOL IsSeekingRequired = FALSE, BOOL DoIgnoreStringEscapeCodes = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Inputs:		IsSeekingRequired - Set to TRUE and seekin and tellin will work, but
 									all the lexfile stuff is slowed down a bit
				DoIgnoreStringEscapeCodes - Set to TRUE to force '\' chars in strings to be ignored
										    (see below)
	Returns:	TRUE if successful; FALSE otherwise.
	Purpose:	Prepare the file object for performing lexical analysis on the input
				stream.  This allocates the buffers and sets the various character sets
				to the default values:
					Whitespace:	spaces and tabs
					Delimiters: none
					Comment:	#
					String:		Delimited by " and " (i.e. like C)
				Also resets the newline indent output used by PutNewLine() to 0;

  				DoIgnoreStringEscapeCodes = Should escape codes in strings be ignored?
				e.g. Should "hello\r" turn into "hello" + CR, or should we ignore the \r code and hence leave
				the string unaltered?
				I.e. if DoIgnoreStringEscapeCodes is TRUE, then the '\' character in strings is left alone.

	Errors:		Out of memory.
	SeeAlso:	CCLexFile::SetWhitespace; CCLexFile::SetDelimiters; 
				CCLexFile::SetCommentMarker; CCLexFile::SetStringDelimiters

********************************************************************************************/

BOOL CCLexFile::InitLexer(BOOL IsSeekingRequired, BOOL DoIgnoreStringEscapeCodes)
{
	// If we've already been here before, get out of town.
 	if (LexerInitialised) return TRUE;

	// Allocate buffer for reading in each line
	LineBuf = new String_256;
	if (LineBuf == NULL)
		return FALSE;

	// Allocate space for storing tokens while decoding a line
	TokenBuf = new TCHAR[TokenBufSize];
	if (TokenBuf == NULL)
	{
		delete LineBuf; 
		LineBuf = NULL; 
		return FALSE;
	}

	// Get ready to parse tokens.
	TokenBuf[0] = 0;
	Ofs = 0;
	Ch = 0;
	Line = 0;
	
	// Make buffer pointer point to line buffer.
	Buf = (TCHAR *) (*LineBuf);

	CharsRead = 0;
	LastLinePos = (FilePos) 0;
	EOFFound = FALSE;

	// Set character classes to reasonable defaults.
	WhitespaceChars = " \t";
	DelimiterChars = NULL;
	CommentMarker = '#';
	StringDelimiters = "\"\"";

	// reset the current indent count and delta value
	IndentSpaces = 0;
	IndentDelta	 = 4;

	// If true then seeking and tellin lex files will work - slows things down a bit
	SeekingRequired = IsSeekingRequired;

	// Ignore '\' characters in string tokens?
	IgnoreStringEscapeCodes = DoIgnoreStringEscapeCodes;

	InitHTMLLexer();
						
	LexerInitialised = TRUE;

	return TRUE;
}


/********************************************************************************************

>	void CCLexFile::DeinitLexer()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Purpose:	Frees up the dynamic objects allocated by CCLexFile::InitLexer.
				Can be called at any point, and should be called before the next call
				to InitLexer(), even if the last InitLexer() call has failed.
	SeeAlso:	CCLexFile::InitLexer

********************************************************************************************/

void CCLexFile::DeinitLexer()
{
	// Deallocate our buffers
	if (LineBuf  != NULL) delete LineBuf;
	if (TokenBuf != NULL) delete TokenBuf;

	LineBuf  = NULL;
	TokenBuf = NULL;
	Buf      = NULL;

	DeleteHTMLBuffer();

	LexerInitialised = FALSE;
}

/********************************************************************************************

>	void CCLexFile::SetWhitespace(char *Str)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Inputs:		Str - the set of characters to treat as whitespace.
	Purpose:	This function changes the set of characters that the lexical analyser
				treats as white space.  Note that it defaults to tabs and spaces, but you
				can specify any set of characters you want - for example you could even
				pass in " tf" and it would treat spaces, lower case t's and lower case f's
				as whitespace characters.  Tokens extracted by CCLexFile::GetToken never
				contain any whitespace characters.
	SeeAlso:	CCLexFile::InitLexer; CCLexFile::GetToken

********************************************************************************************/

void CCLexFile::SetWhitespace(char *Str)
{
	WhitespaceChars = Str;
}

/********************************************************************************************

>	void CCLexFile::SetDelimiters(char *Str)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Inputs:		Str - the set of characters to be interpreted as token delimiters.
	Purpose:	Change the set of characters that are treated as 'delimiters'. A delimiter
				character indicates that a token has finished. A delimiter character *will*
				be extracted by CCLexFile::GetToken, and returned on its own.  For example, 
				for the line "hello; there ();", processed with the set of delimiters
				";()", you would get the following tokens out of CCLexFile::GetToken:
				MonoOn
					"hello"
					";"
					"there"
					"("
					")"
					";"
				MonoOff

				By default, the set of delimiter characters is empty.

	SeeAlso:	CCLexFile::InitLexer; CCLexFile::GetToken

********************************************************************************************/

void CCLexFile::SetDelimiters(char *Str)
{
	DelimiterChars = Str;
}

/********************************************************************************************

>	void CCLexFile::SetCommentMarker(char ch)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Inputs:		ch - the character to use as the comment marker.
	Purpose:	Change the character used to denote a comment token.
				Comments start from the comment character until the end of the line.
				So, for example, to set the lexer up for PostScript style comments, pass
				in '%' as the comment marker; for assembler style comments, pass in ';'.
				Multi-line comments are not supported.
	SeeAlso:	CCLexFile::InitLexer; CCLexFile::GetToken

********************************************************************************************/

void CCLexFile::SetCommentMarker(char ch)
{
	CommentMarker = ch;
}

/********************************************************************************************

>	void CCLexFile::SetStringDelimiters(char *Str)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Inputs:		Str - the two-character string to indicate what characters to use to
					  start and end strings.
	Purpose:	Change the characters used to delimit a string.  The lexer will handle
				strings as a special token type (see CCLexFile::GetToken).
				You can specify which characters delimit the strings - you pass in a 
				two-character string; the first character is used to start the string,and
				the second is used to terminate it.  The two characters can be the same.
				So, for example, for PostScript strings, use "()", or for C-style strings,
				use "\"\"".
	SeeAlso:	CCLexFile::InitLexer; CCLexFile::GetToken

********************************************************************************************/

void CCLexFile::SetStringDelimiters(char *Str)
{
	StringDelimiters = Str;
}

/********************************************************************************************

>	BOOL CCLexFile::GetSimpleToken()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		-
	Purpose:	A simplified interface onto GetToken().
				The caller is not informed of token types TOKEN_COMMENT and TOKEN_EOL.
				The function returns TRUE if a TOKEN_NORMAL or TOKEN_STRING is read.
				FALSE is returned if TOKEN_EOF is read, or if FALSE is returned by 
				GetToken().

	SeeAlso:	CCLexFile::GetToken

********************************************************************************************/

BOOL CCLexFile::GetSimpleToken()
{
	BOOL ok;
	LexTokenType TokenType;

	do
	{
		do
		{
			ok = GetToken();
		} while (DelimiterFound);

		TokenType = GetTokenType();
	} while (ok && 	(TokenType != TOKEN_NORMAL) && 
					(TokenType != TOKEN_STRING) && 
					(TokenType != TOKEN_EOF));

	return (ok && (TokenType != TOKEN_EOF));
}
		




/********************************************************************************************

>	BOOL CCLexFile::GetToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Returns:	TRUE if all went well; 
				FALSE otherwise.
	Purpose:	Extract the next lexical token from the input stream of this file.
				Tokens are separated by whitespace, delimiters, line-breaks, or any 
				combination of the three.  Delimiters and line-breaks are returned as
				tokens (and may be ignored if desired); whitespace is never returned.
				Call CCLexFile::GetTokenType() to find out what kind of token was extracted.
				You can examine the token directly by looking at the buffer returned by
				CCLexFile::GetTokenBuf() (this is a const buffer so you may examine it, but
				not alter it).  This buffer address will not change unless 
				CCLexFile::InitLexer is called again, so it is safe to cache the return
				value of CCLexFile::GetTokenBuf.

	Errors:		A 'string' token exceeded 256 characters in length.

	SeeAlso:	CCLexFile::GetTokenType; CCLexFile::GetTokenBuf; CCLexFile::SetWhitespace;
				CCLexFile::SetDelimiters; CCLexFile::SetCommentMarker;
				CCLexFile::SetStringDelimiters; CCLexFile::InitLexer; CCLexFile

********************************************************************************************/

BOOL CCLexFile::GetToken()
{
	// Check to see if the client has put a token back onto the input stream
	if (TokenIsCached)
	{
		// Yes - just return success to the caller and they will use the current token
		// again.
		TokenIsCached = FALSE;
		return TRUE;
	}

	DelimiterFound = FALSE;

	// No effect if at EOF
	if (EOFFound)
	{
		TokenType = TOKEN_EOF;
		TokenBuf[0] = 0;
		return TRUE;
	}
		
	// Ignore white space until we get something useful
	while (!EOFFound && IsWhitespace())
		GetCh();
	
	if (EOFFound)
	{
		TokenType = TOKEN_EOF;
		TokenBuf[0] = 0;
		return TRUE;
	}

	// Check for line breaks
	if (Ch == 0)
	{
		GetCh();
		TokenType = TOKEN_EOL;
		return TRUE;
	}
	else if (Ch == StringDelimiters[0])
	{
		// It's a string
		return GetStringToken();
	}
	else if (Ch == CommentMarker)
	{
		// It's a comment - ignore the rest of this line
		camStrcpy( TokenBuf, Buf + Ofs - 1 );
		TokenType = TOKEN_COMMENT;
		
		GetLine();
		GetCh();
	}
	// Extract the next token
	else
	{
		// Read until delimiter or white-space, and analyse result.
		// NB. 0-terminator ends a token => tokens cannot be split over a line.
		INT32 i = 0;

		if (IsDelim())
		{
			// Found a delimiter token - just return it
			TokenBuf[i++] = Ch;
			GetCh();
			DelimiterFound = TRUE;
		}
		else
		{
			do
			{
				TokenBuf[i++] = Ch;
				GetCh();
			} while (!EOFFound && !IsDelim() && !IsWhitespace() && (Ch != 0));
		}

		// Terminate the token
		TokenBuf[i] = 0;
		TokenType = TOKEN_NORMAL;
	} 

	// Token extracted ok
	return TRUE;
}

/********************************************************************

  Graham 15/5/97

  CCLexFile: HTML parsing code

  The lexer above isn't suited to parsing HTML code. For example, it insists
  on breaking tokens at the end of lines, but HTML should be able to run
  over lines ignoring line break characters. And HTML tokens should be able
  to be any length...not just 256 characters. And so on.

  So it makes sense to do HTML parsing completely separately.
  
	This HTML lexer returns two sorts of tokens:

  TAGS	Which start with < and end with >

  TEXT  Which is anything in between tags.

  So for example, a file containing the characters <B>Bold</B> would produce
  three tokens:

  TAG	<B>
  TEXT	Bold
  TAG	</B>

  Here's an example of how to use the lexer. This code reads
  all the HTML from a file pMyFile and prints out only the tags. (I haven't
  tested this code, of course, there might be the odd mistake).

  //First initialise the lexer. This function will call InitHTMLLexer().
  InitLexer();

  //And read HTML tokens from the file until we reach the end of the file
  while (!pMyFile->IsEndOfHTMLFile())
  {
		//Tell the HTML lexer to get an HTML token into its buffer
		pMyFile->GetHTMLToken();

		//Get the text buffer
		TCHAR* pcBuffer=pMyFile->GetHTMLTokenBuffer();

		//Now, does the buffer contain a tag?
		if (pMyFile->IsHTMLTag())
		{
			//Yes. So print it out.
			//Remember that pcBuffer may be NULL if it had nothing in it
			if (pcBuffer)
				TRACE(pcBuffer);
		}
	}

  //And deinitialise the lexer
  DeInitLexer();


  Also see the function ImagemapFilter::WriteExistingDataHelper.

  ************************************************************************/

/********************************************************************************************

>	void CCLexFile::InitHTMLLexer()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/97
	Inputs:		-
	Returns:	-
	Purpose:	Initialises all the variables used in the HTML parsing code

	Errors:		-
	SeeAlso:	CCLexFile::InitLexer

********************************************************************************************/

void CCLexFile::InitHTMLLexer()
{
	//Initialise the "character waiting" variables
	m_cWaitingCharacter=0;
	m_fIsCharacterWaiting=FALSE;

	//And delete the buffer
	DeleteHTMLBuffer();

	//And say that we haven't reached the end of the file	
	m_fEndOfHTMLFile=FALSE;

	//And say that the current token is not a tag
	m_fIsTag=FALSE;
	
}

/********************************************************************************************

>	void CCLexFile::DeleteHTMLBuffer()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/97
	Inputs:		-
	Returns:	-
	Purpose:	Deletes the HTML buffer and sets all the member variables
				that refer to it to appropriate defaults

	Errors:		-
	SeeAlso:	CCLexFile::InitHTMLLexer

********************************************************************************************/

void CCLexFile::DeleteHTMLBuffer()
{
	if (m_pcHTMLBuffer)
		free( m_pcHTMLBuffer );

	m_pcHTMLBuffer=NULL;

	m_iCharsInHTMLBuffer=0;
	m_iLengthOfHTMLBuffer=0;

}

/********************************************************************************************

>	void CCLexFile::AddToHTMLBuffer(TCHAR &cToAdd)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/97
	Inputs:		cToAdd - the character to add to the buffer
	Returns:	-
	Purpose:	Adds cToAdd to the buffer, extending the buffer if
				necessary.

	Errors:		-
	SeeAlso:	CCLexFile::GetHTMLToken

********************************************************************************************/

void CCLexFile::AddToHTMLBuffer( char cToAdd )
{
	//First, do we have an HTML buffer?
	if( NULL == m_pcHTMLBuffer )
	{
		//No. So create one now
		m_iLengthOfHTMLBuffer = TokenBufSize;

		m_pcHTMLBuffer = (char *)malloc( m_iLengthOfHTMLBuffer );

		m_iCharsInHTMLBuffer = 0;
	}

	//Now, is there enough room in our HTML buffer to store
	//our new character?
	if( m_iCharsInHTMLBuffer >= m_iLengthOfHTMLBuffer )
	{
		//No. So double the size of the buffer
		m_iLengthOfHTMLBuffer *= 2;

		m_pcHTMLBuffer = (char *)realloc( m_pcHTMLBuffer, m_iLengthOfHTMLBuffer );
	}

	ERROR3IF(m_pcHTMLBuffer==NULL, "AddToHTMLBUffer - realloc error");
	
	//And finally add our character
	m_pcHTMLBuffer[m_iCharsInHTMLBuffer] = cToAdd;

	//And increase the number of characters in the buffer
	m_iCharsInHTMLBuffer++;
}


/********************************************************************************************

>	BOOL CCLexFile::GetHTMLToken(BOOL fIgnoreEOL=TRUE, BOOL fCorrectCase=TRUE)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/97
	Inputs:		fIgnoreReturns	Whether all EOL characters should
								be removed from the text in the buffer. This
								is TRUE by default because HTML should
								normally ignore EOL characters

				fCorrectCase	Whether the string should automatically
								be converted to upper case. This is TRUE
								by default because HTML should ignore case.

	Returns:	TRUE if all went well; 
				FALSE otherwise.
	Purpose:	Get the next HTML token from the file.

				This works as follows: if the first character read in the token
				is a <, then the token will contain all the characters between
				that < and the following >.

				If the next character read is not a <, then the token will
				contain all the characters until the next <.

				So the token you get back from this function will
				either be an HTML tag:

				<IMG SRC="filename.gif" ALT="Alt text">

				Or some text:

				This is some text

				Note that we read into a variable length buffer.
				This is because HTML tokens could be any length at all. So,
				as soon as you have called GetHTMLToken, you should call
				GetHTMLTokenBuffer to get a pointer to the string that
				has just been read.

	Errors:		-

	SeeAlso:	CCLexFile::GetToken; CCLexFile::GetHTMLTokenBuffer

  

********************************************************************************************/

BOOL CCLexFile::GetHTMLToken(BOOL fIgnoreEOL, BOOL fCorrectCase)
{
	//First delete our buffer
	DeleteHTMLBuffer();
									   
	//Now, are we reading a tag or some text?
	//It depends on whether the first character we read is a <
	m_fIsTag=(PeekNextHTMLChar()=='<');

	//Create a two-character buffer to read characters into
	//(Why is it a buffer, not a single character? Because the
	//function _tcsupr needs to be passed a null-terminated
	//string rather than a single character)
	TCHAR				pcReadCharacter[2];

		
	//But when does the HTML token end? If the token is a tag, it ends after the 
	//next > is read. If the token is text, it ends just *before* the next <
	//is read. This condition is contained in the following while statement...
	while (!eof()
		&& !(m_fIsTag && pcReadCharacter[0]=='>')
		&& !(!m_fIsTag && PeekNextHTMLChar()=='<'))
	{
		//Read in a character
		pcReadCharacter[0]=ReadNextHTMLChar();

		//And null terminate the string
		pcReadCharacter[1]='\0';

		//Correct the string to uppercase if necessary
		if( fCorrectCase )
			CharUpper( pcReadCharacter );

		//If we just reached the end of the file, break
		if (eof())
			break;

		//Now, are we ignoring end of line characters?
		//And is the character we've read an end of line character?
		if (!(fIgnoreEOL && (pcReadCharacter[0]=='\r' || pcReadCharacter[0]=='\n')))
		{
			//No. So add the character to the buffer
			AddToHTMLBuffer(pcReadCharacter[0]);
		}
	}

	//If we reached the end of the file
	if (eof())
	{
		//Then set our member flag
		m_fEndOfHTMLFile=TRUE;
	}

	//And NULL terminate the buffer
	AddToHTMLBuffer('\0');

	//And return TRUE
	return TRUE;
}

/********************************************************************************************

>	TCHAR CCLexFile::PeekNextHTMLChar()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/97
	Returns:	The next character that will be read from the file
	Purpose:	In theory, this function finds out what the next character
				that will be read from the file will be.

				But of course to find out what the next character in the file is,
				we have to read that character.

				So this is what we do. We read the character, but we put it into
				our member variable m_cWaitingCharacter. We also set our member flag
				m_fIsCharacterWaiting to TRUE.

				Then the function ReadNextHTMLCharacter will know there is a character waiting
				to be 'read' in m_cWaitingCharacter, and will return that
				character instead of actually reading one from the file.

				Not perfect but it works.

	Errors:		-

	SeeAlso:	CCLexFile::GetHTMLToken; CCLexFile::ReadNextHTMLChar
									 
********************************************************************************************/

char CCLexFile::PeekNextHTMLChar()
{
	//Is there a character waiting in m_cWaitingCharacter?
	if (!m_fIsCharacterWaiting)
	{
		//No. So read the next character into our "Waiting Character" buffer
		//We use "get" because "read" will throw an exception 
		//if you try to read past the end of the file.
		get(m_cWaitingCharacter);

		//And remember there's a character waiting
		m_fIsCharacterWaiting=TRUE;
	}

	//Now return the waiting character
	return m_cWaitingCharacter;

}

/********************************************************************************************

>	TCHAR CCLexFile::ReadNextHTMLChar()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/97
	Returns:	The next character in the file
	Purpose:	This function reads the next character from the file. Well, actually,
				the way it works is slightly more complicated.
				
				That's because the function PeekNextHTMLChar may have given us
				a character	"waiting to be read" in m_cCharacterWaiting. If so, 
				we should return that character first.

				See PeekNextHTMLChar for a better explanation.

	Errors:		-

	SeeAlso:	CCLexFile::GetHTMLToken; CCLexFile::PeekNextHTMLChar
									 
********************************************************************************************/

char CCLexFile::ReadNextHTMLChar()
{
	//Do we have a character waiting?
	if (m_fIsCharacterWaiting)
	{
		//Yes. So remember we no longer have a character waiting
		m_fIsCharacterWaiting=FALSE;

		//And return our waiting character
		return m_cWaitingCharacter;
			   		
	}
	else
	{
		//No. So simply read a new character from the file
		//We use "get" because "read" will throw an exception 
		//if you try to read past the end of the file. 
		char			cToRead;
		get( cToRead );

		return cToRead;
	}
}

/********************************************************************************************

>	String_256 CCLexFile::GetHTMLTagName()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/97
	Returns:	The name of the tag which is currently in the buffer
	Purpose:	This function reads the name of the tag
				in the buffer.

				It does this by simply reading from the second character
				in the buffer to the first whitespace character (where
				whitespace is usually either a space or a tab).

				Also, if the token in the buffer is not a tag, this function
				returns FALSE.

	Errors:		-

	SeeAlso:	CCLexFile::GetHTMLToken
									 
********************************************************************************************/

String_256 CCLexFile::GetHTMLTagName()
{
	//Create an empty string to return
	String_256 strToReturn = "";

	//If this isn't a tag, return that empty string
	if (!IsHTMLTag())
		return strToReturn;

	//And, if there's less than two characters in the buffer,
	//return now
	if (m_iCharsInHTMLBuffer<2)
		return strToReturn;

	//Otherwise, start copying from the buffer to the string
	//Start with the second character (remember that the second
	//character is m_pcHTMLBuffer[1]) and finish
	//when either we find a whitespace character or we go
	//past the end of the string
	INT32 i;
	for( i = 1; 
		( i < 255 &&  
		  i < ( m_iCharsInHTMLBuffer - 1 ) && 
		  !IsWhitespace( m_pcHTMLBuffer[i] ) && 
		  m_pcHTMLBuffer[i] != _T('>') ); 
		i++ )
	{
		strToReturn+=m_pcHTMLBuffer[i];
	}

	// test for overflow
	if (i == 255)
		return "";

	//Correct the string to upper case
	strToReturn.toUpper();

	//And return our string
	return strToReturn;

}

/********************************************************************************************

>	String_256 CCLexFile::GetHTMLParameterValue(String_256 strParameterName, BOOL fCorrectCase=TRUE)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/97
	Inputs:		strParameterName	The name of the parameter whose
									value we want to find. For example,
									in an image tag, we might want to find
									the value of 'SRC'.

				fCorrectCase		TRUE if the value should be returned in uppercase
									FALSE if the value should be returned in its original case
	Returns:	If the parameter is specified within the buffer (e.g.
				<IMG SRC="www.xara.com">, then the value of that parameter
				will be returned without any enclosing quotes (e.g. www.xara.com
				will be returned, not "www.xara.com").

				If the parameter is not specified within the buffer, an
				empty string will be returned.
											  				
	Purpose:	Finds the value of the specified parameter.

				For example, if we search for the value of "ALT" and 
				the buffer contains the text

				<IMG SRC="http://www.xara.com" ALT=Alt>

				Then the string returned will be 
				
				  Alt

				This is done as follows: first we make up a composite string
				consisting of the strParameterName with an equals sign on the end
				(e.g. "ALT="). Then we search for that string.

				If we find it:

				Then if the character after that string is a ", we return everything
				up until the next ".

				Otherwise, if the character after that string is not a ", we
				return everything up until the next > or whitespace character.

	Errors:		-

	SeeAlso:	CCLexFile::GetHTMLToken
									 
********************************************************************************************/

String_256 CCLexFile::GetHTMLParameterValue( const String_256 &strParameterName, BOOL fCorrectCase )
{
	//Create an empty string to return
	String_256 strToReturn;

	//And, if there's less than two characters in the buffer,
	//return now
	if (m_iCharsInHTMLBuffer<2)
		return strToReturn;

	//So search for the string we have been told to search for	
	PCSTR				pcFound;
#if 0 != wxUSE_UNICODE
	{
		size_t			cchParam = camWcstombs( NULL, (const TCHAR *)strParameterName, 0 ) + 1;
		PSTR			pszParam = PSTR( alloca( cchParam ) );
		camWcstombs( pszParam, (const TCHAR *)strParameterName, cchParam );
		pcFound = FindStringWithoutCase( m_pcHTMLBuffer, pszParam );
	}
#else
	pcFound = FindStringWithoutCase( m_pcHTMLBuffer, (const TCHAR *)strParameterName );
#endif

	//Have we found something?
	if (pcFound)
	{
		//Yes. So move our found pointer past the string we've found
//		pcFound = camStrninc( pcFound, strParameterName.Length() );
		pcFound = pcFound + strParameterName.Length();

		//We're now pointing to the character after the parameter name

		//If we're pointing to a whitespace character, move forward
		//to the first character that's not whitespace					
		while (*pcFound!='\0' && IsWhitespace(*pcFound))		
		{
//			pcFound = camStrinc( pcFound );
			pcFound++;
		}
		 
		//If that character is not an equals, return an empty string
		if (*pcFound!='=')
			return strToReturn;

		//And, again, move our found pointer to the next character that's not a space
		do
		{
//			pcFound = camStrinc( pcFound );
			pcFound++;
		}
		while (*pcFound!='\0' && IsWhitespace(*pcFound));		

		//Note that there is no way that pcFound can
		//have been advanced past the end of the buffer.
		//It may, possibly, have been advanced to the NULL
		//character at the end of the buffer, and I'll handle that...

		//Now, this variable will tell us whether the parameter value
		//is in quotes. Set to FALSE by default.
		BOOL fInQuotes=FALSE;

		//And is the parameter value in quotes?
		if (*pcFound=='\"')
		{
			//Yes. So make a note of that
			fInQuotes=TRUE;

			//And move on to the character after the quotes
//			pcFound=camStrinc(pcFound);
			pcFound++;
		}

		//Now, copy everything from the character we are pointing at
		//until...
		//IF the parameter value is in quotes, until the next "
		//OTHERWISE, until the next > or whitespace character
		while (*pcFound!='\0' &&
			((fInQuotes && *pcFound!='\"')
			|| (!fInQuotes && (!IsWhitespace(*pcFound) && *pcFound!='>'))))
		{
			strToReturn+=*pcFound;
//			pcFound=camStrinc(pcFound);
			pcFound++;
		}
	}

	//And correct the case if necessary
	if (fCorrectCase)
		strToReturn.toUpper();

	//And return the string
	//Note that if nothing was found, this string will be empty
	return strToReturn;
}

/********************************************************************************************

>	TCHAR* CCLexFile::FindStringWithoutCase(TCHAR* strToSearch, TCHAR* strToFind)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/97
	Inputs:		strToSearch			The string in which to search

				strToFind			The string to search for
	Returns:	A pointer to the first occurrence of strToFind within
				strToSearch.
											  				
	Purpose:	Finds strToFind within strToSearch.

				The find operation is done without regard to case - so,
				for example, the string "Map" would be found within the
				string "<MAP NAME=MyMap>"

	Errors:		-

	SeeAlso:	CCLexFile::GetHTMLParameterValue
									 
********************************************************************************************/

PCSTR CCLexFile::FindStringWithoutCase( PCSTR strToSearch, PCSTR strToFind )
{
	//We assume strToSearch and strToFind are valid

	//First set up a pointer to search strToSearch with
	PCSTR				pcThisChar = strToSearch;

	//Now, while pcThisChar is valid
	while( pcThisChar != NULL && *pcThisChar != '\0' )
	{
		//Is strToSearch from pcThisChar onwards the same
		//as strToFind, doing a case insensitive comparision?
		if( 0 == _strnicmp( pcThisChar, strToFind, strlen( strToFind ) ) )
		{
			//Yes. So return TRUE
			return pcThisChar;
		}

//		pcThisChar = camStrinc(pcThisChar);
		pcThisChar++;
	}

	//Otherwise return NULL
	return NULL;
}

/********************************************************************************************

>	BOOL CCLexFile::GetHexToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Returns:	TRUE if all went well; 
				FALSE otherwise.
	Purpose:	Extract the next lexical token from the input stream of this file.
				The token is expected to be a string of hexadecimal digits, and its length
				should be an even number of characters. If whitespace or a delimiter is 
				encountered then the token is returned ok.  If any other character is found
				which is not a legal hex digit thenFALSE is returned.

	Errors:		A 'string' token exceeded 256 characters in length.

	SeeAlso:	CCLexFile::GetTokenType; CCLexFile::GetTokenBuf; CCLexFile::SetWhitespace;
				CCLexFile::SetDelimiters; CCLexFile::SetCommentMarker;
				CCLexFile::SetStringDelimiters; CCLexFile::InitLexer; CCLexFile

********************************************************************************************/

BOOL CCLexFile::GetHexToken()
{
	// Check to see if the client has put a token back onto the input stream
	if (TokenIsCached)
	{
		// Yes - just return success to the caller and they will use the current token
		// again.
		// But for hex token we must check that it is a legal hex token...
		INT32 i = 0;
		while (TokenBuf[i] != 0)
		{
			if (!camIsxdigit(TokenBuf[i]))
				// Not valid hex data
				return FALSE;

			// Try next character
			i++;
		}

		// Valid hex data - return success.
		TokenIsCached = FALSE;
		return TRUE;
	}

	DelimiterFound = FALSE;

	// Keep ignoring whitespace/comments etc
	for(;;)
	{
		// No effect if at EOF
		if (EOFFound)
		{
			// Did not find a hex token
			TokenType = TOKEN_EOF;
			TokenBuf[0] = 0;
			return FALSE;
		}
			
		// Ignore white space until we get something useful
		while (!EOFFound && IsWhitespace())
			GetCh();
		
		if (EOFFound)
		{
			// Did not find a hex token
			TokenType = TOKEN_EOF;
			TokenBuf[0] = 0;
			return FALSE;
		}

		if (Ch == 0)
		{
			// Skip line breaks
			GetCh();
		}
		else if (Ch == CommentMarker)
		{
			// It's a comment - ignore the rest of this line
			GetLine();
			GetCh();
		}
		else
		{
			// Ok - is it a hex digit?
			if (camIsxdigit(Ch))
				// Yes - fall through to next bit of code to decode it
				break;
			else
				// No - so we can't find a hex string so return an error
				return FALSE;
		}
	}

	// Read until not a hexadecimal digit, and analyse result.
	// NB. 0-terminator ends a token => tokens cannot be split over a line.
	INT32 i = 0;

	// This loop is hugely critical (e.g. importing large bitmaps), so we resort to direct 
	// buffer access and register variables. (sorry!)
	register TCHAR *pHexCh = Buf + Ofs - 1;

	while (camIsxdigit(*pHexCh))
	{
		TokenBuf[i++] = *pHexCh++;
	}

	// Update the offset variable and Ch variable so we maintain lookahead.
	Ofs += i;
	Ch = Buf[Ofs-1];

	// Set up token correctly.
	TokenBuf[i] = 0;
	TokenType = TOKEN_NORMAL;

	// End of hex data - is it a legal or illegal end?
	if ((i % 2) != 0)
	{
		// Not an even number of hex digits => illegal
		return FALSE;
	}

	// Inform caller if the hex data was terminated legally 
	// (i.e. delimiter, whitespace or line-end)

	return (IsDelim() || IsWhitespace() || (Ch == 0));
}


/********************************************************************************************

>	void CCLexFile::UngetToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Purpose:	Put the current token back onto the input stream - i.e. the next time 
				GetToken() is called it will return the token it returned last time.
				This is not nestable - you can only put back one token at once.
	SeeAlso:	CCLexFile::GetToken(); CCLexFile

********************************************************************************************/

void CCLexFile::UngetToken()
{
	TokenIsCached = TRUE;
}

/********************************************************************************************

>	BOOL CCLexFile::GetLineToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/94
	Returns:	TRUE if all went well; 
				FALSE otherwise.
	Purpose:	Read in a token in a line-based manner.  If the current input position is 
				in the middle of a line, the data up until the end of the line is read. 
				If at the start of a line, the whole line is returned.
				The token is still examined to work out what it is, but obviously it may not
				match any proper construct, in which case the type of the token is set to 
				TOKEN_LINE.
				(NB this is the only time that TOKEN_LINE is used)
	SeeAlso:	CCLexFile::GetToken; CCLexFile::GetSimpleToken; CCLexFile::UngetToken

********************************************************************************************/

BOOL CCLexFile::GetLineToken()
{
	// Check to see if the client has put a token back onto the input stream
	if (TokenIsCached)
	{
		TokenIsCached = FALSE;

		if (Ofs == 1)
		{
			// We're at the start of the line, so the last token must have been the
			// wholeof the rest of the line, so just return it.
			return TRUE;
		}

		// Otherwise, we should append the rest of the current line to the token, 
		// and work out the token type again.
		camStrcpy( TokenBuf + camStrlen( TokenBuf ), Buf + Ofs - 1 );

		if (TokenBuf[0] == CommentMarker)
		{
			TokenType = TOKEN_COMMENT;
		}
		else
		{
			TokenType = TOKEN_LINE;
		}

		GetLine();
		GetCh();

		// We have a token
		return TRUE;
	}

	// No effect if at EOF
	if (EOFFound)
	{
		TokenType = TOKEN_EOF;
		TokenBuf[0] = 0;
		return TRUE;
	}
		
	// Ignore white space until we get something useful
	while (!EOFFound && IsWhitespace())
		GetCh();
	
	if (EOFFound)
	{
		TokenType = TOKEN_EOF;
		TokenBuf[0] = 0;
		return TRUE;
	}

	// Check for line breaks
	if (Ch == 0)
	{
		GetCh();
		TokenType = TOKEN_EOL;
		return TRUE;
	}

	// Otherwise, just get the rest of the line...
	camStrcpy( TokenBuf, Buf + Ofs - 1 );

	GetLine();
	GetCh();

	if (TokenBuf[0] == CommentMarker)
	{
		TokenType = TOKEN_COMMENT;
	}
	else
	{
		TokenType = TOKEN_LINE;
	}

	// Token extracted ok
	return TRUE;
}

/********************************************************************************************

>	BOOL CCLexFile::GetStringToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Returns:	TRUE if all ok; FALSE if not.
	Purpose:	Extracts a string token from the input stream, as defined by the currently
				installed string delimiters.
	Errors:		String too long.
	SeeAlso:	CCLexFile::InitLexer; CCLexFile; CCLexFile::SetStringDelimiters

********************************************************************************************/

BOOL CCLexFile::GetStringToken()
{
	INT32 i = 0;

	TokenType = TOKEN_STRING;
	
	// Extract the string.
	// The first time through the loop causes the 'open string' character character to be 
	// discarded.
	do
	{
		GetCh();
		TokenBuf[i++] = Ch;
		if (!IgnoreStringEscapeCodes)
		{
			// We are NOT ignoring '\' characters, so have we found one?
			if (Ch == '\\')
			{
				// Backslash character - ignore it and insert the next character in its place.
				GetCh();
				if(Ch == 'r')
				{
					TokenBuf[i-1] = '\r';
					Ch = 32;
				}
				else
				{
					TokenBuf[i-1] = Ch;

					// Set Ch to 32, in case this is a string delimiter - it's been escaped (by
					// a backslash) so we don't want it to terminate the loop.
					// (Space (ASCII 32) is a 'safe' value - 0 is out as it causes GetCh() to
					// read in the next line.
					Ch = 32;
				}
			}
		}

		// If string is too long to fit into our buffer, this is classed as an error
		ERRORIF(i >= TokenBufSize, _R(IDT_LEX_STRINGTOOLONG), FALSE);

	} while (!EOFFound && (Ch != StringDelimiters[1]));
	
	// Null terminate the token string
	TokenBuf[i-1] = 0;

	// Discard the 'close string' character
	GetCh();

	// String extracted ok
	return TRUE;
}

/********************************************************************************************

>	void CCLexFile::GetLine()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Purpose:	Read in the next line from the file - used when extracted tokens with 
				CCLexFile::GetToken().   Handles the current line count, number of chars
				read count, etc.
	SeeAlso:	CCLexFile::GetCh

********************************************************************************************/

void CCLexFile::GetLine()
{
	// End of file?
	if (eof())
	{
		EOFFound = TRUE;
		return;
	}
	
	if(SeekingRequired)
	{
		// It would be much better to do it via variables rather than doing a tellin()
		//LastLinePos += CharsRead; //tellIn();
		//FilePos StartLastLinePos = LastLinePos;

		/*	static FilePos LastLinePosTmp = 0;
		static BOOL LastLinePosBool = FALSE;
										   
		if(LastLinePosTmp == 0)
		{
	 		LastLinePosTmp = LastLinePos + Ofs - 1 + 2;
			LastLinePosBool = FALSE;
		}
		else
		{
	 		LastLinePosTmp += Ofs + 1 + 2;
			if(LastLinePosBool == FALSE)
			{
				LastLinePosTmp -= 2;
				LastLinePosBool = TRUE;
			}
		}
	
		LastLinePos = LastLinePosTmp;*/

		//LastLinePos += Ofs + 1;

		// Store position of start of line	
		LexerInitialised = FALSE;
		LastLinePos = tellIn();
		LexerInitialised = TRUE;
	}
	else
	{
		LastLinePos = 0;
	}
	
	// Get a new line from the file.
	read( LineBuf );
	Line++;
	Ofs = 0;
}

/********************************************************************************************

>	void CCLexFile::GetCh()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Purpose:	Gets the next character from the input stream, reading in a new line if
				necessary.
	SeeAlso:	CCLexFile::GetLine

********************************************************************************************/

void CCLexFile::GetCh()
{
	// Get a character from the stream.
	
	// If end of file, there is no effect
	if (EOFFound)
		return;
		
	// If at the end of a line, get a new line first
	if ((Ch == 0) && (Ofs != 0))
		GetLine();
	
	// Test again as GetLine() might encounter the end of the file...
	// NB the above comment inserted for the hard of thinking (e.g. Justin and Simon)
	if (EOFFound)
	{
		Ch = 0;
		return;
	}
		
	Ch = Buf[Ofs++];
}

/********************************************************************************************

>	BOOL CCLexFile::IsDelim()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Returns:	TRUE if the current character is a delimiter; 
				FALSE if not.
	Purpose:	Test the current character (as read in by CCLexFile::GetCh) to see if it
				is a delimiter.
	SeeAlso:	CCLexFile::SetDelimiters

********************************************************************************************/

BOOL CCLexFile::IsDelim()
{
	if (DelimiterChars == NULL)
		return FALSE;

	return( strchr( DelimiterChars, Ch ) != NULL);
}

/********************************************************************************************

>	BOOL CCLexFile::IsWhitespace()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Returns:	TRUE if the current character is white-space; 
				FALSE if not.
	Purpose:	Test the current character (as read in by CCLexFile::GetCh) to see if it
				is a white-space character.
	SeeAlso:	CCLexFile::SetWhitespace

********************************************************************************************/

BOOL CCLexFile::IsWhitespace()
{	
	// 0-terminators are not whitespace
	if (Ch == 0)
		return FALSE;

	return( strchr( WhitespaceChars, Ch ) != NULL );
}

/********************************************************************************************

>	BOOL CCLexFile::IsWhitespace(TCHAR cToTest)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/97
	Inputs:		cToTest Character to test
	Returns:	TRUE if the character is white-space; 
				FALSE if not.
	Purpose:	Tests the character passed to see if it
				is a white-space character.
	SeeAlso:	CCLexFile::GetWhitespace()

********************************************************************************************/

BOOL CCLexFile::IsWhitespace( char cToTest )
{	
	// 0-terminators are not whitespace
	if (cToTest == 0)
		return FALSE;

	return( strchr( WhitespaceChars, cToTest ) != NULL );
}



/********************************************************************************************

>	BOOL CCLexFile::PutString(StringBase& str, UINT32 length=0, TCHAR* Sep=" ")

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/94
	Inputs:		str    = string to write
				length = num chars in str to write (0 means write to termination char)
				Sep    = chars to use as a separator between this string and the next.
	Outputs:	-
	Returns:	TRUE = successful, FALSE = error
	Purpose:	Outputs the string to the file enclosed in the string delimiting chars.
				(see CCLexFile::SetStringDelimiters for more details)
				Writes out 'length' chars of str, then finishes off by writing 'Sep' out
				as a bunch of Sep characters.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CCLexFile::PutString( const StringBase &str, UINT32 length, char* pszSep )
{
	write( StringDelimiters[0] );
	write( str, length );
	write( StringDelimiters[1] );
	if( strlen( pszSep ) > 0 )
		write( pszSep, (UINT32)strlen( pszSep ) );

	return (good());
}

/********************************************************************************************

>	BOOL CCLexFile::PutToken(StringBase& str, UINT32 length=0, TCHAR* Sep=" ")

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/94
	Inputs:		str    = string to write
				length = num chars in str to write (0 means write to termination char)
				Sep    = chars to use as a separator between this string and the next.
	Outputs:	-
	Returns:	TRUE = successful, FALSE = error
	Purpose:	Outputs the string to the file.
				Writes out 'length' chars of str, then finishes off by writing 'Sep' out
				as a bunch of Sep characters.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CCLexFile::PutToken( const StringBase &str, UINT32 length, char *pszSep )
{
	write( str, length );
	if( strlen( pszSep ) > 0 )
		write( pszSep, (UINT32)strlen( pszSep ) );

	return( good() );
}

/********************************************************************************************

>	BOOL CCLexFile::PutToken(const TCHAR* buf, TCHAR* Sep=" ")

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/94
	Inputs:		buf  = buffer of chars to write
				Sep  = chars to use as a separator between this string and the next.
	Outputs:	-
	Returns:	TRUE = successful, FALSE = error
	Purpose:	Outputs the string to the file.
				Writes out 'length' chars of str, then finishes off by writing 'Sep' out
				as a bunch of Sep characters.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CCLexFile::PutToken( const TCHAR *buf, char *Sep )
{
#if 0 != wxUSE_UNICODE
	size_t				cch = camWcstombs( NULL, (const TCHAR *)buf, 0 ) + 1;
	PSTR				psz = PSTR( alloca( cch ) );
	camWcstombs( psz, (const TCHAR *)buf, cch );
	write( psz, cch );
#else
	UINT32 length = camStrlen( buf );
	write( buf, length );
#endif
	if( strlen( Sep ) > 0 )
		write( Sep, (UINT32)strlen( Sep ) );

	return (good());
}

/********************************************************************************************

>	BOOL CCLexFile::PutToken(INT32 n, TCHAR* Sep=" ")

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/94
	Inputs:		n    = a number to write
				Sep  = chars to use as a separator between this string and the next.
	Outputs:	-
	Returns:	TRUE = successful, FALSE = error
	Purpose:	Outputs the number to the file.
				Writes out 'n', then finishes off by writing 'Sep' out
				as a bunch of Sep characters.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CCLexFile::PutToken( INT32 n, char *Sep )
{
	char				buf[256];
	_snprintf( buf, 256, "%d", n );

	UINT32 length = strlen( buf );
	write( buf, length );
	if( strlen( Sep ) > 0 )
		write( Sep, (UINT32)strlen(Sep) );

	return( good() );
}

/********************************************************************************************

>	BOOL CCLexFile::PutNewLine()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE = successful, FALSE = error
	Purpose:	Outputs a new line, followed by a number of spaces. The number of
				spaces output can be changed using IncIndent() and DecIndent();
				InitLexer resets the number of indent spaces to 0.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CCLexFile::PutNewLine()
{
	write("\n");
	for (INT32 n=0; n < IndentSpaces;n++)
		write(" ");

	return (good());
}

/********************************************************************************************

>	void CCLexFile::IncIndent()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/94
	Inputs:		-
	Outputs:	-
	Returns:	Increases IndentSpaces by IndentDelta.
	Purpose:	Increases the number of spaces written by PutNewLine() at the start
				of the next line.  It is increased by IndentDelta, which can be altered
				using SetIndentDelta()
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CCLexFile::IncIndent()
{
 	IndentSpaces += IndentDelta;
}

/********************************************************************************************

>	void CCLexFile::DecIndent()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/94
	Inputs:		-
	Outputs:	-
	Returns:	Decreases IndentSpaces by IndentDelta.
	Purpose:	Decreases the number of spaces written by PutNewLine() at the start
				of the next line.  It is decreased by IndentDelta, which can be altered
				using SetIndentDelta()
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CCLexFile::DecIndent()
{
 	IndentSpaces -= IndentDelta;
	if (IndentSpaces < 0) IndentSpaces = 0;
}



/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
////
////	CCStreamFile
////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************
>	CCStreamFile::CCStreamFile(iostream* pStream,
							   UINT32 bufferSize = CCFILE_DEFAULTSIZE,
							   BOOL ErrorReporting = TRUE,
							   BOOL ExceptionThrowing = FALSE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93        
	Inputs:		pstream		- the iostream to use for inout and output
				bufferSize	- Size of the fixed size buffer (default = 1024)
							- if size > 0 then create a buffer of the specified size
							- if size = 0 then create a file without a buffer
				ReportErrors flag
				ThrowException flag
	Purpose:	Default constructor.
	SeeAlso:	iostream::setbuf
********************************************************************************************/

CCStreamFile::CCStreamFile(iostream* pStream, UINT32 bufferSize,
						   BOOL ErrorReporting, BOOL ExceptionThrowing)
  :	CCLexFile(ErrorReporting, ExceptionThrowing),
	buffer(0),
	StreamFileInitialised(FALSE),
	IOFile(pStream),
	GZFile(NULL),
	GZStream(NULL),
	ModeOfFile(0)
{
	// Nothing to do if there's no stream (and note that StreamFileInitialised
	// remains FALSE if no iostream is passed).
//	TRACEUSER( "JustinF", _T("In CCStreamFile::CCStreamFile\n"));
	if (IOFile == NULL)
	{
		TRACE( _T("Null iostream* passed to CCStreamFile::CCStreamFile - is that OK?\n") );
		return;
	}

	// Possibly allocate a buffer.
/*	if (bufferSize > 0)
	{
		// Deallocate any existing buffer.
		if (!IOFile->rdbuf()->setbuf(0, 0))
		{
			TRACEUSER( "JustinF", _T("\t- Can't deallocate iostream buffer\n"));
			String_256 ErrorMsg(_R(IDE_BUFFER_ERROR));
			GotError(_R(IDE_BUFFER_ERROR), ErrorMsg);	// handle the error
			return;
   		}

		// Assign four extra bytes to workaround an MFC bug.
		buffer = new char[bufferSize +4];
		if (!buffer)
		{
			GotError(_R(IDE_NOMORE_MEMORY));
			return;
		}

		// Set the stream's buffer.
		if (!IOFile->rdbuf()->setbuf(buffer, bufferSize))
		{
			TRACEUSER( "JustinF", _T("\t- Can't allocate iostream buffer of %u bytes\n"),
						(UINT32) bufferSize + 4);
			String_256 ErrorMsg(_R(IDE_BUFFER_ERROR));
			GotError(_R(IDE_BUFFER_ERROR), ErrorMsg);	// handle the error
			return;
   		}
    }
*/
	// Everything is hunky-dory ...
	StreamFileInitialised = TRUE;
}



/********************************************************************************************

>	CCStreamFile::~CCStreamFile()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93        
	Purpose:	Default destructor. Closes file if it is still open.  Deallocates buffer.

********************************************************************************************/

CCStreamFile::~CCStreamFile()
{
	// If the compression flag is on then close down the compression system
//	TRACEUSER( "JustinF", _T("In CCStreamFile::~CCStreamFile\n"));
	if (CompressData) StopCompression();

	// These are the special classes we might use to compress the file.
	// If present then delete them.
	if (GZStream)
	{
		delete GZStream;
		GZStream = NULL; 
	}

	if (GZFile)
	{
		delete GZFile;
		GZFile = NULL; 
	}

	// Finally, deallocate the stream object.  Derived classes should make sure they have set
	// the IOFile pointer to null if they pass CCStreamFile an iostream that wasn't allocated
	// on the heap.
	if (IOFile != NULL)
	{
		delete IOFile;
		IOFile = NULL;
	}

	// Finally, deallocate the buffer, as the iostream doesn't.
/*	delete[] buffer;
	buffer = 0;				*/
}



/********************************************************************************************

>	BOOL CCStreamFile::IsInited()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/94
	Inputs:		None
	Returns:	TRUE if the contructor went without errors or FALSE otherwise.
	Purpose:	Allows errors to be returned from the constructor.
	SeeAlso:	CCStreamFile

********************************************************************************************/

BOOL CCStreamFile::IsInited()
{
	return StreamFileInitialised;
}



/********************************************************************************************
>	virtual iostream* CCStreamFile::SetStream(iostream* pStream)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pStream		-		the new iostream to attach this CCStreamFile to
	Returns:	Pointer to the previous iostream.  You should deallocate this sometime
				if you don't use it again in a different way.
	Purpose:	Changes the iostream that this CCStreamFile uses for I/O.  Don't call
				this unless you know what you're doing - it probably won't work with
				derived classes, such as CCDiskFile.
********************************************************************************************/

iostream* CCStreamFile::SetStream(iostream* pStream)
{
	iostream* pio = IOFile;
	IOFile = pStream;
	return pio;
}



/********************************************************************************************
>	virtual BOOL CCStreamFile::isOpen() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Returns:	TRUE if this CCFile is open.  CCStreamFiles are *always* open, provided they
				were constructed successfully.
	Purpose:	Gives the status of this CCStreamFile.
********************************************************************************************/

BOOL CCStreamFile::isOpen() const
{
	// Stream files are always open, and cannot be closed.
	return StreamFileInitialised && IOFile != NULL;
}



/********************************************************************************************
>	virtual void CCStreamFile::close()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Purpose:	Closes a CCStreamFile.  This is a no-op as CCStreamFiles are always open.
********************************************************************************************/

void CCStreamFile::close()
{
	TRACEUSER( "JustinF", _T("CCStreamFile::close called - I wonder why?\n") );	
}



/********************************************************************************************
>	CCFile& CCStreamFile::read(void* buf, UINT32 length)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		buf - buffer to place the bytes in.
				length - the number of bytes to read.
	Outputs:	buf - the data read from the file.
	Returns:	Reference to the CCFile object.
	Purpose:	Read a number of bytes from the file.
	Errors:		This function calls the error handling function (which could throw an
				Exception if the ThrowExceptions flag is set) if the fail flag of the
				file is set after the read.
********************************************************************************************/

CCFile &CCStreamFile::read( void *buf, UINT32 length )
{
/* #ifdef _DEBUG
	if (length > 1)
	{
		TRACEUSER( "JustinF", _T("In CCStreamFile::read(void* 0x%p, %u)\n"),
					(LPVOID) buf, (UINT32) length);
	}
#endif */
	if (CompressData)
	{
		if (GZStream !=NULL && GZFile != NULL)
		{
			// returns the number of uncompressed bytes actually read
			// (0 for end of file, -1 for error)
			INT32 RetValue = GZFile->gzread( GZStream, (char *)buf, length );
				
			if (RetValue < 0)
			{
				TRACEUSER( "JustinF", _T("Read error = %d\n"), RetValue );
				GZipErrorID = RetValue;
				GotError(_R(IDE_ZFILE_READ_ERROR));
			}
	
			if (RetValue == 0)
			{
				// found end of file so make sure we return an EOF as the data
				TCHAR* b = (TCHAR*) buf;
				*b = EOF;
				IOFile->clear(ios::eofbit);


			}

			if (IOFile->fail())
			{
				TRACEUSER( "JustinF", _T("Read fail error\n") );
				GotError(_R(IDE_FILE_READ_ERROR));
			}
		}
		else
		{
	        ERROR3("CCStreamFile::read bad GZFile/GZStream");
			GotError(_R(IDE_FILE_READ_ERROR));
		}
	}
	else
	{
		if (IOFile->read((char*) buf, length).fail()) GotError(_R(IDE_FILE_READ_ERROR));
	}
	
	// Update the CharsRead param
	CharsRead += length;

	// and return
	return *this;
}



/********************************************************************************************

>	CCFile& CCStreamFile::read(StringBase* buf)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		buf - the String object to place the characters in.
	Outputs:	buf - the characters read from the file.
	Returns:	Reference to the CCFile object.
	Purpose:	Read in a string from the file.  The read operation stops at the first
				newline, or when the string is full.  The newline is removed from the input
				stream.

********************************************************************************************/

CCFile& CCStreamFile::read(StringBase* buf)
{
	// Read characters in until we have MaxLength() of them, or we hit a new line
//	TRACEUSER( "JustinF", _T("In CCStreamFile::read(String* 0x%p)\n"), (LPVOID) buf);
	INT32 Max = buf->MaxLength();
	INT32 Off = 0;
	TCHAR* Text = (TCHAR*) (*buf);
	char Ch;

	// chars to look for
	const TCHAR CR = TEXT('\n');
	const TCHAR LF = TEXT('\r');

	// We have to do this sneeky test for end of file because the iostream classes
	// are a load of old bollox!
	if (CompressData)
	{
		// First check if our zipping classes are present, if not then fail 
		if (GZStream ==NULL && GZFile == NULL)
		{
	        ERROR3("CCStreamFile::read string bad GZFile/GZStream");
			GotError(_R(IDE_FILE_READ_ERROR));
		}

		// returns the number of uncompressed bytes actually read
		// (0 for end of file, -1 for error
		INT32 RetValue = GZFile->gzpeek( GZStream, &Ch );
			
		if (RetValue < 0)
		{
			TRACEUSER( "JustinF", _T("Peek error = %d\n"), RetValue );
			GZipErrorID = RetValue;
			GotError(_R(IDE_ZFILE_READ_ERROR));
		}

		if (RetValue == 0)
		{
			// found end of file so make sure we return an EOF as the data
			Ch = EOF;
		}

		if (IOFile->fail())
		{
			TRACEUSER( "JustinF", _T("Peek char fail error\n") );
			GotError(_R(IDE_FILE_READ_ERROR));
		}
	}
	else
	{
		Ch = IOFile->peek();
	}
	
	// check if found the EOF
	if (Ch == EOF)
	{
		// Mark the EOF has having been reached.
		IOFile->clear(ios::eofbit);

		// Its the end of the file all right
		Text[0] = TEXT('\0');
		return *this;
	}
	

	// Get the first char
	read(&Ch, 1);

	// loop around until we get to the end of the line or the end of the file
	if (CompressData)
	{
		// Must leave 1 character space in the buffer if we reach the maximum buffer size
		// so that we can fit the terminating zero in below.
		while ((Max>1) && (Ch!=CR) && (Ch!=LF) && (Ch!=EOF))
		{
			// store the char we read
			Text[Off++] = Ch;

			// read another one and decrement the counter
			read(&Ch, 1);

			// Keep track of how much we have read
			Max--;
		}

		if (GZStream ==NULL && GZFile == NULL)
		{
	        ERROR3("CCStreamFile::read string bad GZFile/GZStream");
			GotError(_R(IDE_FILE_READ_ERROR));
		}

		// returns the number of uncompressed bytes actually read
		// (0 for end of file, -1 for error
		INT32 RetValue = GZFile->gzpeek( GZStream, &Ch );
			
		if (RetValue < 0)
		{
			TRACEUSER( "JustinF", _T("Peek error = %d\n"), RetValue );
			GZipErrorID = RetValue;
			GotError(_R(IDE_ZFILE_READ_ERROR));
		}

		if (RetValue == 0)
		{
			// found end of file so make sure we return an EOF as the data
			Ch = EOF;
		}

		if (IOFile->fail())
		{
			TRACEUSER( "JustinF", _T("Peek char fail error\n") );
			GotError(_R(IDE_FILE_READ_ERROR));
		}
		
		if ((Ch==LF) || (Ch==CR))
		{
			// eat the other part of the CR LF combo
			read(&Ch, 1);
		}
		
		// See if we have reached the end of the file
		if (Ch==EOF)
		{
			// Mark the End Of File
			IOFile->clear(ios::eofbit);
		}
	}
	else
	{
		// Must leave 1 character space in the buffer if we reach the maximum buffer size
		// so that we can fit the terminating zero in below.
		while ((Max>1) && (Ch!=CR) && (Ch!=LF) && (IOFile->peek() != EOF))
		{
			// store the char we read
			Text[Off++] = Ch;

			// read another one and decrement the counter
			read(&Ch, 1);

			// Keep track of how much we have read
			Max--;
		}

		// if we ended at the end of a line, see if the next char is also part of a new line
		Ch = IOFile->peek();
		if ((Ch==LF) || (Ch==CR))
		{
			// eat the other part of the CR LF combo
			read(&Ch, 1);
		}

		// See if we have reached the end of the file
		if (Ch==EOF)
		{
			// Mark the End Of File
			IOFile->clear(ios::eofbit);
		}
	}

	// Terminate the string
	Text[Off++] = 0;

	// return
	return *this;
}



/********************************************************************************************

>	CCFile& CCStreamFile::read(TCHAR& buf)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Pointer to the TCHAR buffer.
	Outputs:	buf - the character read from the file.
	Returns:	Reference to the CCFile object.
	Purpose:	Read a character from the file.

********************************************************************************************/

CCFile &CCStreamFile::read( char &buf )
{
//	TRACEUSER( "JustinF", _T("In CCStreamFile::read(TCHAR)\n"));
	if (CompressData)
	{
		if (GZStream !=NULL && GZFile != NULL)
		{
			// returns the number of uncompressed bytes actually read
			// (0 for end of file, -1 for error
			char		outbuf;
			INT32 RetValue = GZFile->gzread( GZStream, &outbuf, 1 );
				
			if (RetValue < 0)
			{
				TRACEUSER( "JustinF", _T("Read error = %d\n"), RetValue );
				GZipErrorID = RetValue;
				GotError(_R(IDE_ZFILE_READ_ERROR));
			}
	
			if (RetValue == 0)
			{
				// found end of file so make sure we return an EOF as the data
				Ch = EOF;
			}

			if (IOFile->fail())
			{
				TRACEUSER( "JustinF", _T("Read char fail error\n") );
				GotError(_R(IDE_FILE_READ_ERROR));
			}

			buf = outbuf;
		}
		else
		{
	        ERROR3("CCStreamFile::read char bad GZFile/GZStream");
			GotError(_R(IDE_FILE_READ_ERROR));
		}
	}
	else
	{
		if (IOFile->get(buf).fail()) GotError(_R(IDE_FILE_READ_ERROR));
	}
	
	return *this;
}
	                                


/********************************************************************************************

>	CCFile& CCStreamFile::write(const StringBase& buf, UINT32 length = 0)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		buf - String buffer.
				length - number of characters to write.
						 (0 => write all of them up to the string terminator)
	Returns:	Reference to the CCFile object.
	Purpose:	Writes the number of charactors specified by length.
	SeeAlso:	fstream::write()

********************************************************************************************/

CCFile &CCStreamFile::write( const StringBase &buf, UINT32 length )
{
//	TRACEUSER( "JustinF", _T("In CCStreamFile::write(String* 0x%p, %u)\n"), (LPVOID) &buf, (UINT32) length);
	ENSURE((INT32) length <= buf.Length(), "CCStreamFile::write(): Not enough characters in string!");

#if 0 != wxUSE_UNICODE
	size_t				cch = camWcstombs( NULL, (const TCHAR *)buf, 0 ) + 1;
	PSTR				psz = PSTR( alloca( cch ) );
	camWcstombs( psz, (const TCHAR *)buf, cch );
#else
	PCSTR				psz = PCSTR(buf);
#endif
	
	if( 0 == length )
		length = (UINT32)strlen( psz );

	if (CompressData)
	{
		if (GZStream !=NULL && GZFile != NULL)
		{
			// returns the number of uncompressed bytes actually read
			// (0 for end of file, -1 for error
			INT32 RetValue = GZFile->gzwrite( GZStream, psz, DWORD(length) );
				
			if (RetValue < 0)
			{
				TRACEUSER( "JustinF", _T("Write error = %d\n"), RetValue );
				GZipErrorID = RetValue;
				GotError(_R(IDE_ZFILE_WRITE_ERROR));
			}
	
			if ((UINT32) RetValue != length)
			{
				TRACEUSER( "JustinF", _T("Wrote wrong length = %d\n"), RetValue );
				GotError(_R(IDE_FILE_WRITE_ERROR));
			}

			if (IOFile->bad())
			{
				TRACEUSER( "JustinF", _T("Write bad error\n") );
				GotError(_R(IDE_FILE_WRITE_ERROR));
			}
		}
		else
		{
	        ERROR3("CCStreamFile::write bad GZFile/GZStream");
			GotError(_R(IDE_FILE_READ_ERROR));
		}
	}
	else
	{
		if( IOFile->write( psz, DWORD(length) ).bad() )
			GotError( _R(IDE_FILE_WRITE_ERROR) );
	}
	
	return *this;
}



/********************************************************************************************

>	CCFile& CCStreamFile::write(const void *buf, UINT32 length)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		buf - pointer to the bytes to write.
				length - the number of bytes to write.
	Returns:	Reference to the CCFile object.
	Purpose:	Write a number of bytes to the file.

********************************************************************************************/

CCFile& CCStreamFile::write(const void* buf, UINT32 length)
{
/* #ifdef _DEBUG
	if (length > 1)
	{
		TRACEUSER( "JustinF", _T("In CCStreamFile::write(void* 0x%p, %u)\n"),
					(LPVOID) buf, (UINT32) length);
	}
#endif */

	if (CompressData)
	{
		if (GZStream !=NULL && GZFile != NULL)
		{
			// returns the number of uncompressed bytes actually read
			// (0 for end of file, -1 for error
			INT32 RetValue = GZFile->gzwrite( GZStream, PSTR(buf), length );
				
			if (RetValue < 0)
			{
				TRACEUSER( "JustinF", _T("Write error = %d\n"), RetValue );
				GZipErrorID = RetValue;
				GotError(_R(IDE_ZFILE_WRITE_ERROR));
			}
	
			if ((UINT32) RetValue != length)
			{
				TRACEUSER( "JustinF", _T("Wrote wrong length = %d\n") ,RetValue );
				GotError(_R(IDE_FILE_WRITE_ERROR));
			}

			if (IOFile->bad())
			{
				TRACEUSER( "JustinF", _T("Write bad error\n") );
				GotError(_R(IDE_FILE_WRITE_ERROR));
			}
		}
		else
		{
	        ERROR3("CCStreamFile::write bad GZFile/GZStream");
			GotError(_R(IDE_FILE_READ_ERROR));
		}
	}
	else
	{
		if( IOFile->write( PCSTR(buf), length).bad() )
			GotError( _R(IDE_FILE_WRITE_ERROR) );
	}

	return *this;
}



/********************************************************************************************

>	CCFile& CCStreamFile::write(TCHAR& buf)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		buf - the character to write out.
	Returns:	Reference to the CCFile object.
	Purpose:	Writes a character out to the file.

********************************************************************************************/

CCFile &CCStreamFile::write( char &buf )
{
//	TRACEUSER( "JustinF", _T("In CCStreamFile::write(TCHAR %c)\n"), (TCHAR) buf);
	if (CompressData)
	{
		if (GZStream !=NULL && GZFile != NULL)
		{
			// returns the number of uncompressed bytes actually read
			// (0 for end of file, -1 for error
			INT32 RetValue = GZFile->gzwrite( GZStream, &buf, 1 );
				
			if (RetValue <= 0 || IOFile->bad())
			{
				TRACEUSER( "JustinF", _T("Write char error = %d\n"), RetValue );
				GZipErrorID = RetValue;
				GotError(_R(IDE_ZFILE_WRITE_ERROR));
			}
	
			if ((UINT32) RetValue != 1)
			{
				TRACEUSER( "JustinF", _T("Wrote wrong length = %d\n"), RetValue );
				GotError(_R(IDE_FILE_WRITE_ERROR));
			}

			if (IOFile->bad())
			{
				TRACEUSER( "JustinF", _T("Write char bad error\n") );
				GotError(_R(IDE_FILE_WRITE_ERROR));
			}
		}
		else
		{
	        ERROR3("CCStreamFile::write char bad GZFile/GZStream");
			GotError(_R(IDE_FILE_READ_ERROR));
		}
	}
	else
	{
		if (IOFile->put(buf).bad())
			GotError(_R(IDE_FILE_WRITE_ERROR));
	}
	
	return *this;
}



/********************************************************************************************

>	CCFile& CCStreamFile::seekIn(FilePos pos)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		FilePos - a position in the file
	Returns:	Reference to the CCFile object.
	Purpose:	Sets the input pointer to the file position passed in.
	SeeAlso:	istream::seekg()

********************************************************************************************/

CCFile& CCStreamFile::seekIn(FilePos pos)
{
	if (CompressData)
	{
		// This is not implemented in compressed files so error
		ERROR3("seekIn called in Compressed mode");
		GotError(_R(IDE_FILE_READ_ERROR));
		return *this;
	}
	else
	{
		// Stuff to sort a seeking lexer out
		if (IsLexerInitialised() && SeekingRequired)
		{
			Ch = 0;
			Ofs = 0;
			LastLinePos = pos;
		}

		IOFile->seekg(pos);
		return *this;
	}
}



/********************************************************************************************

>	CCFile& CCStreamFile::seekIn(INT32 Offset, ios::seek_dir Dir)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		Offset - the file offset to seek to.
				Dir - specifies where in the file the offset is relative to.
					  (can be ios::beg, ios::cur, or ios::end).
	Returns:	Reference to the CCFile object.
	Purpose:	Sets the input pointer to the offset passed in, relative to the specified
				position in the file.
	SeeAlso:	istream::seekg()

********************************************************************************************/

CCFile& CCStreamFile::seekIn(INT32 Offset, ios::seekdir Dir)
{
	if (CompressData)
	{
		// This is not implemented in compressed files so error
		ERROR3("seekIn (Offset, dir) called in Compressed mode");
		GotError(_R(IDE_FILE_READ_ERROR));
	}
	else
	{
		IOFile->seekg( Offset, ios_base::seekdir(Dir) );

		// Stuff to sort the lexer out
		if (IsLexerInitialised() && SeekingRequired)
		{
			Ch = 0;
			Ofs = 0;
			LastLinePos = IOFile->tellg();
		}
	}

	return *this;
}



/********************************************************************************************

>	FilePos CCStreamFile::tellIn()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Returns:	FilePos - a position in the file
	Purpose:	Gets the input pointer file position.
	SeeAlso:	istream::tellg()

********************************************************************************************/

FilePos CCStreamFile::tellIn()
{           
	if (CompressData)
	{
		// This is not implemented in compressed files so error
		//ERROR3("tellIn called in Compressed mode");
		//GotError(_R(IDE_FILE_READ_ERROR));
		return (FilePos) IOFile->tellg();
	}
	else
	{
		if (IsLexerInitialised())
		{
			ERROR3IF(!SeekingRequired,
						"CCStreamFile::tellIn() will only work properly with seeking enabled");

			// -1 for the one character lookahead that's in operation
			return (FilePos) (Ofs + (INT32) LastLinePos - 1);
		}

		return (FilePos) IOFile->tellg();
	}
}


             
/********************************************************************************************

>	CCFile& CCStreamFile::seek(FilePos pos)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		FilePos - a position in the file
	Returns:	Reference to the CCFile object.
	Purpose:	Sets the output pointer to the file position passed in.
	SeeAlso:	ostream::seekp()

********************************************************************************************/

CCFile& CCStreamFile::seek(FilePos pos)
{                         
	if (CompressData)
	{
		// This is not implemented in compressed files so error
		ERROR3("seek called in Compressed mode");
		GotError(_R(IDE_FILE_READ_ERROR));
	}
	else
		IOFile->seekp(pos);

	return *this;
}



/********************************************************************************************

>	FilePos CCStreamFile::tell()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Returns:	FilePos - a position in the file
	Purpose:	Gets the output pointer file position.
	SeeAlso:	ostream::tellp()

********************************************************************************************/

FilePos CCStreamFile::tell()
{           
	if (CompressData)
	{
		// This is not implemented in compressed files so error
		ERROR3("tell called in Compressed mode");
		GotError(_R(IDE_FILE_READ_ERROR));
	}

	return (FilePos) IOFile->tellp();
}


             
/********************************************************************************************

>	CCFile&	CCStreamFile::flush()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Returns:	Reference to the CCFile object.
	Purpose:	Clears the io buffer
	SeeAlso:	iostream::flush

********************************************************************************************/

CCFile&	CCStreamFile::flush()
{
	if (CompressData)
	{
		if (GZStream !=NULL && GZFile != NULL)
		{
			// returns the number of uncompressed bytes actually read
			// (0 for end of file, -1 for error
			INT32 RetValue = GZFile->gzflush(GZStream, Z_FULL_FLUSH);
				
			if (RetValue < 0)
			{
				TRACEUSER( "JustinF", _T("flush error = %d\n"), RetValue );
				GZipErrorID = RetValue;
				GotError(_R(IDE_ZFILE_READ_ERROR));
			}
	
			if (IOFile->fail())
			{
				TRACEUSER( "JustinF", _T("flush fail error\n") );
				GotError(_R(IDE_FILE_READ_ERROR));
			}
		}
		else
		{
	        ERROR3("CCStreamFile::flush has bad GZFile/GZStream");
			GotError(_R(IDE_FILE_READ_ERROR));
		}

		// now flush the main IOFIle itself 
		IOFile->flush();
	}
	else
		IOFile->flush();

	return *this;
}

    

/********************************************************************************************

>	CCFile& CCStreamFile::get(TCHAR& buf)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Outputs:	Single charactor retrieved by get.
	Returns:	Reference to the CCFile object.
	Purpose:	Retrieves a single charactor from the file.
				If the compression flag is on then it gets the compressed byte from the file
				and decompresses it.
	SeeAlso:	fstream::operator<<()

********************************************************************************************/

CCFile& CCStreamFile::get( char &buf )
{
//	TRACEUSER( "JustinF", _T("In CCStreamFile::get(TCHAR)\n"));
	if (CompressData)
	{
		if (GZStream !=NULL && GZFile != NULL)
		{
			// returns the number of uncompressed bytes actually read
			// (0 for end of file, -1 for error
			char		outbuf;
			INT32 RetValue = GZFile->gzread( GZStream, &outbuf, 1 );
				
			if (RetValue < 0)
			{
				TRACEUSER( "JustinF", _T("get error = %d\n"), RetValue );
				GZipErrorID = RetValue;
				GotError(_R(IDE_ZFILE_READ_ERROR));
			}
	
			if (RetValue == 0)
			{
				// end of file reached so return the EOF character
				buf = EOF;
			}
			else
				buf = outbuf;
		}
		else
		{
	        ERROR3("CCStreamFile::get bad GZFile/GZStream");
			GotError(_R(IDE_FILE_READ_ERROR));
		}
	}
	else
		IOFile->get( buf );
	
	return *this;
}



/********************************************************************************************

>	CCFile& CCStreamFile::put(const TCHAR& buf)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		Single charactor to be inserted into file.
	Returns:	Reference to the CCFile object.
	Purpose:	Inserts a single charactor into the file.
				If the compression flag is on then it sends it out compressed to file.
	SeeAlso:	fstream::operator<<()

********************************************************************************************/

CCFile &CCStreamFile::put( const char &buf )
{
//	TRACEUSER( "JustinF", _T("In CCStreamFile::put(TCHAR %c)\n"), (TCHAR) buf);
	if (CompressData)
	{
		if (GZStream !=NULL && GZFile != NULL)
		{
			// returns the number of uncompressed bytes actually read
			// (0 for end of file, -1 for error
			char		outbuf = buf;		
			INT32 RetValue = GZFile->gzwrite( GZStream, &outbuf, 1 );
				
			if (RetValue < 0)
			{
				TRACEUSER( "JustinF", _T("put error = %d\n"), RetValue );
				GZipErrorID = RetValue;
				GotError(_R(IDE_ZFILE_WRITE_ERROR));
			}
	
			if ((UINT32) RetValue != 1)
			{
				TRACEUSER( "JustinF", _T("put wrong length = %d\n"), RetValue );
				GotError(_R(IDE_FILE_WRITE_ERROR));
			}
		}
		else
		{
	        ERROR3("CCStreamFile::put bad GZFile/GZStream");
			GotError(_R(IDE_FILE_WRITE_ERROR));
		}
	}
	else
		IOFile->put(buf);
		
	return *this;
}



/********************************************************************************************

>	BOOL CCStreamFile::good() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Returns:	True if file status is healthy and FALSE otherwize.
	Purpose:	To identify if file is ok
	SeeAlso:	ios::good

********************************************************************************************/

BOOL CCStreamFile::good() const
{
	return IOFile->good();
}



/********************************************************************************************

>	BOOL CCStreamFile::bad() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Returns:	True if a serious I/O error occurs and FALSE otherwize.
	Purpose:	To identify an erroneous file status
	SeeAlso:	ios::bad

********************************************************************************************/

BOOL CCStreamFile::bad() const
{
	// If we are about to return True then make sure we call set error
	// as the caller may not have done this and hence a random error
	// may be displayed
	if (IOFile->bad()) Error::SetError( _R(IDE_IO_ERROR) );
	return IOFile->bad();
}



/********************************************************************************************

>	BOOL CCStreamFile::fail() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Returns:	True if a recoverable fail error occurs and FALSE otherwize.
	Purpose:	To identify recoverable file errors
	SeeAlso:	ios::fail

********************************************************************************************/

BOOL CCStreamFile::fail() const
{
	return IOFile->fail();
}



/********************************************************************************************

>	BOOL CCStreamFile::eof() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Returns:	True if end of file and FALSE otherwize.
	Purpose:	To identify end of file
	SeeAlso:	ios::eof

********************************************************************************************/

BOOL CCStreamFile::eof() const
{
	return IOFile->eof();
}



/********************************************************************************************

>	BOOL CCStreamFile::SetBadState()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/08/94
	Returns:	None.
	Purpose:	It allows the forceable setting of the test that functions like bad() use
				so the next check of the current file status to see if we should continue
				using the file or not will fail. This will be called by the GotError function
				so that any attempts to read/write after this should fail.
	SeeAlso:	CCFile::GotError

********************************************************************************************/

void CCStreamFile::SetBadState()
{
	// All the status functions should just return the state of eof()
	// Therefore, just make sure that eof() will fail. 
	IOFile->clear(IOFile->badbit);				// remember clear == set badbit
}



/********************************************************************************************

>	BOOL CCStreamFile::SetGoodState()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/94
	Returns:	None.
	Purpose:	It allows the forceable setting of the test that functions like bad() use
				so the next check of the current file status to see if we should continue
				using the file or not will pass. It would seem that sometimes the eof() or
				similar might need forcing by somebody using the class. 
	SeeAlso:	CCFile::GotError

********************************************************************************************/

void CCStreamFile::SetGoodState()
{
	// Nuke any existing error states that might exist e.g. a lying eof() bit
	IOFile->clear(IOFile->goodbit);
}



/********************************************************************************************
>	virtual INT32 CCStreamFile::Size()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/96
	Returns:	The size of the attached stream, in bytes.
********************************************************************************************/

size_t CCStreamFile::Size()
{
	// We can't really do this for any stream, but we can have a bodgy bash.  We'll subtract
	// the start position from the end position of the file.
	streampos spOrigin = IOFile->tellg();
	size_t sz = IOFile->seekg(0, ios::end).tellg() - IOFile->seekg(0, ios::beg).tellg();
	IOFile->seekg(spOrigin);
	return sz;
}



/********************************************************************************************

>	virtual BOOL CCStreamFile::InitCompression(BOOL Header = FALSE)


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95
	Purpose:	To initialise the compression system ready for use. This is to make sure
				there is enough memory available for its buffers before writing out any
				compression tokens into the output file.
				Usually used for writing only. Reading, there should be no similar
				requirement.
				Graeme (15/11/99) - Added the Header parameter to the function to allow
				compression to use the ZLib header. Previously, the CCFile functions were
				hardwired to supress this, but Flash export needs it.
	SeeAlso:	StartCompression; StopCompression

********************************************************************************************/

BOOL CCStreamFile::InitCompression(BOOL Header)
{
//	TRACEUSER( "JustinF", _T("CCStreamFile::InitCompression\n"));

	// We first need to create the classes that we will be using
	GZFile = new GZipFile;
	if (GZFile == NULL)
	{
		return FALSE;
	}

	// Can't seem to get to the read/write (in/out) flag states via IOFile.flags()
	// so use stored versions
	String_16			filemode;

	//return CCStreamFile::isOpen() && cs.is_open();
	
	if (ModeOfFile & ios::out)		filemode = _T("w");
	else if (ModeOfFile & ios::in)	filemode = _T("r");
	else ERROR3("filemode incorrect as neither in or out");	

	// Add in the b if we are in binary mode
	if (ModeOfFile & ios::binary) filemode += _T("b");

	GZStream = GZFile->gz_init( IOFile, (TCHAR*) filemode, Header );		
	if (GZStream == NULL)
	{
		// Remove the class that we allocated
		delete GZFile;
		GZFile = NULL;
		return FALSE;
	}
	
	// Everything went ok
	return TRUE;	
}



/********************************************************************************************

>	virtual BOOL CCStreamFile::StartCompression()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Purpose:	To start up the compression system.
				If we are writing then should have called Init before so that we have
				a chance to fail BEFORE the compression tokens are written
				If we are reading then no need to do this.
	SeeAlso:	StopCompression

********************************************************************************************/

BOOL CCStreamFile::StartCompression()
{
//	TRACEUSER( "JustinF", _T("CCStreamFile::StartCompression\n"));

	// We first need to create the classes that we will be using
	// Only create the GZFile class if not already present as in the writing case
	// we will have called Init and it will be present 
	if (GZFile == NULL) GZFile = new GZipFile;
	if (GZFile == NULL)
	{
		return FALSE;
	}

	// Can't seem to get to the read/write (in/out) flag states via IOFile.flags()
	// so use stored versions
	String_16			filemode;
	
	if( ModeOfFile & ios::out )
	{
		filemode = _T("w");
	}
	else
	if( ModeOfFile & ios::in )
	{
		filemode = _T("r");
	}
	else
	{
		ERROR3("filemode incorrect as neither in or out");	
	}
	
	// Add in the b if we are in binary mode
	if( ModeOfFile & ios::binary )
	{
		filemode += _T("b");
	}
	
	// Pass in the leafname of the file
	GZipErrorID = Z_OK;

	// If writing then should have called Init before so that we have a chance to 
	// fail BEFORE the compression tokens are written
	if (GZStream != NULL)
	{
		// In the write case Init has been called so just open the file
		INT32 RetValue = GZFile->gz_open(GZStream);
		if (RetValue < 0)
		{
			// Only set the error if it is not our special we have found uncompressed
			// data error
			if (RetValue == Z_UNCOMPRESSED_ERROR)
			{
				// Found uncompressed data so stop compression
				// Remove the class that we allocated
				delete GZStream;
				GZStream = NULL;
				delete GZFile;
				GZFile = NULL;
				CompressData = FALSE;	

				return TRUE;
			}
			else
			{
				// Some other error has happened so remember this error
				GZipErrorID = RetValue;
			}
		}
	}
	else
	{
		// In the read case Init will not have been called so we use the full
		// version of open.
		GZStream = GZFile->gz_open( IOFile, (TCHAR *)filemode, NULL );		// no paths please!
		if (GZStream == NULL)
		{
			// Remove the class that we allocated
			delete GZFile;
			GZFile = NULL;

			// Return False so that we error
			return FALSE;
		}

		// If it has given our special error then we have found uncompressed
		// data and should just stop the compression process
		if (GZStream->z_err == Z_UNCOMPRESSED_ERROR)
		{
			delete GZStream;
			GZStream = NULL;

			// Remove the class that we allocated
			delete GZFile;
			GZFile = NULL;

			// Make out that compression did start up ok otherwise it will error.
			// But ensure compression is off
			CompressData = FALSE;	
			return TRUE;
		}
	}

	if (GZStream->z_err != Z_OK) GZipErrorID = GZStream->z_err;

	// check if an error was detected in opening
	if (GZipErrorID != Z_OK)
	{
		// Work out if we are reading or writing and use the corresponding
		// correct error message
		UINT32 ErrorID = _R(IDE_ZFILE_READ_ERROR);
		if (ModeOfFile & ios::out)		ErrorID = _R(IDE_ZFILE_WRITE_ERROR);
		else if (ModeOfFile & ios::in)	ErrorID = _R(IDE_ZFILE_READ_ERROR);
		GotError(ErrorID);
	}
	
	return TRUE;	
}	



/********************************************************************************************

>	virtual BOOL CCStreamFile::StopCompression()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Purpose:	To stop the compression system.
	SeeAlso:	StartCompression

********************************************************************************************/

BOOL CCStreamFile::StopCompression()
{
//	TRACEUSER( "JustinF", _T("CCStreamFile::StopCompression\n"));

	BOOL Ok = TRUE;

	// Try and close down the system
	if (GZFile && GZStream)
	{
		INT32 RetValue = GZFile->gzclose(GZStream);

		// The close should have removed the GZStream.
		// Must set our pointer to NULL overwise there will be problems! 
		GZStream = NULL; 
//		TRACEUSER( "JustinF", _T("CCStreamFile::StopCompression gzclose returned %d\n"),RetValue);

		if (RetValue < 0)
		{
			GZipErrorID = RetValue;
			// Work out if we are reading or writing and use the corresponding
			// correct error message
			UINT32 ErrorID = _R(IDE_ZFILE_READ_ERROR);
			if (ModeOfFile & ios::out)		ErrorID = _R(IDE_ZFILE_WRITE_ERROR);
			else if (ModeOfFile & ios::in)	ErrorID = _R(IDE_ZFILE_READ_ERROR);
			
			// Set up a friendly form of the error 
			SetZipFileErrorMessage(ErrorID);

			// return the fact to the caller
			Ok = FALSE;	
		}

		// Now try and clean up our special classes for compressing the file.
		if (GZFile)
		{
			delete GZFile;
			GZFile = NULL; 
		}
	}

	return Ok;
}



/********************************************************************************************

>	virtual	INT32 CCStreamFile::GetCharsRead()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Purpose:	To make sure the progress bar does not go over the end
	SeeAlso:	LexFile::GetCharsRead

********************************************************************************************/
INT32 CCStreamFile::GetCharsRead()
{
	// This is then used to update the progress bar on EPS import. 
	// A progress bar is started with the file size.
	// Hence, if we are compressing/decompressing we must not return the true value

	// If not compressing then just do what the lexfile does which is return the
	// number of characters read so far.
	if (!CompressData) return CharsRead;

	// Return the current position in the file. This must be the present
	if (GZFile && GZStream)
	{
		// Get the file stream class to tell us where it is currently in the compressed file
		// Must be the compressed file as the progress bar is started with the actual size of the
		// file i.e. compressed size. 
		return GZFile->GetCurrentFilePos(GZStream);
	}	
	else
		return (INT32) IOFile->tellg();	
}



/********************************************************************************************
>	virtual filedesc CCStreamFile::GetFileHandle() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/96
	Returns:	A "file-handle" for this CCStreamFile.  This is faked for plain iostreams.
	SeeAlso:	CCDiskFile::GetFileHandle
********************************************************************************************/

filedesc CCStreamFile::GetFileHandle() const
{
	return (filedesc) fake_filedesc(*IOFile);
}


PORTNOTE("other","Removed OLE stuff")
#ifndef EXCLUDE_FROM_XARALX
#ifndef RALPH

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
////
////	CCOleStream
////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************
>	CCOleStream::CCOleStream(IStream* pIStream, UINT32 nBufSize, BOOL fReport, BOOL fThrow)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/96
	Inputs:		pIStream	-	the IStream interface to attach to this CCOleStream.  This
								will be used for all I/O.
	Purpose:	Constructs a CCOleStream attached to the given IStream, or not.
********************************************************************************************/

CCOleStream::CCOleStream(IStream* pIStream, UINT32 cbSize, BOOL fReport, BOOL fThrow)
  :	CCStreamFile(new costream(pIStream), cbSize, fReport, fThrow)
{
	// Work out the access mode, if any.
//	TRACEUSER( "JustinF", _T("In CCOleStream::CCOleStream\n"));
//	IOFile = NULL;

	if (isOpen()) GetCoStream().get_access_mode(&ModeOfFile, 0);
}



/********************************************************************************************
>	costream& CCOleStream::GetCoStream()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/77
	Inputs:		-
	Purpose:	I don't know why but things seem to be going awire!  Basically, camelot has
				developed a severe BUG; and doing the following:  edit-copy, file-exit;
				causes it to throw a wobobley!  The only thing that I managed to find that
				was responsible for causing this was the inline function
				CCOleStream::GetCoStream().  The original implementation of this function
				should be fine (and had been fine for about four years); but (like most
				things) it seems to have stopped working overnight - in the form of a nasty
				access violation!  Anyway, I've made the function not-inline; and it seems
				to have solved the bug (and the fellow line on 4021).  All I can hazard a
				guess at is that the compiler was getting things wrong !!!!
********************************************************************************************/


costream& CCOleStream::GetCoStream() const
{
	return ((costream&) *IOFile);
}



/********************************************************************************************
>	CCOleStream::CCOleStream(PathName path, INT32 fileMode,
							 INT32 fileAccess = filebuf::openprot,
							 UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     						 BOOL ErrorReporting = TRUE,
     						 BOOL ExceptionThrowing = FALSE)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/96
	Inputs:		path		-		the non-structured storage file to open.
				fileMode	-		the opening mode, eg. ios::in
				fileAccess	-		the sharing mode, eg. costream::sh_none
				  etc
	Purpose:	Opens the given "old" file in conversion mode, using the STGM_CONVERT
				access mode.  See the OLE docs for an explanation of this.
	SeeAlso:	costream::open_file; STGM_CONVERT; StgCreateDocfile
********************************************************************************************/

CCOleStream::CCOleStream(PathName path, INT32 nMode, INT32 nAccess, UINT32 cbSize,
						 BOOL fReport, BOOL fThrow)
  :	CCStreamFile(new costream(path.GetPath(), nMode, nAccess), cbSize, fReport, fThrow)
{
//	TRACEUSER( "JustinF", _T("In CCOleStream::CCOleStream\n"));
	ModeOfFile = nMode;
}



/********************************************************************************************
>	virtual BOOL CCOleStream::isOpen() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/96
	Returns:	TRUE if this CCOleStream is currently open, ie. attached to an IStream.
	Purpose:	Returns the status of this file object.
********************************************************************************************/

BOOL CCOleStream::isOpen() const
{
	// the inline function GetCoStream seems to have been fouling this up ....
	// so we are doing this for now ....
	
	const costream& cs = (const costream&) *IOFile;
	return CCStreamFile::isOpen() && cs.is_open();

	// instead of this ....
	
	//return CCStreamFile::isOpen() && GetCoStream().is_open();
}



/********************************************************************************************
>	virtual BOOL CCOleStream::open_file(PathName path, INT32 fileMode,
										INT32 fileAccess = filebuf::openprot)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/96
	Inputs:		path		-		the non-structured storage file to open.
				fileMode	-		the opening mode, eg. ios::in
				fileAccess	-		the sharing mode, eg. costream::sh_none
	Returns:	TRUE if successful, FALSE if something goes wrong.
	Purpose:	Opens the given "old" file in conversion mode, using the STGM_CONVERT
				access mode.  See the OLE docs for an explanation of this.
	SeeAlso:	costream::open_file; STGM_CONVERT; StgCreateDocfile
********************************************************************************************/

BOOL CCOleStream::open_file(PathName path, INT32 fileMode, INT32 fileAccess)
{
	// Sanity check.
//	TRACEUSER( "JustinF", _T("In CCOleStream::open_file(%s)\n"), (LPCTSTR) path.GetPath());
	ERROR2IF(!path.IsValid(), FALSE, "Invalid PathName in CCOleStream::open_file");
	ERROR2IF(!IsInited(), FALSE, "CCOleStream not initiallised in CCOleStream::open_file");

	// Nuke any existing open error states that might exist
	IOFile->clear(IOFile->goodbit);

	// If we are not opening this file for writing then set the ios::nocreate
	// flag due to a bug in the iostream classes (but not if the ios::out flag is specified
	// as well. If this is not set we can not open files on read-only mounts shared by windows 95.
	if ((fileMode & ios::in) && !(fileMode & ios::out)) fileMode |= ios::nocreate;

	// Set the sharing mode if the default was passed.
	if (fileAccess == filebuf::openprot) fileAccess = costream::sh_read | costream::sh_write;

	// Note the current opening mode.
	ModeOfFile = fileMode;

	// Try to open the stream.
	if (FAILED(GetCoStream().open_file(path.GetPath(), fileMode | ios::binary, fileAccess)))
	{
		// TO DO: fix this to be a more appropriate error message.
		GotError(_R(IDE_OPEN_ERROR));
       	return FALSE;
	}

	// Success.
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL CCOleStream::attach(IStream* pIStream)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/96
	Inputs:		pIStream	-	the IStream interface to connect to this CCOleStream.  This
								will be used for all subsequent I/O.
	Returns:	TRUE if successful, FALSE if not.
	Purpose:	Attaches this CCOleStream to the IStream.
********************************************************************************************/

BOOL CCOleStream::attach(IStream* pIStream)
{
//	TRACEUSER( "JustinF", _T("In CCOleStream::attach\n"));
	return SUCCEEDED(GetCoStream().attach(pIStream));
}



/********************************************************************************************
>	void CCOleStream::close()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/96
	Purpose:	Closes this CCOleStream.  You must call attach with another IStream before
				you perform I/O on it again.
	SeeAlso:	CCOleStream::attach
********************************************************************************************/

void CCOleStream::close()
{
//	TRACEUSER( "JustinF", _T("In CCOleStream::close\n"));
	GetCoStream().close();
}



/********************************************************************************************
>	virtual INT32 CCOleStream::Size()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/96
	Returns:	The size of this IStream, in bytes.
********************************************************************************************/

INT32 CCOleStream::Size()
{
	// Size of what???
	if (!isOpen())
	{
		ERROR3("CCOleStream::Size called when stream is not open");
		return 0;
	}
	
	// Go direct to the IStream for this.
	STATSTG statstg;
	HRESULT hr = ((IStream*) GetCoStream())->Stat(&statstg, STATFLAG_NONAME);
	if (FAILED(hr))
	{
		ERROR3("CCOleStream::Size failed - why?");
		return 0;
	}

	// We can only return the lower 31 bits (doh!)
	return (INT32) statstg.cbSize.LowPart;
}



/********************************************************************************************
>	virtual BOOL CCOleStream::setMode(INT32)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/96
	Returns:	TRUE
	Purpose:	Set this CCOleStream's mode, either text or binary.  Does nothing, as
				CCOleStreams only support binary mode.
********************************************************************************************/

BOOL CCOleStream::setMode(INT32)
{
	TRACEUSER( "JustinF", _T("CCOleStream::setMode called - is this OK?\n") );
	return TRUE;
}

// WEBSTER-Martin-29/12/96
// no accusoft stuff
#ifndef WEBSTER

/********************************************************************************************
>	CCOleAccusoftStream::CCOleAccusoftStream(IStream* pIStream = 0,
											 UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     										 BOOL ErrorReporting = TRUE,
     										 BOOL ExceptionThrowing = FALSE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/96
	Inputs:		See CCOleStream::CCOleStream.
	Purpose:	Like its base class, CCOleStream, but redirects the Accusoft libraries to
				use a different stream as well.
	SeeAlso:	CCOleAccusoftStream::~CCOleAccusoftStream
********************************************************************************************/

CCOleAccusoftStream::CCOleAccusoftStream(IStream* pIStream, UINT32 bufferSize,
										 BOOL ErrorReporting, BOOL ExceptionThrowing)
  :	CCOleStream(pIStream, bufferSize, ErrorReporting, ExceptionThrowing)
{
//	TRACEUSER( "JustinF", _T("CCOleAccusoftStream redirecting Accusoft ...\n"));
	AccusoftFilters::RedirectStream();
}



/********************************************************************************************
>	CCOleAccusoftStream::CCOleAccusoftStream(PathName path, INT32 fileMode,
											 INT32 fileAccess = filebuf::openprot,
											 UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     										 BOOL ErrorReporting = TRUE,
     										 BOOL ExceptionThrowing = FALSE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/96
	Inputs:		See CCOleStream::CCOleStream.
	Purpose:	Like its base class, CCOleStream, but redirects the Accusoft libraries to
				use a different stream as well.
	SeeAlso:	CCOleAccusoftStream::~CCOleAccusoftStream
********************************************************************************************/

CCOleAccusoftStream::CCOleAccusoftStream(PathName path, INT32 nMode, INT32 nAccess,
										 UINT32 cbSize, BOOL fReport, BOOL fThrow)
  :	CCOleStream(path, nMode, nAccess, cbSize, fReport, fThrow)
{
//	TRACEUSER( "JustinF", _T("CCOleAccusoftStream redirecting Accusoft ...\n"));
	AccusoftFilters::RedirectStream();
}



/********************************************************************************************
>	virtual CCOleAccusoftStream::~CCOleAccusoftStream()


	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/96
	Purpose:	
	SeeAlso:	CCOleAccusoftStream::CCOleAccusoftStream
********************************************************************************************/

CCOleAccusoftStream::~CCOleAccusoftStream()
{
	AccusoftFilters::UnredirectStream();
//	TRACEUSER( "JustinF", _T("CCOleAccusoftStream un-redirecting Accusoft ...\n"));
}

#endif // WEBSTER

#endif // RALPH

#endif // !wx PORT


//-----------------------
/********************************************************************************************

										CCDiskFile

********************************************************************************************/
//-----------------------



/********************************************************************************************

>	CCDiskFile::CCDiskFile(UINT32 bufferSize,
						   BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> (Altered by Neville 01/09/94)
	Created:	1/9/93        
	Inputs:		bufferSize	- Size of the fixed size buffer (default = 1024)
							- if size > 0 then create a buffer of the specified size
							- if size = 0 then create a file without a buffer
				ReportErrors flag
				ThrowException flag
	Purpose:	Default constructor.
	SeeAlso:	fstream::setbuf()

********************************************************************************************/

CCDiskFile::CCDiskFile(UINT32 bufferSize,
					   BOOL ErrorReporting,
					   BOOL ExceptionThrowing)
  :	CCStreamFile(new fstream, bufferSize, ErrorReporting, ExceptionThrowing)
{
//	TRACEUSER( "JustinF", _T("In CCDiskFile::CCDiskFile\n"));
}


/********************************************************************************************

>	CCDiskFile::CCDiskFile(UINT32 bufferSize,
						   BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> (Altered by Neville 01/09/94)
	Created:	1/9/93        
	Inputs:		bufferSize	- Size of the fixed size buffer (default = 1024)
							- if size > 0 then create a buffer of the specified size
							- if size = 0 then create a file without a buffer
				ReportErrors flag
				ThrowException flag
	Purpose:	Default constructor.
	SeeAlso:	fstream::setbuf()

********************************************************************************************/

CCDiskFile::CCDiskFile(fstream *pfstream,
					   UINT32 bufferSize,
					   BOOL ErrorReporting,
					   BOOL ExceptionThrowing)
  :	CCStreamFile(pfstream, bufferSize, ErrorReporting, ExceptionThrowing)
{                      
	// Empty.
}

/********************************************************************************************

>	CCDiskFile(PathName fPath, INT32 fileMode, 
			   INT32 fileAccess  = filebuf::openprot,
			   UINT32 bufferSize = CCFILE_DEFAULTSIZE
			   BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE);

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>		(Altered by Neville 01/09/94)
	Created:	1/9/93
	Inputs:		fPath		- PathName for file to be opened
				fileMode	- Mode in which file is to be opened - use ios flags  
				fileAccess	- file access flag(s) as specified by the filebuf flags
				bufferSize	- Size of the fixed size buffer (default = 1024)
							- if size > 0 then create a buffer of the specified size
							- if size = 0 then create a file without a buffer
				ReportErrors flag
				ThrowExceptions flag
	Purpose:	Constructs a CCDiskFile object and opens the file specified by the Path over it.
	SeeAlso:	fstream::setbuf()
	SeeAlso:	fstream::open()					 
	
********************************************************************************************/

CCDiskFile::CCDiskFile(fstream *pfstream,
					   PathName fPath, 
					   INT32 fileMode, 
					   INT32,
					   UINT32 bufferSize,
					   BOOL ErrorReporting,
					   BOOL ExceptionThrowing)
  :	CCStreamFile(pfstream, bufferSize, ErrorReporting, ExceptionThrowing)
{
	// Set up Path name
	filePath = fPath;
	ModeOfFile = fileMode;	// note current state

	// Check if the base class setup OK.
	if (!IsInited()) return;
   
	// Open the file 
#if 0 != wxUSE_UNICODE
	{
		size_t			cch = camWcstombs( NULL, (const TCHAR *)filePath.GetPath(), 0 ) + 1;
		PSTR			pszFileName = PSTR( alloca( cch ) );
		camWcstombs( pszFileName, (const TCHAR *)filePath.GetPath(), cch );
		GetFileStream().open( pszFileName, ios_base::openmode(fileMode) );
	}
#else
	GetFileStream().open( filePath.GetPath(), ios_base::openmode(fileMode) );
#endif

	// If file not opened properly report error
	if (IOFile->fail() || !GetFileStream().is_open())
	{
		String_256 ErrorMsg(_R(IDE_OPEN_ERROR));
		ErrorMsg += filePath.GetFileName();	
		GotError(_R(IDE_OPEN_ERROR), ErrorMsg);
		StreamFileInitialised = FALSE;
	}
}




/********************************************************************************************

>	CCDiskFile(PathName fPath, INT32 fileMode, 
			   INT32 fileAccess  = filebuf::openprot,
			   UINT32 bufferSize = CCFILE_DEFAULTSIZE
			   BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE);

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>		(Altered by Neville 01/09/94)
	Created:	1/9/93
	Inputs:		fPath		- PathName for file to be opened
				fileMode	- Mode in which file is to be opened - use ios flags  
				fileAccess	- file access flag(s) as specified by the filebuf flags
				bufferSize	- Size of the fixed size buffer (default = 1024)
							- if size > 0 then create a buffer of the specified size
							- if size = 0 then create a file without a buffer
				ReportErrors flag
				ThrowExceptions flag
	Purpose:	Constructs a CCDiskFile object and opens the file specified by the Path over it.
	SeeAlso:	fstream::setbuf()
	SeeAlso:	fstream::open()					 
	
********************************************************************************************/

CCDiskFile::CCDiskFile(PathName fPath, 
					   INT32 fileMode, 
					   INT32,
					   UINT32 bufferSize,
					   BOOL ErrorReporting,
					   BOOL ExceptionThrowing)
  :	CCStreamFile(new fstream, bufferSize, ErrorReporting, ExceptionThrowing)
{
	// Set up Path name
//	TRACEUSER( "JustinF", _T("In CCDiskFile::CCDiskFile\n"));
	filePath = fPath;
	ModeOfFile = fileMode;	// note current state

	// Check if the base class setup OK.
	if (!IsInited()) return;
   
	// Open the file 
#if 0 != wxUSE_UNICODE
	{
		size_t			cch = camWcstombs( NULL, (const TCHAR *)filePath.GetPath(), 0 ) + 1;
		PSTR			pszFileName = PSTR( alloca( cch ) );
		camWcstombs( pszFileName, (const TCHAR *)filePath.GetPath(), cch );
		GetFileStream().open( pszFileName, ios_base::openmode(fileMode) );
	}
#else
	GetFileStream().open( filePath.GetPath(), ios_base::openmode(fileMode) );
#endif

	// If file not opened properly report error
	if (IOFile->fail() || !GetFileStream().is_open())
	{
		GotError(_R(IDE_OPEN_ERROR));
		StreamFileInitialised = FALSE;
	}
}



/********************************************************************************************
>	BOOL CCDiskFile::open(PathName fPath, INT32 fileMode, INT32 fileAccess = filebuf::openprot)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		fPath		- PathName for file to be opened
				fileMode	- Mode in which file is to be opened - use ios flags  
				fileAccess	- file access flag(s) as specified by the filebuf flags
	Returns:	TRUE if file is opened ok or FALSE otherwize
	Purpose:	opens file specified by the path.
	SeeAlso:	fstream::open()

	Note:		It would appear that calling this with ios::in | ios::out | ios::binary on
				NT on an existing file truncates the file, whereas on Windows 95 it does
				not truncate the file. Exporting bitmap and eps files was failing as the
				data was appended to the end of the file on Windows 95.
				OpMenuExport::DoWithParam fixed by adding a ios::trunc to the open call.

********************************************************************************************/

BOOL CCDiskFile::open( PathName fPath, INT32 fileMode, INT32 )
{
	// First check that the constructor went ok
	if (!StreamFileInitialised)
	{
		// Did not construct ok, error should have been reported, so just fail
     	return FALSE;
	}

	// Nuke any existing open error states that might exist
	IOFile->clear(IOFile->goodbit);

	// If we are not opening this file for writing then set the ios::nocreate
	// flag due to a bug in the iostream classes (but not if the ios::out flag is specified
	// as well. If this is not set we can not open files on read-only mounts shared by windows 95.
//	if ((fileMode & ios::in) && !(fileMode & ios::out)) fileMode |= ios::nocreate;

	// Set up file pathname
	filePath = fPath;
	ModeOfFile = fileMode;	// note current state

	// Open file
#if 0 != wxUSE_UNICODE
	{
		size_t			cch = camWcstombs( NULL, (const TCHAR *)filePath.GetPath(), 0 ) + 1;
		PSTR			pszFileName = PSTR( alloca( cch ) );
		camWcstombs( pszFileName, (const TCHAR *)filePath.GetPath(), cch );
		GetFileStream().open( pszFileName, ios_base::openmode(fileMode) );
	}
#else
	GetFileStream().open( filePath.GetPath(), ios_base::openmode(fileMode) );
#endif
	
	// If error on opening file report it!
	if (IOFile->fail() || !GetFileStream().is_open())
	{
		GotError(_R(IDE_OPEN_ERROR));
       	return FALSE;
	}

	return TRUE;
}



/********************************************************************************************

>	void CCDiskFile::close()


	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Purpose:	To close the file
	SeeAlso:	fstream::close()

********************************************************************************************/

void CCDiskFile::close()
{
	GetFileStream().close();
}



/********************************************************************************************

>	BOOL CCDiskFile::setMode(INT32 fileMode)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/93
	Inputs:		fileMode	- Text or Binary (default is text)  
	Returns:	TRUE if the mode has changed and FALSE otherwize.
	Purpose:	Changes the file's mode from text\binary.
	SeeAlso:	fstream::setmode()

********************************************************************************************/

BOOL CCDiskFile::setMode(INT32 fileMode)
{
	// This might cause problems in compressed mode so error if called in this mode
	ERROR3IF(CompressData, "DiskFile set mode called when compressed");

PORTNOTE("other","ios::setmode not suppport anymore")
#ifndef EXCLUDE_FROM_XARALX
	return GetFileStream().setmode(fileMode) >= 0;
#else
	return true;
#endif
}


	
/********************************************************************************************

>	BOOL CCDiskFile::isOpen() const

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/93
	Returns:	TRUE if file is open and FALSE otherwize.
	Purpose:	Determines whether file is open
	SeeAlso:	fstream::is_open()

********************************************************************************************/

BOOL CCDiskFile::isOpen() const
{
	return CCStreamFile::isOpen() && const_cast<std::fstream &>(GetFileStream()).is_open();
}



/********************************************************************************************

>	BOOL CCDiskFile::GetName( StringBase* name) const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/09/94
	Inputs:		Pointer to a CCFile name 
	Outputs:	None
	Returns:	True if a name has been set otherwise False. At present always returns False.
	Purpose:	Given a CCDiskFile*, you can ask for some sort of name which is associated with
				that file. For filenames it might be a filename, or a pathname, for resource
				files it might be "Default Bitmap" etc. These names could then be used for
				making error messages have some extra useful information in.
	Errors:		None	

********************************************************************************************/

BOOL CCDiskFile::GetName( StringBase* name) const
{
	// Work out how much space we have
	INT32 MaxLen = name->MaxLength();

	filePath.GetPath();

	// Now fit our name into this space
	String_256 Pathname = filePath.GetPath();
	if (MaxLen < Pathname.Length())
	{
		// Use just the filename
		Pathname = filePath.GetFileName();

		// If it is still too long, turn it into the form "...ffff.ext", i.e. have an
		// ellipsis followed by as much of the filename as we can fit into the string.
		// (We use the last segment of the filename, i.e. we throw away characters from
		// the beginning).
		INT32 FilenameLen = Pathname.Length();
		if (MaxLen < FilenameLen)
		{
			// Shuffle pathname down to fit into buffer
			Pathname.Remove(0, FilenameLen - MaxLen);

			// Put the ellipsis at the front
			TCHAR *Buf = (TCHAR *) Pathname;
			Buf[0] = '.';
			Buf[1] = '.';
			Buf[2] = '.';
		}
	}

	// Copy into output parameter
	*name = Pathname;

	// Successful
	return TRUE;
}



/********************************************************************************************

>	virtual PathName CCDiskFile::GetPathName() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Returns:	The pathname we have associated with the CCDiskFile
	Purpose:	To find out the name of the file

********************************************************************************************/

PathName CCDiskFile::GetPathName() const
{
	return filePath;
}



/********************************************************************************************

>	filedesc CCDiskFile::GetFileHandle( ) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/09/94
	Inputs:		None 
	Outputs:	None
	Returns:	The file handle (descriptor) for this file (really an INT32).
	Purpose:	Given an instance of a CCDiskFile, you can ask for the actual file handle
				that describes the associated file. Required for the external filters. 
	Errors:		None	

********************************************************************************************/
// defined in winoil\filehndl.cpp

//filedesc CCDiskFile::GetFileHandle() const 
//{ 
//	// Need to convert this into a real file handle rather than the descriptor.
//	// At present there seems to be no legal way of doing this.
//	return _get_osfhandle(IOFile.fd());
//}



//-----------------------
/********************************************************************************************

										CCMemFile

********************************************************************************************/
//-----------------------


/********************************************************************************************

>	CCMemFile::CCMemFile(BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Inputs:		ReportErrors flag
				ThrowExceptions flag
	Created:	1/9/94
	Purpose:	Constructs an instance of CCMemFile.

********************************************************************************************/

CCMemFile::CCMemFile(BOOL ErrorReporting, BOOL ExceptionThrowing)
{
	// Set our class variables from the passed in values
	ReportErrors = ErrorReporting;
	ThrowExceptions = ExceptionThrowing;
	WasError = FALSE;
	MemFileInitialised = TRUE;

	// Better set these up too or an unused object crashes on destuction
	MemHandle = BAD_MHANDLE;
	MemFile = NULL;
	FileSize = 0;
	CurrentPos = 0;
	IsOpen = FALSE;
	FileProt = CCMemRead;
}


/********************************************************************************************

>	CCMemFile::CCMemFile(void *pFile, UINT32 size, FileAccess fProt,
						 BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Inputs:		pFile - a pointer to the file
				UINT32 - the size of the file
				FileAccess - read or write access 
				ReportErrors flag
				ThrowExceptions flag
	Created:	6/9/93
	Purpose:	Constructs an instance of CCMemFile and Opens it

********************************************************************************************/

CCMemFile::CCMemFile(void *pFile, UINT32 size, FileAccess fProt,
					 BOOL ErrorReporting, BOOL ExceptionThrowing)
{
	// First, set up our initialised flag to False
	MemFileInitialised = FALSE;
	WasError = FALSE;

	// Set our class variables from the passed in values
	ReportErrors = ErrorReporting;
	ThrowExceptions = ExceptionThrowing;

	FileProt = fProt;

	IsOpen = FALSE;

	// If file is opened for reading
	if (FileProt == CCMemRead) 
	{
		// Is file pointer NULL?
		if (pFile == NULL)
		{
			// Yes; report error.
			IsOpen = FALSE;
			GotError(_R(IDE_NULL_FILE));
  	     	return;
		}		      
		
		MemFile		= (BYTE*) pFile;			// Set Memory file pointer
		CurrentPos  = 0;						// Set Current file position		
		
		// Check that file size is greater than zero
		if (size > 0)
		{
			FileSize	= size;
			// Flag that everything has gone ok
			MemFileInitialised = TRUE;
		}
		else
		{
			IsOpen	= FALSE;
			GotError(_R(IDE_FILE_SIZE_ZERO));
  	     	return;
		}
 
 		IsOpen = TRUE;
    }

    // if file is open for writing	
	else if (FileProt == CCMemWrite)
	{                             
		// check file size 
		if (size > 0)
			FileSize = size;					// if not zero use size passed in
		else
			FileSize = CCFILE_DEFAULTSIZE;				// else used default size
			
		//Allocate Memory 
		MemHandle = ClaimBlock(FileSize);
		
		// Check allocation worked ok
		if (MemHandle != BAD_MHANDLE)
		{
			IsOpen 		= TRUE;
			CurrentPos 	= 0;
			// Flag that everything has gone ok
			MemFileInitialised = TRUE;
		}
		else
		{
			IsOpen	= FALSE;
			GotError(_R(IDE_MEM_BLOCK_FAILURE));
     		return;
		} 
	}
}

/********************************************************************************************

>	CCMemFile::~CCMemFile()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93        
	Purpose:	Default destructor.

********************************************************************************************/

CCMemFile::~CCMemFile()
{                   
	if(isOpen())
	{
		// report error that file is not closed before destructor call
		GotError(_R(IDE_MEM_CLOSE_ERROR));

        // Release the memory occupied by the file (only if not reading)
		if (FileProt != CCMemRead) 
			ReleaseBlock(MemHandle);

       	return;
	}		
}

/********************************************************************************************

>	BOOL CCMemFile::IsMemFileInited()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/94
	Inputs:		None
	Returns:	TRUE if the contructor went without errors or FALSE otherwise.
	Purpose:	Allows errors to be returned from the constructor.
	SeeAlso:	CCMemFile

********************************************************************************************/

BOOL CCMemFile::IsMemFileInited()
{
	return MemFileInitialised;
}


/********************************************************************************************

>	BOOL CCMemFile::open(void *pFile, UINT32 size, FileAccess fProt)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Inputs:		pFile - a pointer to the file
				UINT32 - the size of the file
				fProt - defines the files protection - default = read only
	Returns: 	TRUE if file is opened successfully or FALSE otherwize
	Purpose:	Opens an instance of a CCmemFile

********************************************************************************************/

BOOL CCMemFile::open(void *pFile, UINT32 size, FileAccess fProt)
{
	// First check that the constructor went ok
	if (!MemFileInitialised)
	{
		// Did not construct ok, error should have been reported, so just fail
			IsOpen	= FALSE;
  	     	return FALSE;
	}

	FileProt	= fProt;

	// If file is opened for reading
	if (FileProt == CCMemRead) 
	{
		// Is file pointer NULL
		if (!pFile)
		{
			IsOpen	= FALSE;
			GotError(_R(IDE_NULL_FILE));
  	     	return FALSE;
		}		      
		
		MemFile		= (BYTE*) pFile;			// Set Memory file pointer
		CurrentPos  = 0;						// Set Current file position		
		
		// Check that file size is greater than zero
		if (size > 0)
			FileSize	= size;
		else
		{
			IsOpen	= FALSE;
			GotError(_R(IDE_FILE_SIZE_ZERO));
  	     	return FALSE;
		}
    }
    // if file is open for writing	
	else if (FileProt == CCMemWrite)
	{                             
		// check file size 
		if (size > 0)
			FileSize = size;					// if not zero use size passed in
		else
			FileSize = CCFILE_DEFAULTSIZE;				// else used default size
			
		// Allocate Memory 
		MemHandle = ClaimBlock(FileSize);
		
		// Check allocation worked ok
		if (MemHandle != BAD_MHANDLE)
		{
			IsOpen 		= TRUE;
			CurrentPos 	= 0;
		}
		else
		{
			IsOpen	= FALSE;
			GotError(_R(IDE_MEM_BLOCK_FAILURE));
     		return FALSE;
		} 
	}
		
	return IsOpen;
}



/********************************************************************************************

>	BOOL CCMemFile::GrowMemFile()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Return:		TRUE if size can be increased otherwize FALSE 
	Purpose:	
		Will increase the size of the memory file by allocating more space in blocks 1k.
		This function is called by all the write functions when the file size limit is 
		reached. 
	Scope:		Private 

********************************************************************************************/

BOOL CCMemFile::GrowMemFile()
{
	if (IncreaseBlock(MemHandle, CCFILE_DEFAULTSIZE))	//SplitBlock(MemHandle, CCFILE_DEFAULTSIZE, (FileSize  - 1)))
	{
		FileSize += CCFILE_DEFAULTSIZE;
		return TRUE;
	}
	else
	{	            
		IsOpen	= FALSE;
		GotError(_R(IDE_MEM_BLOCK_FAILURE));
		return FALSE;
	}
}

/********************************************************************************************

>	BOOL CCMemFile::setMode(INT32 fileMode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Purpose:	None- cannot change the mode on a memory file - either it's binary in
				which case it's a CCMemFile, or it's text, in which case it's a 
				CCMemTextFile.
	SeeAlso:	CCFile::setMode

********************************************************************************************/

BOOL CCMemFile::setMode(INT32 fileMode)
{
	ENSURE(FALSE, "Should use CCMemFile or CCMemTextFile to set the file mode!");
	return FALSE;
}


/********************************************************************************************

>	BOOL CCMemFile::isOpen() const

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Return:		TRUE if file is open and FALSE otherwize
	Purpose:	Determines whether an instance of CCMemFile is open

********************************************************************************************/

BOOL CCMemFile::isOpen() const
{
	return IsOpen;
}

/********************************************************************************************

>	CCFile& CCMemFile::read(void* buf, UINT32 length = 1)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Outputs:	A pointer to a single byte
	Returns:	Reference to the CCFile object.
	Purpose:	Reads a single byte from the Memory file 

********************************************************************************************/

CCFile& CCMemFile::read(void* buf, UINT32 length)
{
	// Make sure the file is open before it is read!
	if (!isOpen())
	{
		GotError(_R(IDE_NOTOPEN_ERROR));
		return (CCFile&) *this;
	}

    // If not allowed to read, then exit.
    if (FileProt != CCMemRead)
    {
    	GotError(_R(IDE_WRITE_ONLY));
		return (CCFile&) *this;
	}
    
	BYTE *TempBuf = (BYTE*) buf;
	
	while ((length > 0) && !eof())
	{
		*TempBuf++ = MemFile[CurrentPos++];
		length--;
	}

	if (length > 0)
	{
		// Could not read the required number of bytes.
		GotError(_R(IDE_EOF_ERROR));
	}

	return (CCFile&) *this;
}

CCFile& CCMemFile::read(StringBase *buf)
{
	ENSURE(FALSE, "Use a CCMemTextFile for ASCII memory files!");
	return (CCFile&) *this;
}

CCFile &CCMemFile::read( char &buf )
{
	// Make sure the file is open before it is read!
	if (!isOpen())
	{
		GotError(_R(IDE_NOTOPEN_ERROR));
		return *this;
	}

    // if file is write protected then exit
    if (FileProt != CCMemRead)
    {
    	GotError(_R(IDE_WRITE_ONLY));
		return *this;
	}
    
	TCHAR* tempMemFile 	= (TCHAR*) MemFile;			// Cast MemFile to a TCHAR pointer

	if (!eof())
	{
		buf = tempMemFile[CurrentPos++];
		CharsRead ++;
	}
	else
	{
		// Trying to read pasty EOF
		GotError(_R(IDE_EOF_ERROR));
	}

	return *this;
}


/********************************************************************************************

>	CCFile& CCMemFile::write(const void* buf, UINT32 length = 1)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Inputs:		A pointer to a single byte
				Length denotes number of bytes to written 
	Returns:	Reference to the CCFile object.
	Purpose:	Writes a stream of bytes to the Memory file 

********************************************************************************************/

CCFile& CCMemFile::write(const void* buf, UINT32 length)
{                                      
	// Make sure the file is open before it is written!
	if (!isOpen())
	{
		GotError(_R(IDE_NOTOPEN_ERROR));
		return (CCFile&) *this;
	}

    // if file is read only then exit
    if (FileProt != CCMemWrite)
    {
    	GotError(_R(IDE_READ_ONLY));
		return (CCFile&) *this;
	}
    
	BYTE* tempBuf = (BYTE*) buf;
    
	// Get Pointer to the memory file
	if (!DescribeBlock(MemHandle, &MemFile, &FileSize))
	{
		GotError(_R(IDE_MEM_BLOCK_FAILURE));
  		return (CCFile&) *this;
	} 

	while (length > 0)
	{
		if (eof())
		{
			if (GrowMemFile())
			{
				// Get Pointer to the memory file
				if (!DescribeBlock(MemHandle, &MemFile, &FileSize))
				{
					GotError(_R(IDE_MEM_BLOCK_FAILURE));
 		 			return (CCFile&) *this;
				}
			}
			else
				// Can't increase memfile - error
				break;
		}

		// Write input byte
		MemFile[CurrentPos++] = *tempBuf++;
		length--;
	}
	
	return (CCFile&) *this;
}

CCFile& CCMemFile::write(const StringBase& buf, UINT32 length)
{
	ENSURE(FALSE, "Use a CCMemTextFile for ASCII memory files!");
	return (CCFile&) *this;
}

CCFile &CCMemFile::write( char &buf )
{
	ENSURE(FALSE, "Use a CCMemTextFile for ASCII memory files!");
	return (CCFile&) *this;
}

/********************************************************************************************

>	CCFile& CCMemFile::seek(FilePos pos)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Returns:	Reference to the CCFile object.
	Purpose:
		Sets the file pointer to the file position passed in. (Position zero is the 
		beginning of the file)

********************************************************************************************/

CCFile& CCMemFile::seek(FilePos pos)
{
	if(!isOpen())
	{
		GotError(_R(IDE_NOTOPEN_ERROR));
		return (CCFile&) *this;
	}
    
	if (pos <= (FilePos)(FileSize))
	{
		CurrentPos = pos;

		// There were 1001 problems with memfile seeking / telling, the below sorts them out though
		if(IsLexerInitialised() && SeekingRequired)
		{
			Ch = 0;
			Ofs = 0;
			LastLinePos = pos;
			TokenBuf[0] = 0;
			Line = 0;
			CharsRead = 0;
			LastLinePos = pos;
			EOFFound = FALSE;

			// Fill up the buffers with the new chars			
			GetLine();
			//GetCh();
		}
	}
	else
	{
		GotError(_R(IDE_FILEPOS_ERROR));
		return (CCFile&) *this;
	}

	return (CCFile&) *this;
}

/********************************************************************************************

>	FilePos CCMemFile::tell()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Return:		current file pointer position
	Purpose:	Gets the file pointer  position of the file. 

********************************************************************************************/

FilePos CCMemFile::tell()
{
	return CurrentPos;
}

/********************************************************************************************

>	CCFile&	CCMemFile::seekIn(FilePos Pos)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Pos - the desired file position.
	Returns:	Reference to the CCFile object.
	Purpose:	Alter position of the file input pointer.  NB. input and output
				pointers are linked with CCMemFiles, so you will alter the output
				pointer too.

********************************************************************************************/

CCFile&	CCMemFile::seekIn(FilePos Pos)
{
	return seek(Pos);
}



/********************************************************************************************

>	CCFile& CCMemFile::seekIn(INT32 Offset, ios::seek_dir Dir)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/05/95
	Inputs:		Offset - the file offset required.
				Dir - the offset mode to use.
	Outputs:	-
	Returns:	Reference to the CCFile object.
	Purpose:	Alter position of the file input pointer.  NB. input and output
				pointers are linked with CCMemFiles, so you will alter the output
				pointer too.

********************************************************************************************/

CCFile& CCMemFile::seekIn(INT32 Offset, ios::seekdir Dir)
{
	switch(Dir)
	{
		case ios::beg:
			return(seek(Offset));

		case ios::cur:
			return(seek(tell() + Offset));

		case ios::end:
			{
				FilePos NewPos = (FilePos)(FileSize - Offset);
				if (NewPos < 0)
				{
					ERROR3("Attempt to seek before beginning of file using seekIn with ios::end");
					NewPos = 0;
				}
				return(seek(NewPos));
			}
		default:
			break; // fall through
	}

	ERROR3("Illegal seekIn seek_dir value (CCMemFile::seekIn)");
	return (CCFile&) *this;
}



/********************************************************************************************

>	FilePos CCMemFile::tellIn()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Returns:	File position.
	Purpose:	Return the position of the file input pointer.

********************************************************************************************/

FilePos CCMemFile::tellIn()
{
	if(IsLexerInitialised())
	{
		if(!SeekingRequired)
			ERROR3("CCMemFile::tellIn() will only work properly with seeking enabled");

		// -1 for the character lookahead in operation
		return (FilePos)(Ofs + (INT32) LastLinePos - 1);
	}

	return tell();
}


/********************************************************************************************

>	virtual INT32 CCMemFile::Size()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Returns:	Size in bytes of the file.
	Purpose:	Find the size of the file.

********************************************************************************************/

size_t CCMemFile::Size()
{
	return FileSize;
}


/********************************************************************************************

>	BOOL CCMemFile::eof()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> (fixed by Andy)
	Created:	6/9/93
	Return:		TRUE if end of file and FALSE otherwize
	Purpose:	Determines whether an instance of CCMemFile is open

********************************************************************************************/

BOOL CCMemFile::eof() const
{
	return (CurrentPos >= FileSize);
}

BOOL CCMemFile::good() const
{
	return !WasError;
}

BOOL CCMemFile::bad() const
{
	return WasError;
}

BOOL CCMemFile::fail() const
{
	return WasError;
}

/********************************************************************************************

>	BOOL CCMemFile::SetBadState()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/08/94
	Returns:	None.
	Purpose:	It allows the forceable setting of the test that functions like bad() use
				so the next check of the current file status to see if we should continue
				using the file or not will fail. This will be called by the GotError function
				so that any attempts to read/write after this should fail.
	SeeAlso:	CCFile::GotError

********************************************************************************************/

void CCMemFile::SetBadState()
{
	WasError = TRUE;
}

/********************************************************************************************

>	BOOL CCMemFile::SetGoodState()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/94
	Returns:	None.
	Purpose:	It allows the forceable setting of the test that functions like bad() use
				so the next check of the current file status to see if we should continue
				using the file or not will pass.
	SeeAlso:	CCFile::GotError

********************************************************************************************/

void CCMemFile::SetGoodState()
{
	// Nuke any existing error states that might exist e.g. a lying eof() bit
	WasError = FALSE;
}

/********************************************************************************************

>	BOOL CCMemFile::GetName( StringBase* name) const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/09/94
	Inputs:		Pointer to a CCMemFile name 
	Outputs:	None
	Returns:	True if a name has been set otherwise False. At present always returns False.
	Purpose:	Given a CCMemFile*, you can ask for some sort of name which is associated with
				that file. For filenames it might be a filename, or a pathname, for resource
				files it might be "Default Bitmap" etc. These names could then be used for
				making error messages have some extra useful information in.
	Errors:		None	

********************************************************************************************/
BOOL CCMemFile::GetName( StringBase* name) const
{
	return FALSE;
}


/********************************************************************************************

>	filedesc CCMemFile::GetFileHandle( ) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/09/94
	Inputs:		None 
	Outputs:	None
	Returns:	The file handle (descriptor) for this file (really an INT32).
	Purpose:	Given an instance of a CCMemFile, you can ask for the actual file handle
				that describes the associated file. Required for the external filters.
	Errors:		Not appropriate for memory files hence error at present
	SeeAlso:	CCDiskFile::GetFileHandle(); CCFile::GetFileHandle()	

********************************************************************************************/
filedesc CCMemFile::GetFileHandle() const 
{
	ERROR3("CCMemFile::GetFileHandle() called");
	return -1;
}

/********************************************************************************************

>	void CCMemFile::close()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Purpose:	Closes an instance of a CCMemFile

********************************************************************************************/

void CCMemFile::close()
{
	IsOpen = FALSE;

    // Release the memory occupied by the file
	if (FileProt != CCMemRead) 
		ReleaseBlock(MemHandle);
}


/********************************************************************************************

>	virtual BOOL CCMemFile::InitCompression(BOOL Header = FALSE)


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/95
	Purpose:	To initialise the compression system ready for use. This is to make sure
				there is enough memory available for its buffers before writing out any
				compression tokens into the output file.
				MemFiles not supported at present so always returns FALSE.
				Graeme (15/11/99): Added the Header parameter.
	SeeAlso:	StartCompression; StopCompression

********************************************************************************************/

BOOL CCMemFile::InitCompression(BOOL Header)
{
	ERROR3("CCMemFile::InitCompression Cannot compress memory files yet");
	return FALSE;	
}

/********************************************************************************************

>	virtual BOOL CCMemFile::StartCompression()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Purpose:	To start up the compression system.
				MemFiles not supported at present so always returns FALSE.
	SeeAlso:	StopCompression

********************************************************************************************/

BOOL CCMemFile::StartCompression()
{
	ERROR3("CCMemFile::StartCompression Cannot compress memory files yet");
	return FALSE;	
}	

/********************************************************************************************

>	virtual BOOL CCMemFile::StopCompression()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Purpose:	To stop the compression system.
				MemFiles not supported at present so always returns FALSE.
	SeeAlso:	StartCompression

********************************************************************************************/

BOOL CCMemFile::StopCompression()
{
	ERROR3("CCMemFile::StartCompression Cannot compress memory files yet");
	return FALSE;
}

/********************************************************************************************
										CCMemTextFile
********************************************************************************************/




/********************************************************************************************

>	CCMemFile::CCMemFile(TCHAR *pFile, UINT32 size, FileAccess fProt)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Inputs:		pFile - a pointer to the file
				UINT32 - the size of the file
				fProt - file protection - default = Read Only
	Created:	6/9/93
	Purpose:	Constructs an instance of CCMemTextFile and Opens it

********************************************************************************************/

CCMemTextFile::CCMemTextFile( char *pFile, UINT32 size, FileAccess fProt,
							  BOOL ErrorReporting, BOOL ExceptionThrowing )
 : CCMemFile( pFile, size, fProt, ErrorReporting, ExceptionThrowing )
{
}

/********************************************************************************************

>	CCMemTextFile::~CCMemTextFile()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93        
	Purpose:	Default destructor.

********************************************************************************************/

CCMemTextFile::~CCMemTextFile()
{                   
}

/********************************************************************************************

>	BOOL CCMemTextFile::open(TCHAR *pFile, UINT32 size, FileAccess fProt)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Inputs:		pFile - a pointer to the file
				UINT32 - the size of the file  
				fProt - file protection - default = Read Only
	Returns: 	TRUE if file is opened successfully or FALSE otherwize
	Purpose:	Opens an instance of a CCMemTextFile

********************************************************************************************/

BOOL CCMemTextFile::open( char *pFile, UINT32 size, FileAccess fProt )
{
	return CCMemFile::open(pFile, size, fProt);
}

/********************************************************************************************

>	CCFile& CCMemTextFile::read(TCHAR& buf)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Outputs;	character read in
	Returns:	Reference to the CCFile object.
	Purpose:	Reads a single character from the Memory file 

********************************************************************************************/

CCFile &CCMemTextFile::read( char &buf )
{
	// Make sure the file is open before it is read!
	if (!isOpen())
	{
		GotError(_R(IDE_NOTOPEN_ERROR));
		return *this;
	}

    // if file is write protected then exit
    if (FileProt != CCMemRead)
    {
    	GotError(_R(IDE_WRITE_ONLY));
		return *this;
	}
    
	TCHAR* tempMemFile 	= (TCHAR*) MemFile;			// Cast MemFile to a TCHAR pointer

	if (!eof())
	{
		buf = tempMemFile[CurrentPos++];
		CharsRead ++;
	}
	else
	{
		// Trying to read pasty EOF
		GotError(_R(IDE_EOF_ERROR));
	}

	return *this;
}

/********************************************************************************************

>	CCFile& CCMemTextFile::read(StringBase* buf)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> (Fixed by Richard)
	Created:	6/9/93 (14/4/95)
	Inputs:		- 
	Output:		Buf is the string buffer to return the string in
	Returns:	Reference to the CCFile object.
	Purpose:	Reads a stream of characters from the Memory file .
				We now stop the string if it won't fit in the buffer, or has hit an EOL.

********************************************************************************************/

CCFile& CCMemTextFile::read( StringBase *pBuf )
{
	// Make sure the file is open before it is read!
	if (!isOpen())
	{
		GotError(_R(IDE_NOTOPEN_ERROR));
		return (CCFile&) *this;
	}

    // if file is write protected then exit
    if (FileProt != CCMemRead)
    {
    	GotError(_R(IDE_WRITE_ONLY));
		return (CCFile&) *this;
	}
    
	// Read characters in until we have MaxLength() of them, or we hit a new line
	PSTR				tempMemFile = PSTR(MemFile);	// Cast MemFile to a char pointer
	char				Ch;
	INT32				Max = pBuf->MaxLength();
	INT32				Off = 0;
	
	PTSTR				Text = *pBuf;

	// chars to look for
	const char			CR = '\n';
	const char			LF = '\r';

	// We have to do this sneeky test for end of file because the io stream classes
	// are a load of old bollox!
	if (eof())
	{
		// Its the end of the file all right
		Text[0] = 0;
		return (CCFile&) *this;
	}

	// Get the first char
	read(Ch);

	// Loop around until we get to the end of the line or the end of the file
	// Must leave 1 character space in the buffer if we reach the maximum buffer size
	// so that we can fit the terminating zero in below.
	while ((Max>1) && (Ch!=CR) && (Ch!=LF) && (!eof()))
	{
		// store the char we read
#if FALSE != wxUSE_UNICODE
		mbtowc( Text + Off, &Ch, 1 );
		++Off;
#else
		Text[Off++] = Ch;
#endif
		
		// read another one and decrement the counter
		read(Ch);

		// Keep track of how much we have read
		Max--;
	}
	
	// if we ended at the end of a line, see if the next char is also part of a new line
	Ch = tempMemFile[CurrentPos];
	if ((Ch==LF) || (Ch==CR))
	{
		// eat the other part of the CR LF combo
		read(Ch);
	}

	// Terminate the string
	Text[Off++] = 0;

	// return
	return (CCFile&) *this;

#if 0
	// ******* This is the old Mario code

	// Make sure the file is open before it is read!
	if (!isOpen())
	{
		GotError(_R(IDE_NOTOPEN_ERROR));
		return (CCFile&) *this;
	}

    // if file is write protected then exit
    if (FileProt != CCMemRead)
    {
    	GotError(_R(IDE_WRITE_ONLY));
		return (CCFile&) *this;
	}
    
	TCHAR* tempMemFile 	= (TCHAR*) MemFile;			// Cast MemFile to a TCHAR pointer
	TCHAR* pString 		= *buf;						// Cast input string to a TCHAR pointer
	UINT32 i = 0;
	
	// While String Length > String Max size
	//	and not end of file
	while ((i <= DEF_STRING_SIZE) && (!eof())) 
	{
		*pString = tempMemFile[CurrentPos++];		// output buffer = char read
		pString++;                                  // increment output buffer ptr
		i++;										// increment counter
	}
	
	return (CCFile&) *this;
#endif
}

/********************************************************************************************

>	CCFile& CCMemTextFile::write(const TCHAR& buf)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Inputs:		A pointer to a single byte
	Returns:	Reference to the CCFile object.
	Purpose:	Writes a single byte to the Memory file 

********************************************************************************************/

CCFile &CCMemTextFile::write( const char &buf )
{
	// Make sure the file is open before it is written!
	if (!isOpen())
	{
		GotError(_R(IDE_NOTOPEN_ERROR));
		return (CCFile&) *this;
	}

    // if file is read only then exit
    if (FileProt != CCMemWrite)
    {
    	GotError(_R(IDE_READ_ONLY));
		return (CCFile&) *this;
	}
    
    TCHAR* tempMemFile;
    
	// Get Pointer to the memory file
	if (!DescribeBlock(MemHandle, &MemFile, &FileSize))
	{
		GotError(_R(IDE_MEM_BLOCK_FAILURE));
  		return (CCFile&) *this;
	} 
    
	// Cast pointer to memory file to TCHAR
    tempMemFile = (TCHAR*) MemFile;
    
	// if the file size limit is reached then increase memory file by default amount
	if (CurrentPos != (FileSize - 1)) 
		tempMemFile[CurrentPos++] = buf;
	else
	{
		if (GrowMemFile())
		{
			// Get Pointer to the memory file
			if (!DescribeBlock(MemHandle, &MemFile, &FileSize))
			{
				GotError(_R(IDE_MEM_BLOCK_FAILURE));
 		 		return (CCFile&) *this;
			} 
    		// Cast pointer to memory file to TCHAR
    		tempMemFile = (TCHAR*) MemFile;
			// write input byte
			tempMemFile[CurrentPos++] = buf;
		}
	}
				
	return (CCFile&) *this;
}

/********************************************************************************************

>	CCFile& CCMemTextFile::write(const StringBase& buf, UINT32 length = 0)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Inputs:		constant unicode string
				Length denotes number of bytes to written 
	Returns:	Reference to the CCFile object.
	Purpose:	Writes a stream of bytes to the Memory file 

********************************************************************************************/

CCFile& CCMemTextFile::write(const StringBase& buf, UINT32 length)
{                                      
	// Make sure the file is open before it is written!
	if (!isOpen())
	{
		GotError(_R(IDE_NOTOPEN_ERROR));
		return (CCFile&) *this;
	}

    // if file is read only then exit
    if (FileProt != CCMemWrite)
    {
    	GotError(_R(IDE_READ_ONLY));
		return (CCFile&) *this;
	}
    
	// Ensure that the length of the string to be written is not longer
	// than the string's size.
	if (length == 0)
		length = buf.Length();
	else if ((INT32) length > buf.Length())
	{
		GotError(_R(IDE_STRING_SIZE_ERROR));
		return (CCFile&) *this;
	}

	const TCHAR* tempBuf = (const TCHAR *) buf;
    TCHAR* tempMemFile;
    
	// Get Pointer to the memory file
	if (!DescribeBlock(MemHandle, &MemFile, &FileSize))
	{
		GotError(_R(IDE_MEM_BLOCK_FAILURE));
  		return (CCFile&) *this;
	} 
    
	// Cast pointer to memory file to TCHAR
    tempMemFile = (TCHAR*) MemFile;

	for (UINT32 i = 0; (i <= length); i++)
	{
		// if the file size limit is reached then increase memory file by default amount
		if (CurrentPos != (FileSize - 1))
			tempMemFile[CurrentPos++] = *tempBuf++;
		else
		if (GrowMemFile())
		{
			// Get Pointer to the memory file
			if (!DescribeBlock(MemHandle, &MemFile, &FileSize))
			{
				GotError(_R(IDE_MEM_BLOCK_FAILURE));
 		 		return (CCFile&) *this;
			} 
			// Cast pointer to memory file to TCHAR
    		tempMemFile = (TCHAR*) MemFile;

			// write input byte
			tempMemFile[CurrentPos++] = *tempBuf++;
		}
	}
	
	return (CCFile&) *this;
}


/********************************************************************************************

>	BOOL CCMemTextFile::eof()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/93
	Return:		TRUE if end of file and FALSE otherwise.
	Purpose:	Determines whether an instance of CCMemTextFile has reached its EOF or not.

********************************************************************************************/

BOOL CCMemTextFile::eof() const
{
	// Cast pointer to memory file to TCHAR
    TCHAR* tempMemFile = (TCHAR*) MemFile;

	if ((CurrentPos == (FileSize - 1)) || (tempMemFile[CurrentPos] == END_OF_FILE))
		return TRUE;
	else
		return FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************
>	INT32 fake_filedesc(iostream& io)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/96
	Inputs:		io		-		a reference to an iostream object (not constant).
	Returns:	An integer 'file-descriptor'.  This can be passed to the stream_* functions
				to identify the stream they should work on.
	Purpose:	Creates a fake 'file-descriptor' that can be passed to C libraries, such
				as the Accusoft filter library, that allow the caller to redirect I/O.
	SeeAlso:	unfake_filedesc; stream_read; stream_write; stream_seek; _read; _write;
				_lseek; fstream::fd
********************************************************************************************/

UINT_PTR fake_filedesc(iostream& io)
{
	// Make sure this dirty trick is possible.
	ERROR3IF(sizeof(INT32) < sizeof(void*),
			 "type 'INT32' too small for type 'istream*' in fake_filedesc");

	// Return a "magic number" file-descriptor which is really a pointer to an iostream.
	return ~UINT32( (UINT_PTR)(&io) );
}



/********************************************************************************************
>	iostream* unfake_filedesc(INT32 nFileDesc)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/96
	Inputs:		nFileDesc		-	the integer fake 'file-descriptor' previously obtained
									by a call to fake_filedesc.
	Returns:	A pointer to the iostream object the 'file-descriptor' refers to.
	Purpose:	Complementary function to fake_filedesc.
	SeeAlso:	fake_filedesc; stream_read; stream_write; stream_seek; _read; _write;
				_lseek; fstream::fd
********************************************************************************************/

iostream* unfake_filedesc( UINT_PTR nFileDesc )
{
	// Make sure this dirty trick is possible.
	ERROR3IF(sizeof(INT32) < sizeof(void*),
			 "type 'INT32' too small for type 'istream*' in unfake_filedesc");

	// Convert this magic number back into a pointer to an iostream.
	iostream* ps = (iostream*) (void*) ~nFileDesc;
	ERROR3IF(ps == 0, "Null 'file-descriptor' in unfake_filedesc");
	return ps;
}



/********************************************************************************************
>	extern "C" INT32 ACCUAPI_IO stream_read(INT32 nFileDesc, char* pchBuffer, unsigned nBufSize)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/96
	Inputs:		nFileDesc		-	the "file-descriptor" of the "file" to read from.  This
									is really an iostream* cast to an INT32.
				pchBuffer		-	where to put the data that is read.
				nBufSize		-	the size of the input buffer pointed to by pchBuffer
	Returns:	The number of bytes read by the call, or zero if at the end of the file, or
				-1 if there is an error.
	Purpose:	Adapts functions that call C-style I/O routines so that they can use
				C++ streams instead.  This function substitutes for the standard C
				read() function.  Instead of nFileDesc being a file-descriptor, it
				is expected to be an INT32 returned by fake_filedesc.  Using this function we
				can persuade old code, such as the Accusoft library, to read and write
				from structured storage.
	Errors:		ERROR3 if the given buffer is larger than that allowed by istream.
	SeeAlso:	stream_write; stream_seek; fake_filedesc; unfake_filedesc; iostream;
				istream::read; ios::eof; ios::bad; costream; fstream; _read
********************************************************************************************/

INT32 ACCUAPI_IO stream_read(INT32 nFileDesc, char* pchBuffer, unsigned nBufSize)
{
	// Extract the input stream object associated with this request.
	iostream* ps = unfake_filedesc(nFileDesc);
	if (ps == 0 || ps->bad()) return -1;

	// Make sure the buffer isn't too big for a signed integer.
	if (nBufSize > INT_MAX)
	{
		ERROR3("Buffer too large for type 'signed INT32' in stream_read");
		nBufSize = INT_MAX;
	}

	// If we're already at the end of the file then indicate this.
	if (ps->eof()) return 0;

	// Try to read the data into the given buffer.
	if (!ps->read(pchBuffer, (INT32) nBufSize) && !ps->eof()) return -1;

	// Return the number of bytes actually read.
	return ps->gcount();
}



/********************************************************************************************
>	extern "C" INT32 ACCUAPI_IO stream_write(INT32 nFileDesc, char* pchBuffer, unsigned nBufSize)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/96
	Inputs:		nFileDesc		-	the "file-descriptor" of the "file" to write to.  This
									is really an iostream* cast to an INT32.
				pchBuffer		-	where to get the data that is to be written.
				nBufSize		-	the size of the output buffer pointed to by pchBuffer
	Returns:	The number of bytes written by the call, or -1 if there is an error.
	Purpose:	Adapts functions that call C-style I/O routines so that they can use
				C++ streams instead.  This function substitutes for the standard C
				write() function.  Instead of nFileDesc being a file-descriptor, it
				is expected to be an INT32 returned by fake_filedesc.  Using this function we
				can persuade old code, such as the Accusoft library, to read and write
				from structured storage.
	Errors:		ERROR3 if the given buffer is larger than that allowed by ostream.
	SeeAlso:	stream_read; stream_seek; fake_filedesc; iostream; ostream::write; costream;
				fstream; _write
********************************************************************************************/

INT32 ACCUAPI_IO stream_write(INT32 nFileDesc, char* pchBuffer, unsigned nBufSize)
{
	// Extract the output stream object associated with this request.
	iostream* ps = unfake_filedesc(nFileDesc);
	if (ps == 0 || ps->bad()) return -1;

	// Make sure the buffer isn't too big for a signed integer.
	if (nBufSize > INT_MAX)
	{
		ERROR3("Buffer too large for type 'signed INT32' in stream_write");
		nBufSize = INT_MAX;
	}

	// Try to write the data from the given buffer.
	if (!ps->write(pchBuffer, (INT32) nBufSize)) return -1;

	// Return the number of bytes actually written, which is always all or none.
	return (INT32) nBufSize;
}



/********************************************************************************************
>	extern "C" INT32 ACCUAPI_IO stream_seek(INT32 nFileDesc, INT32 nOffset, unsigned fSeekType)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/96
	Inputs:		nFileDesc		-	the "file-descriptor" of the "file" to write to.  This
									is really an iostream* cast to an INT32.
				nOffset			-	the amount to seek by or the position to seek to.
				fSeekType		-	the direction of the seek (see the run-time docs for
									the _lseek function to get these flags).
	Returns:	The new seek position, or -1 if there was an error.
	Purpose:	Adapts functions that call C-style I/O routines so that they can use
				C++ streams instead.  This function substitutes for the standard C
				seek() function.  Instead of nFileDesc being a file-descriptor, it
				is expected to be an INT32 returned by fake_filedesc.  Using this function we
				can persuade old code, such as the Accusoft library, to read and write
				from structured storage.  Note that the function assumes that the stream
				has only one combined seek pointer, just like fstream and costream.  If
				moving the input seek pointer doesn't also move the output seek pointer,
				as may be the case for some exotic iostream derivatives, then the function
				won't work properly.
	Errors:		ERROR3 if the seek type is invalid or if the put and get seek positions
				aren't tied, as the semantics of the stream require.
	SeeAlso:	stream_read; stream_write; fake_filedesc; iostream; iostream::seekg;
				fstream; costream; _lseek
********************************************************************************************/

INT32 ACCUAPI_IO stream_seek(INT32 nFileDesc, INT32 nOffset, unsigned fSeekType)
{
	// Extract the stream object associated with this request.  Note that in disk-based
	// streams (including the costream), the get and put seek positions are tied, so
	// moving one moves the other.  Hence we can use a plain istream here.
	iostream* ps = unfake_filedesc(nFileDesc);
	if (ps == 0 || ps->bad()) return -1;

	// Convert the standard C flags for fSeekType into the iostream equivalents.
	// NB. make sure the flags as passed by Accusoft are the same as those used by
	// the Microsoft run-time library!
	ios::seekdir dir;
	switch (fSeekType)
	{
	case SEEK_SET:	dir = ios::beg; break;
	case SEEK_CUR:	dir = ios::cur; break;
	case SEEK_END:	dir = ios::end; break;
	default:		ERROR3("Bad seek direction in stream_seek"); return -1;
	}

	// Try to seek to the new position.
	if( !ps->seekg( (streampos)nOffset, ios_base::seekdir(dir) ) )
		return -1;

	// Check if the iostream has similar semantics to fstream.
	ERROR3IF(ps->tellp() != ps->tellg(),
			 "Put and get seek pointers aren't tied in stream_seek");

	// Return the new seek position.
	return (INT32) ps->tellg();
}
