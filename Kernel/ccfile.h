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

#ifndef INC_CCFILE
#define INC_CCFILE

#include <fstream>

#include "handles.h"
#include "pathname.h"
#include "memory.h"

using namespace std;
typedef INT32				filedesc;

typedef INT32			FilePos;

// Default File Buffer Size

#define CCFILE_DEFAULTSIZE (1024)

// Default String Sizes
                         
#define DEF_STRING_SIZE	32
#define STRING_64_SIZE	64
#define STRING_128_SIZE	128
#define STRING_256_SIZE	256

// CCMemTextFile::Read terminators 

#define END_OF_LINE '\n'
#define END_OF_FILE '\0'


// Enumeration of all the File Access rights available	
enum FileAccess {CCMemRead, CCMemWrite};	


class GZipFile; 
class GZipStream;
class costream;
struct IStream;


/********************************************************************************************

>	class CCFile : public CCObject

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/1993          
	Base Class:	CCObject
	Purpose:	Represents an abstract file class from which all other file classes will be 
				derived. It therefore acts as a generic interface to file handling of all
				sorts. A routine can read or write to a CCFile object without caring whether
				it is a real file on disk, or to the clipboard, or to a block of RAM
				somewhere.
				This class contains many pure virtual functions, which should be implemented
				by the derived classes.  The basic functions of this class are supplemented
				by the derived class CCLexFile, which provides functions to perform lexical
				analysis on a file object. (Note that CCLexFile is also	an abstract base
				class.) The main use of this class will be for the export and import filters.
				The class and its derivatives in this file used to just call InformError()
				about any errors that were encountered by the routines. The caller had no 
				control over this so the liklyhood was that users would see a number of 
				errors instead of just one. This is still the default but now all error
				handling goes through a GotError function which can be made to call
				InformError() and/or throw an exception. This is controlled by two protected
				BOOL values	and can be either set in the constructor or via the public
				SetReportErrors	and	SetThrowExceptions calls. GotError also tries to force
				the bad(), fail() functions to fail on the next call so that no more file
				accesses should happen. 
	SeeAlso:	CCLexFile; CCDiskFile; CCMemFile; CCMemTextFile

********************************************************************************************/

class CCAPI CCFile : public CCObject
{                                   
	CC_DECLARE_DYNAMIC(CCFile);
public:
	// Constructor with default parameters 
	//CCFile() {}
	CCFile(BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE);
	virtual ~CCFile() { /* empty */ }
	
	// Switch between text and binary modes. (filebuf::text is non-standard and the same as 0)
	virtual BOOL  setMode(INT32 fileMode = 0)				= 0;

	// Check if the file is open or not.
	virtual BOOL  	 isOpen() const						= 0;

	// File pointer access/control.
	virtual CCFile&	 seekIn(FilePos) 					= 0;
	virtual CCFile&	 seekIn(INT32 Offset,
							ios::seekdir Dir) 			= 0;
	virtual FilePos  tellIn() 							= 0;
	virtual CCFile&	 seek(FilePos pos) 					= 0;
	virtual FilePos	 tell() 							= 0;

	// Read functions.
	virtual CCFile&  read(void *buf, UINT32 length = 1)	= 0;
	virtual CCFile&  read(StringBase *buf) 				= 0;
	virtual CCFile&  read(char & buf)					= 0;
	                                
	// Write functions.
	virtual CCFile&  write(const void *buf, 
						   UINT32 length = 1)				= 0;
	virtual CCFile&  write(const StringBase& buf, 
						   UINT32 length = 0) 			= 0;
	virtual CCFile&  write(char& buf)					= 0;

	// Get the size of an open file in bytes.
	virtual size_t   Size() 			  				= 0;

	// Status functions.
	virtual BOOL     eof() const  		  				= 0;

	// Close the file.
    virtual void     close()			  				= 0;

	// Non-pure status functions.
	virtual BOOL     good() const;
	virtual BOOL     bad() const;
	virtual BOOL     fail() const;

