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



#ifndef INC_CCASYNCHFILE
#define INC_CCASYNCHFILE

//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class LoadContext;



/********************************************************************************************
>	class asynchstreambuf : public streambuf

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/96
	Purpose:	an asynch version of streambuf 
				It is assumed that two versions of the file will be open i.e. read and write.
				The read and write will run in seperate threads. Any access to the file that 
				might fail due to lack of data will cause the read thread to sleep until more 
				data is available.
				It is assumed that the write thread will never seek - this would corrupt the 
				BytesAvailable count.
				
	SeeAlso:	streambuf; ios; costream
********************************************************************************************/

class asynchstreambuf : public filebuf
{
private:
	
		
	// determine whether a read or seek can succed
	BOOL DataAvailable(INT32 Length);

	LoadContext * pLoadContext;

	wxCondition			*WakeEvents[2];

	// allow xsgetn to return less bytes than required without failing
	// this is used for LoadInitialSegment
	BOOL DontFail;

public:
	asynchstreambuf();
	~asynchstreambuf();

	void DownloadHasEnded(HRESULT Status);
    void SetDontFail(BOOL state){ DontFail = state;};
	
	virtual streampos seekoff(streamoff sOff ,ios::seekdir eDir,INT32 mode =ios::in|ios::out);
    virtual streampos seekpos(streampos,INT32 mode =ios::in|ios::out);	

    virtual INT32 xsputn(const char *,INT32);
    virtual INT32 xsgetn(char *,INT32);
	void SetLoadContext(LoadContext *pLContext);
	LoadContext *GetLoadContext() const {return pLoadContext;}

	// the total set of virtual functions in streambuf 
	/*
	// Creation & destruction.
	asynchstreambuf(IStream* pIStream = 0);
	virtual INT32 sync();
	virtual streambuf* setbuf(char *, INT32);
    virtual INT32 overflow(INT32 i =EOF) ; // pure virtual function
    virtual INT32 underflow() ;        // pure virtual function
  	virtual INT32 pbackfail(INT32);
	*/

	
};




/********************************************************************************************

>	class CCAsynchDiskFile : public CCDiskFile

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/1993          
	Purpose:	a file class that can be used for asynchronous loading. It contains an
				asynchstreambuf (see above)
				
********************************************************************************************/

class CCAPI CCAsynchDiskFile : public CCDiskFile
{                             
CC_DECLARE_DYNAMIC(CCAsynchDiskFile);
    
public:

	CCAsynchDiskFile(UINT32 bufferSize = CCFILE_DEFAULTSIZE,
     		   BOOL ErrorReporting = TRUE,
     		   BOOL ExceptionThrowing = FALSE);
     
		                                      
	CCAsynchDiskFile(PathName fPath, INT32 fileMode, 
			   INT32 fileAccess  = 0,
			   UINT32 bufferSize = CCFILE_DEFAULTSIZE,
			   BOOL ErrorReporting = TRUE,
			   BOOL ExceptionThrowing = FALSE);

	
	~CCAsynchDiskFile();



	// Uses the above flags to determine whether to report errors and/or throw exceptions
	virtual void GotError( UINT32 errorID, const TCHAR* errorString);	
	virtual void GotError( UINT32 errorID);
	
	virtual size_t   Size();
	virtual BOOL     eof() const;

	void SetDownloadContext(LoadContext * pLContext)
		{ ((asynchstreambuf*)(IOFile->rdbuf()))->SetLoadContext(pLContext);};

	void DownloadHasEnded(HRESULT Status);
	void SetDontFail(BOOL state){ ((asynchstreambuf*)(IOFile->rdbuf()))->SetDontFail(state);
									DontFail=state;};


};



/********************************************************************************************
>	class async_fstream : public fstream

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/96
	Purpose:		Converts fstream to use an async_filebuf.
********************************************************************************************/

class async_fstream : public fstream
{
public:
	async_fstream();
	async_fstream(const char * name, INT32 mode, INT32 prot);
	async_fstream(filedesc fd);
	async_fstream(filedesc fd, char* sbuf, INT32 len);

protected:
	streambuf * set_streambuf(streambuf* pbuf);
};



#endif


