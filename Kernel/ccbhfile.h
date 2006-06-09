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

#ifndef INC_CCBHFILE
#define INC_CCBHFILE

//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]

const UINT32 BinHexMaxLineLength = 100;

/********************************************************************************************

>	class CCBinHexFile : public CCLexFile

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/97
	Base Class:	CCLexFile
	Purpose:	Translates data from binary to BinHex format.

	Notes:		This class only checks pCCFile in debug builds

********************************************************************************************/

class CCBinHexFile : public CCLexFile
{                                   
	CC_DECLARE_DYNAMIC(CCBinHexFile);
public:
	// Constructor with default parameters 
	CCBinHexFile(CCLexFile* pCCFile, BOOL ErrorReporting = TRUE, BOOL ExceptionThrowing = FALSE)
		: CCLexFile(ErrorReporting, ExceptionThrowing), m_pFile(pCCFile) { /* empty */ };
	virtual ~CCBinHexFile() { /* empty */ }
	
	// Switch between text and binary modes.
	virtual BOOL  setMode(INT32 fileMode = 0)
	{
		return(m_pFile->setMode(fileMode));
	};

	// Check if the file is open or not.
	virtual BOOL  	 isOpen() const { return(m_pFile->isOpen()); };

	// File pointer access/control.
	virtual CCFile&	 seekIn(FilePos Pos) { return(m_pFile->seekIn(Pos)); };
	virtual CCFile&	 seekIn(INT32 Offset, ios::seekdir Dir)
	{
		return(m_pFile->seekIn(Offset, Dir));
	};
	virtual FilePos  tellIn() { return(m_pFile->tellIn()); };
	virtual CCFile&	 seek(FilePos pos) { return(m_pFile->seek(pos)); };
	virtual FilePos	 tell() { return(m_pFile->tell()); };

	// Read functions.
	virtual CCFile&  read(void *buf, UINT32 length = 1)
	{
		return(m_pFile->read(buf, length));
	};
	virtual CCFile&  read(StringBase *buf)
	{
		return(m_pFile->read(buf));
	};
	virtual CCFile&  read(char& buf)
	{
		return(m_pFile->read(buf));
	};
	                                
	// Write functions.
	virtual CCFile&  write(const void *buf, UINT32 length = 1);
	virtual CCFile&  write(const StringBase& buf, UINT32 length = 0);
	virtual CCFile&  write(char& buf);

	// Get the size of an open file in bytes.
	virtual size_t   Size() { return(m_pFile->Size()); };

	// Status functions.
	virtual BOOL     eof() const { return(m_pFile->eof()); };

	// Close the file.
    virtual void     close() { m_pFile->close(); };

	// Non-pure status functions.
	virtual BOOL     good() const { return(m_pFile->good()); };
	virtual BOOL     bad() const { return(m_pFile->bad()); };
	virtual BOOL     fail() const { return(m_pFile->fail()); };

	// Pure status setting functions.
	virtual void	SetBadState() { m_pFile->SetBadState(); };
	virtual void	SetGoodState() { m_pFile->SetGoodState(); };

	// Allows access to the underlying fstream, is so required.
	// Mainly required for PNG/ZLib bits
	// All non-disk file classes just return NULL
	virtual iostream* GetIOFile() { return(m_pFile->GetIOFile()); };

	// Flush the file object's buffer (if any).
	virtual CCFile&	 flush() { return(m_pFile->flush()); };

	// Get access to the file descriptor or handle for the file
	virtual filedesc GetFileHandle() const { return(m_pFile->GetFileHandle()); };

	// Allow setting of compression of output data
	virtual BOOL IsCompressionSet( ) { return(FALSE); };
	virtual BOOL SetCompression( BOOL ) { return(FALSE); };

	virtual BOOL InitCompression(BOOL Header = FALSE) { return(TRUE); };
	virtual BOOL StartCompression() { return(TRUE); };
	virtual BOOL StopCompression() { return(TRUE); };

protected:
	void ByteToHex(BYTE b, char* pPtr);

protected:
	CCLexFile* m_pFile;

	BYTE m_Buffer[BinHexMaxLineLength];
};

#endif	// INC_CCBHFILE