	// Pure status setting functions.
	virtual void	SetBadState()						= 0;
	virtual void	SetGoodState()						= 0;

	// Allows access to the underlying fstream, is so required.
	// Mainly required for PNG/ZLib bits
	// All non-disk file classes just return NULL
	virtual iostream* GetIOFile()						= 0;

	// Flush the file object's buffer (if any).
	virtual CCFile&	 flush();

	// Character access.
	virtual CCFile&  get(char&);
	virtual CCFile&  put(const char&);
	
	// Stream operators for Strings.
	virtual CCFile&  operator>>(StringBase *buf);
	virtual CCFile&  operator<<(const StringBase& buf);

	// Error checking operators
    operator void*() const	{ return fail() ? 0 : (void*) this; }
    INT32 operator!() const	{ return fail(); }

	// Public way of setting the two protected variables and reading their state
	virtual BOOL SetReportErrors( BOOL );
	virtual BOOL SetThrowExceptions( BOOL );
	virtual BOOL IsReportingSet();
	virtual BOOL IsThrowingSet();

	// Uses the above flags to determine whether to report errors and/or throw exceptions
	virtual void GotError( UINT32 errorID, const TCHAR *errorString );	
	virtual void GotError( UINT32 errorID);					

	// Find out whether we have a useful name to return which we can append to say
	// error messages to give feedback on what went wrong.
	virtual BOOL GetName(StringBase*) const;
	virtual PathName GetPathName() const;

	// Used to make short but informative error messages.
	virtual BOOL SetFileErrorMessage(UINT32 ErrorID);
	virtual BOOL SetZipFileErrorMessage(UINT32 ErrorID);

	// Get access to the file descriptor or handle for the file
	virtual filedesc GetFileHandle() const				= 0;

	// Allow setting of compression of output data
	virtual BOOL IsCompressionSet( );
	virtual BOOL SetCompression( BOOL );

	virtual BOOL InitCompression(BOOL Header = FALSE)	= 0;
	virtual BOOL StartCompression()						= 0;
	virtual BOOL StopCompression()						= 0;

protected:
	// Determines whether InformError is to be called and/or exception throwing is required  
	BOOL ReportErrors;						// Flag to say is we report errors
	BOOL ThrowExceptions;					// Flag to say whether we throw exceptions

	BOOL CompressData;						// Flag to say compress the output
	UINT32 GZipErrorID;						// current zip file error id
};



/********************************************************************************************

>	class CCLexFile : public CCFile

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Purpose:	Provide a derivation of the CCFile class that provides support for lexical
				analysis of a file. This allows a stream of tokens to be extracted from
				an ASCII text file.  This is an abstract class - you should not try to
				create an object of this class; use one of the derivations such as 
				CCDiskFile or CCMemFile.
				Note that the lexical analysis facilities are present in all concrete
				file classes, but the only overhead is a few tens of bytes on the size
				of the object - no buffers are allocated for lexical analysis unless you
				call the CCLexFile::InitLexer() function (and hence lexical analysis will
				not work *unless* you call this function!).
	SeeAlso:	CCFile; CCDiskFile; CCMemFile; CCMemTextFile

********************************************************************************************/

typedef enum
{
	TOKEN_EOF,
	TOKEN_EOL,
	TOKEN_NORMAL,
	TOKEN_COMMENT,
	TOKEN_STRING,
	TOKEN_LINE
} LexTokenType;


class CCLexFile : public CCFile
{
	CC_DECLARE_DYNAMIC(CCLexFile);
public:
	CCLexFile(BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE);
	~CCLexFile();

	BOOL InitLexer(BOOL IsSeekingRequired = FALSE,BOOL DoIgnoreStringEscapeCodes = FALSE);
	void DeinitLexer();

	// Change behaviour of token parser
	void SetWhitespace(char*);
	void SetDelimiters(char*);
	void SetCommentMarker(char);
	void SetStringDelimiters(char*);

