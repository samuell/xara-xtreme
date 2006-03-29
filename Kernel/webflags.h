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

#ifndef INC_WEBFLAGS
#define INC_WEBFLAGS

/*********************************************************************************************

>	class WebCorrectFlags: public CCObject

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/97
	Purpose:	A set of flags that define how a URL should be corrected.

				For example, should "www.xara.com" be corrected to 
				"http://www.xara.com/"? Should "d:\dir1\file.htm"
				be corrected to "file://d|/dir1/file.htm"? And so on...
	
*********************************************************************************************/

class WebCorrectFlags : public CCObject
{
	CC_DECLARE_DYNAMIC(WebCorrectFlags);

	//Member variables
protected:
	BOOL m_fBackslashes;	//TRUE if we should correct all backslashes to forward slashes

	BOOL m_fLocalFilenames;	//TRUE if we should correct local filenames to URLs
							//e.g. d:\dir1\file.htm to file://d|/dir1/file.htm
							//and \\netloc\dir1\file.htm to file:////netloc/dir1/file.htm

	BOOL m_fNetscapeFilenames;
							//TRUE if we should correct Netscape's local filename URLs
							//to IE3's local filename urls
							//i.e. file:///d|/dir1/filename.h to
							// file://d|/dir1/filename.h
							//(Note the different number of slashes after the "file:"


	BOOL m_fFTP;			//TRUE if we should correct ftp.xxxx.xxxx to
							//ftp://ftp.xxxx.xxxx

	BOOL m_fHTTP;			//TRUE if we should correct xxx.xxx.xxx to
							//http://xxx.xxx.xxx

	BOOL m_fMailto;			//TRUE if we should correct xxxxx@xxxxxxxxxxx
							//to mailto:xxxxx@xxxxxxxxxxx

	BOOL m_fNoSlash;		//TRUE if we should correct http://xxx.xxx.xxx
							//to http://xxx.xxx.xxx/
							//and http://xxx.xxx.xxx/xxx 
							//to http://xxx.xxx.xxx/xxx/

	BOOL m_fNoNetLoc;		//TRUE if we should correct http:xxxxxxxx
							//and http:/xxxxxxxxxx to 
							//http://xxxxxxxxxx


	//Access functions
public:
	BOOL CorrectBackslashes()
	{
		return m_fBackslashes;
	}

	BOOL CorrectLocalFilenames()
	{
		return m_fLocalFilenames;
	}

	BOOL CorrectNetscapeFilenames()
	{
		return m_fNetscapeFilenames;
	}
						  
	BOOL CorrectFTP()
	{
		return m_fFTP;
	}

	BOOL CorrectHTTP()
	{
		return m_fHTTP;
	}

	BOOL CorrectMailto()
	{
		return m_fMailto;
	}

	BOOL CorrectNoSlash()
	{
		return m_fNoSlash;
	}

	BOOL CorrectNoNetLoc()
	{
		return m_fNoNetLoc;
	}


	void SetCorrectBackslashes(BOOL fNewValue)
	{
		m_fBackslashes=fNewValue;
	}

	void SetCorrectLocalFilemames(BOOL fNewValue)
	{
		m_fLocalFilenames=fNewValue;
	}

	void SetCorrectNetscapeFilemames(BOOL fNewValue)
	{
		m_fNetscapeFilenames=fNewValue;
	}

	void SetCorrectFTP(BOOL fNewValue)
	{
		m_fFTP=fNewValue;
	}

	void SetCorrectHTTP(BOOL fNewValue)
	{
		m_fHTTP=fNewValue;
	}

	void SetCorrectMailto(BOOL fNewValue)
	{
		m_fMailto=fNewValue;
	}

	void SetCorrectNoSlash(BOOL fNewValue)
	{
		m_fNoSlash=fNewValue;
	}

	void SetCorrectNoNetLoc(BOOL fNewValue)
	{
		m_fNoNetLoc=fNewValue;
	}


	//Copy constructor, destructor and assignment operator
public:
	WebCorrectFlags& operator=(const WebCorrectFlags&);	
	WebCorrectFlags(const WebCorrectFlags& InFlags);	
	~WebCorrectFlags();	

	//Constructors
public:
	WebCorrectFlags()
	{
		m_fBackslashes=FALSE;
		m_fLocalFilenames=FALSE;
		m_fNetscapeFilenames=FALSE;
		m_fFTP=FALSE;
		m_fHTTP=FALSE;
		m_fMailto=FALSE;
		m_fNoSlash=FALSE;
		m_fNoNetLoc=FALSE;
	}

	WebCorrectFlags(BOOL fBackslashes, BOOL fLocalFilenames, BOOL fNetscapeFilenames, BOOL fFTP, BOOL fHTTP,	BOOL fMailto, BOOL fNoSlash, BOOL fNoNetLoc)
	{
		m_fBackslashes=fBackslashes;
		m_fLocalFilenames=fLocalFilenames;
		m_fNetscapeFilenames=fNetscapeFilenames;
		m_fFTP=fFTP;
		m_fHTTP=fHTTP;
		m_fMailto=fMailto;
		m_fNoSlash=fNoSlash;
		m_fNoNetLoc=fNoNetLoc;
	};

	//Functions to set the flags to appropriate presets
public:
	void SetForURLImport()
	{
		m_fBackslashes=TRUE;
		m_fLocalFilenames=TRUE;
		m_fNetscapeFilenames=TRUE;
		m_fFTP=FALSE;
		m_fHTTP=TRUE;
		m_fMailto=FALSE;
		m_fNoSlash=TRUE;
		m_fNoNetLoc=TRUE;
	}

	void SetForWebAddressDialog()
	{
		m_fBackslashes=TRUE;
		m_fLocalFilenames=TRUE;
		m_fNetscapeFilenames=TRUE;
		m_fFTP=TRUE;
		m_fHTTP=TRUE;
		m_fMailto=TRUE;
		m_fNoSlash=TRUE;
		m_fNoNetLoc=TRUE;
	}


	
			
};

#endif