	// Read in the next token.
	BOOL GetToken();

	// Read in hexadecimal data
	BOOL GetHexToken();

	// Read until next token is TOKEN_NORMAL or TOKEN_STRING.
	// Delimiter tokens are skipped. 
	// Return FALSE if error occurs or if TOKEN_EOF encountered
	BOOL GetSimpleToken();

	// Read in a token in a line-based manner.  If the current input position is in the
	// middle of a line, the data up until the end of the line is read. If at the start
	// of a line, the whole line is returned.
	// The token is still examined to work out what it is, but obviously it may not
	// match any proper construct, in which case the type of the token is set to TOKEN_LINE.
	// (NB this is the only time that TOKEN_LINE is used)
	BOOL GetLineToken();

	// Put the current token back onto the input stream - i.e. the next time GetToken()
	// is called it will return the token it returned last time.
	// This is not nestable - you can only put back one token at once.
	void UngetToken();

	// Get status information...
	UINT32 		 GetLineNumber()	{ return Line;      }
	UINT32  		 GecharOffset()	{ return Ofs;       }
	virtual	INT32 GetCharsRead()  	{ return CharsRead; }
	LexTokenType GetTokenType() 	{ return TokenType; }
	const TCHAR *GetTokenBuf() 		{ return TokenBuf;  }
	BOOL 		 IsLexerInitialised() { return LexerInitialised; }

	// Token writing routines.
	// These very simple functions allow your token output code to look
	// far neater. Besides, if we provide a GetToken() routine, it is only
	// common decency to provide a PutToken() routine too...
	BOOL	PutString( const StringBase &str, UINT32 length = 0, char* Sep=" " );
	BOOL	PutToken( const StringBase &str, UINT32 length = 0, char* Sep=" " );
	BOOL	PutToken( const TCHAR *buf, char *Sep=" " );
	BOOL	PutToken( INT32 n, char* Sep=" " );
	BOOL	PutNewLine();
	void	IncIndent();
	void	DecIndent();
	void	SetIndentDelta(UINT32 d) { IndentDelta = d; }

	// The following 3 functions shouldn't really be public...pretend they're not here.
	// It is possible to call these directly, but it's not recommended unless you absolutely
	// have to, because you might stuff up the one character look-ahead.
	// (see epsfiltr.cpp for example of how to do it properly).
	// In fact, if you need to call these, tell me (Tim) and I'll work out what sort of
	// function we need to have here...I'll probably change it soon, anyway.
	void GetLine();
	void GetCh();

	// Helper function in case you ever need to call GetLine() directly and want to see
	// what you've read in.
	const TCHAR *GetLineBuf() { return Buf; }

	// this might look like a bodge but really it's a cunning way to avoid failure in 
	// LoadInitialSegment() which sadly always requests 1K in the asynch csae 
	// ( cause we dont know the filesize till the downloads over
	virtual void SetDontFail(BOOL state){ DontFail=state;};

	//Graham's HTML parsing code
public:
	//Entry points
	BOOL GetHTMLToken(BOOL fIgnoreEOL=TRUE, BOOL fCorrectCase=TRUE);

	char* GetHTMLTokenBuffer()
	{
		return m_pcHTMLBuffer;
	}

	BOOL IsHTMLTag()
	{
		return m_fIsTag;
	}

	BOOL IsEndOfHTMLFile()
	{
		return m_fEndOfHTMLFile;
	}

	String_256 GetHTMLTagName();
	String_256 GetHTMLParameterValue( const String_256 &strParameterName, BOOL fCorrectCase=TRUE);
	
protected:
	//HTML code member variables
	char			   *m_pcHTMLBuffer;			//The HTML buffer
	INT32					m_iCharsInHTMLBuffer;	//Number of chars in the buffer
	INT32					m_iLengthOfHTMLBuffer;	//Length of the buffer

	BOOL				m_fIsTag;				//Whether the token in the HTML buffer is a tag

	BOOL				m_fIsCharacterWaiting;	//Whether a character is waiting to be read
	char				m_cWaitingCharacter;	//The character waiting to be read
		
	BOOL				m_fEndOfHTMLFile;		//Whether we have reached the end of this file
	

protected:
	//HTML code internal functions
	void InitHTMLLexer();				//Initialises the HTML lexer

	void DeleteHTMLBuffer();			//Deletes the HTML buffer

	void AddToHTMLBuffer(char cToAdd);//Adds a character to the HTML buffer
	
	char PeekNextHTMLChar();	//Looks at the next character to be read, without reading it
	char ReadNextHTMLChar();	//Reads the next character from the file

	BOOL IsWhitespace(char cToTest);

	PCSTR FindStringWithoutCase( PCSTR strToSearch, PCSTR strToFind );

protected:
	// Variable to cache eof status; optimises lexical analysis.
	BOOL				EOFFound;

	BOOL				DontFail;

    // Input manipulation/parsing routines
	BOOL IsDelim();
	BOOL IsWhitespace();
	BOOL GetStringToken();

	// Flag set to TRUE if the token read by GetToken was a delimiter
	BOOL				DelimiterFound;

	// Flag to indicate whether or not to return the current token from GetToken(),
	// or get a new one from the file (the latter is the usual behaviour).
	BOOL				TokenIsCached;

	// Buffer to hold the current line being read
	String_256 *LineBuf;

	// And a pointer to the string data
	TCHAR			   *Buf;
	
	// Buffer to hold the current token
	TCHAR			   *TokenBuf;

	// Indicates what kind of token was read.
	LexTokenType		TokenType;
	
	// The current line number
	UINT32				Line;
	
	// Index into the buffer (the current character)
	UINT32				Ofs;

	// The current character
	char				Ch;
	
	// How many characters have been read.
	INT32				CharsRead;

	// File position of start of last line read - so we can return the correct
	// tell info...
	FilePos				LastLinePos;

	// Should we do the extra seeking code (which is a bit slower)
	BOOL				SeekingRequired;

	// Should escape codes in strings be ignored?
	// e.g. Should "hello\r" turn into "hello" + CR, or should we ignore the \r code and hence leave
	// the string unaltered?
	// I.e. if IgnoreStringEscapeCodes is TRUE, then the '\' character in strings is left alone.
	BOOL				IgnoreStringEscapeCodes;

	// Delimiter/white space definitions.
	// (These point to char arrays that hold a null-terminated set of characters that are
	// considered to be delimiters and whitespace respectively).
	char			   *WhitespaceChars;
	char			   *DelimiterChars;

	// Comment definition - similar to whitespace and delimiters, except that it is
	// holds one character that starts a comment.
	// e.g. for PostScript, it would be "%".
	// Comments are deemed to last until the end of the line (i.e. the first CR or LF found).
	char				CommentMarker;

	// These two characters are what bracket string tokens.
	// e.g. for C it would be "\"\"" and for PostScript it would be "()".
	char			   *StringDelimiters;

	// The number of spaces to output at the start of every line.
	// This can be changed using IncIndent() and DecIndent() and is used
	// by PutNewLine().  Together they provide a mechanism for outputting 
	// structures of tokens.
	// IndentDelta is the amount that IndentSpaces is increased/decreased by
	// IncIndent() and DecIndent().  It can be changed using SetIndentDelta()
	INT32				IndentSpaces;
	UINT32				IndentDelta;

private:
	// A flag to tell when the lexer has been initialised.
	BOOL LexerInitialised;
};



/********************************************************************************************
>	class CCStreamFile : public CCLexFile

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (based on a Mario-ism)
	Created:	13/8/96
	Purpose:	Represents a file class that uses an iostream type object for direct I/O.
				CCDiskFile is derived from this, using a standard fstream object for I/O.
				CCMemFile should be derived from this, but isn't, using a standard strstream
				object.  CCOleStream is also derived from this, using a custom costream
				object for I/O.
********************************************************************************************/

class CCAPI CCStreamFile : public CCLexFile
{                             
private:

	CC_DECLARE_DYNAMIC(CCStreamFile);

	char*		buffer;			   			// Fixed Size Buffer
	BOOL 		StreamFileInitialised;
                                            // Inherited from CCFile
protected:               
	
	iostream* 	IOFile;                 	// Underlying implementation is an iostream
	GZipFile*	GZFile;						// the gz file class we might use 
	GZipStream*	GZStream;					// the gz stream class we might use 
	INT32			ModeOfFile;					// file mode we are currently using, use ios flags	
	    
public:  
    										// Constructor
    CCStreamFile(iostream* pstream,
				 UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     		     BOOL ErrorReporting = TRUE,
     		     BOOL ExceptionThrowing = FALSE);
										 
                                        	// Constructor that opens stream
	CCStreamFile(const char* pcszPath, INT32 fileMode, 
				 INT32 fileAccess  = 0,
			     UINT32 bufferSize = CCFILE_DEFAULTSIZE,
			     BOOL ErrorReporting = TRUE,
			     BOOL ExceptionThrowing = FALSE);
	                                   
	virtual ~CCStreamFile();
	
	// As at present the contructors have errors in them due to memory claims rather
	// than doing these in a init function, we will introduce a flag to say whether
	// things have gone well so that if errors are supressed then a user can check. 
	BOOL IsInited();

	// Call this to change the iostream this classes uses.
	virtual iostream* SetStream(iostream* pStream);

	// Switch between text and binary modes. (filebuf::text is non-standard and the same as 0)
	virtual BOOL  setMode(INT32 fileMode = 0) { return(TRUE); };

	// Stream files are always open and cannot be closed.
	virtual BOOL isOpen() const;

	// This may be very slow for generic iostreams, or even impossible.
	virtual size_t Size();

	// This closes a CCStreamFile.  It is a no-op.
	virtual void close();

	// File pointer access/control.
	virtual CCFile&	 seekIn(FilePos);
	virtual CCFile&	 seekIn(INT32 Offset, ios::seekdir Dir);
	virtual FilePos  tellIn();
	virtual CCFile&  seek(FilePos pos);
	virtual FilePos	 tell();

	// Read functions.
	virtual CCFile&  read(void* buf, UINT32 length = 1);
	virtual CCFile&  read(StringBase *buf);
	virtual CCFile&  read(char& buf);
	                                
	// Write functions.
	virtual CCFile&  write(const void* buf, UINT32 length = 1);
	virtual CCFile&  write(const StringBase& buf, UINT32 length = 0);
	virtual CCFile&  write(char& buf);

	// Character access
	virtual CCFile&  get(char& buf);
	virtual CCFile&  put(const char& buf);

	// Flush the file object's buffer (if any).
	virtual CCFile&	 flush();

	// Status functions.
	virtual BOOL     good() const;
	virtual BOOL     bad() const;
	virtual BOOL     fail() const;
	virtual BOOL     eof() const;

	// Status setting function so that bad() will fail and somebody can force a good state.
	virtual void	SetBadState();
	virtual void	SetGoodState();

	// Allows access to the underlying iostream, if so required.
	// Mainly required for PNG/ZLib bits.  All non-disk file classes just return NULL.
	virtual iostream* GetIOFile() { return IOFile; }

	virtual BOOL InitCompression(BOOL Header = FALSE);
	virtual BOOL StartCompression();
	virtual BOOL StopCompression();

	virtual	INT32 GetCharsRead();

	// Get access to the file descriptor or handle for the file.  This is faked for
	// plain iostreams.
	virtual filedesc GetFileHandle() const;
};



/********************************************************************************************
>	class CCOleStream : public CCStreamFile

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/96
	Purpose:	Represents an IStream within an OLE Structured Storage.  Uses the custom
				iostream class costream for its implementation.
	SeeAlso:	IStream; IStorage; costream; costreambuf; CCStreamFile; CCDiskFile; CCLexFile
********************************************************************************************/

PORTNOTE("other","Removed CCOleStream - makes no sense on Linux")
#ifndef EXCLUDE_FROM_XARALX
class CCAPI CCOleStream : public CCStreamFile
{
public:

	// Creation & destruction.
	CCOleStream(IStream* pIStream = 0,
				UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     			BOOL ErrorReporting = TRUE,
     			BOOL ExceptionThrowing = FALSE);

	CCOleStream(PathName path, INT32 fileMode,
				INT32 fileAccess = filebuf::_Openprot,
				UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     			BOOL ErrorReporting = TRUE,
     			BOOL ExceptionThrowing = FALSE);

	// Attributes.
	virtual BOOL isOpen() const;
	virtual INT32 Size();
	virtual BOOL setMode(INT32 fileMode = filebuf::text);

	// Operations.
	virtual BOOL open_file(PathName fPath, INT32 fileMode, INT32 fileAccess = filebuf::_Openprot);
	virtual BOOL attach(IStream* pIstream);
	virtual void close();

protected:

	// These are shorthand.  Use them to refer to CCStreamFile::IOFile as a costream instead
	// of a plain ol' iostream.

	// following two cause memory exceptions !!!!!!  seems to be related to use of inline
	// always return TRASH !!!!!  Only solution was to make NOT inline ....

	// see more notes in ccfile.cpp (search on "causes it to throw a wobobley") ....
	
	//inline costream& GetCoStream() { return (costream&) *IOFile; }
	//inline const costream& GetCoStream() const { return (const costream&) *IOFile; }

	costream& GetCoStream() const;

private:

	// Implementation.
	CC_DECLARE_DYNAMIC(CCOleStream)
};
#endif

// WEBSTER-Martin-29/12/96
// no accusoft stuff
#ifndef WEBSTER

/********************************************************************************************
>	class CCOleAccusoftStream : public CCOleStream

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/96
	Purpose:	CCOleStream that manages the Accusoft stream switching as well.
	SeeAlso:	CCOleStream
********************************************************************************************/

PORTNOTE("other","Removed CCOleAccusoftStream - makes no sense on Linux")
#ifndef EXCLUDE_FROM_XARALX
class CCOleAccusoftStream : public CCOleStream
{
public:

	// Switch Accusoft to read from OLE streams.
	CCOleAccusoftStream(IStream* pIStream = 0,
						UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     					BOOL ErrorReporting = TRUE,
     					BOOL ExceptionThrowing = FALSE);

	// Switch Accusoft to read from OLE streams.
	CCOleAccusoftStream(PathName path, INT32 fileMode,
						INT32 fileAccess = filebuf::_Openprot,
						UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     					BOOL ErrorReporting = TRUE,
     					BOOL ExceptionThrowing = FALSE);

	// Switch Accusoft back again.
	virtual ~CCOleAccusoftStream();
};
#endif
#endif //WEBSTER


/********************************************************************************************
>	class CCDiskFile : public CCStreamFile

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/1993          
	Purpose:	Represents a file class that can offer buffered disk file I/O in both
				text and binary modes.
				Derived from the CCLexFile class so you can use lexical analysis
				functions on it.
********************************************************************************************/

class CCAPI CCDiskFile : public CCStreamFile
{                             
private:

	CC_DECLARE_DYNAMIC(CCDiskFile);

protected:               

    PathName    filePath;					// PathName of File

	// These are shorthand.  Use them to refer to CCStreamFile::IOFile as an fstream instead
	// of a plain ol' iostream.
	inline fstream& GetFileStream() { return (fstream&) *IOFile; }
	inline const fstream& GetFileStream() const { return (const fstream&) *IOFile; }
    
public:
    
    // Creation & destruction.
	CCDiskFile(UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     		   BOOL ErrorReporting = TRUE,
     		   BOOL ExceptionThrowing = FALSE);
	  // Creation & destruction.
	CCDiskFile( fstream * pfstream,
			   UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     		   BOOL ErrorReporting = TRUE,
     		   BOOL ExceptionThrowing = FALSE);

                                        
	CCDiskFile(PathName fPath, INT32 fileMode, 
			   INT32 fileAccess  = 0,
			   UINT32 bufferSize = CCFILE_DEFAULTSIZE,
			   BOOL ErrorReporting = TRUE,
			   BOOL ExceptionThrowing = FALSE);
	                                   
	                                      
	CCDiskFile(fstream * pfstream,
		       PathName fPath, INT32 fileMode, 
			   INT32 fileAccess  = 0,
			   UINT32 bufferSize = CCFILE_DEFAULTSIZE,
			   BOOL ErrorReporting = TRUE,
			   BOOL ExceptionThrowing = FALSE);

	// Open a file on a disk.
	virtual BOOL open( PathName fPath, INT32 fileMode, INT32 fileAccess = 0 );

	// Switch between text and binary modes. (0 is equiv. to depricated ios_base::text)
	virtual BOOL setMode(INT32 fileMode = 0);

	// Check if the file is open or not.
	virtual BOOL isOpen() const;

	// This closes a CCDiskFile.
	virtual void close();

	// Get the size of an open file in bytes.
	virtual size_t Size();

	// Find out whether we have a useful name to return which we can append to say
	// error messages to give feedback on what went wrong.
	virtual BOOL GetName( StringBase *name) const;
	virtual PathName GetPathName() const;

	// Get access to the file descriptor or handle for the file
	virtual filedesc GetFileHandle() const;
};



/********************************************************************************************

>	class CCMemFile : public CCLexFile

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/1993          
	Purpose:	Represents a memory IO file - IO is in byte units.
	Errors:		None.

********************************************************************************************/

class CCAPI CCMemFile : public CCLexFile
{
CC_DECLARE_DYNAMIC(CCMemFile);
protected:
	
    MHANDLE MemHandle;
	BYTE	*MemFile;
	size_t 	FileSize;
	UINT32	CurrentPos;
	BOOL	IsOpen;
	UINT32	FileProt;
	BOOL	WasError;									// TRUE if error occurred
	
	BOOL GrowMemFile();                           

	// A flag to tell when the disk file has been initialised ok.
	BOOL 		MemFileInitialised;

public:
	CCMemFile(BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE);
	
	CCMemFile(void* pFile, UINT32 size, FileAccess fProt = CCMemRead,
	          BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE);
	
	~CCMemFile();  
	
	// As at present the contructors have errors in them due to memory claims rather
	// than doing these in a init function, we will introduce a flag to say whether
	// things have gone well so that if errors are supressed then a user can check. 
	BOOL IsMemFileInited();

	BOOL GetBuffer(BYTE** ppBuffer, UINT32* pSize);

	BOOL open(void* pFile, UINT32 size, FileAccess fProt = CCMemRead);
    
	// Switch between text and binary modes.
	virtual BOOL  setMode(INT32 fileMode = 0);

	// Check if the file is open or not.
	virtual BOOL  	 isOpen() const;

	// File pointer access/control.
	virtual CCFile&	 seekIn(FilePos);
	virtual CCFile&	 seekIn(INT32 Offset, 
							ios::seekdir Dir);
	virtual FilePos  tellIn();
	virtual CCFile&  seek(FilePos pos);
	virtual FilePos	 tell();

	// Read functions.
	virtual CCFile&  read(void *buf, UINT32 length = 1);
	virtual CCFile&  read(StringBase *buf);
	virtual CCFile&  read(char& buf);
	                                
	// Write functions.
	virtual CCFile&  write(const void *buf, UINT32 length = 1);
	virtual CCFile&  write(const StringBase& buf, UINT32 length = 0);
	virtual CCFile&  write(char& buf);

	// Get the size of an open file in bytes.
	virtual size_t	 Size();

	// Status functions.
	virtual BOOL     good() const;
	virtual BOOL     bad() const;
	virtual BOOL     fail() const;
	virtual BOOL     eof() const;

	// Status setting function so that bad() will fail and somebody can force a good state.
	virtual void	SetBadState();
	virtual void	SetGoodState();

	// Allows access to the underlying fstream, is so required.
	// Mainly required for PNG/ZLib bits
	// All non-disk file classes just return NULL
	virtual iostream* GetIOFile() { return NULL; }

	// Find out whether we have a useful name to return which we can append to say
	// error messages to give feedback on what went wrong.
	virtual BOOL GetName( StringBase *name ) const;

	// Get access to the file descriptor or handle for the file
	virtual filedesc GetFileHandle() const;

	// Close the file.
    virtual void     close();

	virtual BOOL InitCompression(BOOL Header = FALSE);
	virtual BOOL StartCompression();
	virtual BOOL StopCompression();
};



/*********************************************************************************************

>	class CCMemTextFile : public CCMemFile

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/1993          
	Purpose:	Represents a character memory IO file which can cope with unicode.
	Errors:		None.

*********************************************************************************************/

class CCAPI CCMemTextFile : public CCMemFile
{
CC_DECLARE_DYNAMIC(CCMemTextFile);
	
public:
	CCMemTextFile(BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE);
	
	CCMemTextFile(char* pFile, UINT32 size, FileAccess fProt = CCMemRead,
				  BOOL ErrorReporting = TRUE,
				  BOOL ExceptionThrowing = FALSE);
	
	~CCMemTextFile();
	
	BOOL open(char* pFile, UINT32 size, FileAccess fProt = CCMemRead);
    
	// Read functions.
	//virtual CCFile&  read(void *buf, UINT32 length = 1);
	virtual CCFile&  read(StringBase *buf);
	virtual	CCFile&  read(char& buf);
	                                
	// Write functions.
	//virtual CCFile&  write(const void *buf, UINT32 length = 1);
	virtual CCFile&  write(const StringBase& buf, UINT32 length = 0);
	virtual	CCFile&  write(const char& buf);

	// Status functions.
	virtual BOOL     eof() const;
};




/*
	Alternate C-style I/O interfaces.  These global functions substitute for the standard
	C library's read, write, and seek functions, in particular those calls used by the
	Accusoft library.  The "file-descriptor" passed as the first parameter should be
	obtained by calling the fake_filedesc function.

	CAUTION: this won't work if sizeof(INT32) != sizeof(iostream*) !!

	Some wobbly code to show the general idea:-
		
		// Some C++ code that sets up and calls ...
		fstream fs("c:\\in.txt", ios::in);
		costream cs("c:\\out.txt", ios::out | costream::convert | costream::transacted);
		INT32 fd_fs = fake_filedesc(fs);
		INT32 fd_os = fake_filedesc(cs);
		
		// ... some C code that does I/O with C++ streams.
		char buf[256];
		INT32 n = stream_read(fd_fs, buf, sizeof(buf));
		if (n > 0 && stream_seek(fd_os, 0, SEEK_END)) stream_write(fd_os, buf, n);
*/

UINT_PTR fake_filedesc(iostream& io);
iostream* unfake_filedesc(UINT_PTR nFileDesc);

#if defined(__WXMSW__)
#define ACCUAPI_IO		FAR __cdecl
#else
#define ACCUAPI_IO
#endif

extern "C"
{
	INT32  ACCUAPI_IO stream_read(INT32 nFileDesc,  char* pchBuffer, unsigned nBufSize);
	INT32  ACCUAPI_IO stream_write(INT32 nFileDesc, char* pchBuffer, unsigned nBufSize);
	INT32 ACCUAPI_IO stream_seek(INT32 nFileDesc,  INT32 nOffset,	 unsigned fSeekType);
}

#endif


