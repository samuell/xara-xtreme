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

#ifndef INC_WEBADDRESS
#define INC_WEBADDRESS
 
#include "webflags.h"	//Class WebCorrectFlags
#include "pathname.h"	//Class PathName

/*********************************************************************************************

>	class WebAddress : public CCObject

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/96
	Purpose:	Represents a URL.
	
*********************************************************************************************/

class WebAddress : public CCObject
{
	CC_DECLARE_DYNAMIC(WebAddress);

protected:
	BOOL Absolute;									//TRUE if it's Absolute. FALSE if it's relative.

	String_256 Scheme;								// e.g. http:, mailto:
	String_256 NetLoc;    	                		// e.g. //www.xara.com
	String_256 Path;        	            		// e.g. /dir1/dir2/myfile.web
	String_256 Parameters;							// e.g. ; XXXX=1
	String_256 Query;								// e.g. ? XXXXX
	String_256 Fragment;							//e.g.  # XXXXX

	BOOL Parse(const String_256& InString);
	BOOL SetFlags();

public:
	WebAddress& operator=(const WebAddress&);				// Assigns one PathName onto another

	//This combines a relative URL with a base URL
	void Combine(WebAddress Relative);

	//Some constructor functions
	WebAddress();
	WebAddress(const WebAddress& InAddress);	
	WebAddress(const String_256& StringToParse, WebCorrectFlags wcfToUse=WebCorrectFlags());

	INT32 operator==(WebAddress& Other);
			
	//A function to return the whole Web Address as a string
 	String_256 GetWebAddress();			// return the full URL

	//Functions to return parts of the Web Address as strings
	const String_256& GetScheme() const		{return Scheme;}
	const String_256& GetNetLoc() const		{return NetLoc;}
	const String_256& GetPath() const		{return Path;}
	const String_256& GetParameters() const	{return Parameters;}
	const String_256& GetQuery() const		{return Query;}
	const String_256& GetFragment() const	{return Fragment;}

	const BOOL IsAbsolute()				 	{return Absolute;}

	//These functions for Olivier's work
	BOOL IsHTTP() const;
	BOOL IsValidHTTP() const;
	static BOOL IsHTTP(const String_256& strTest);

	//Functions to check if the Web Address is a local file, and
	//to get the Web Address as a PathName
	BOOL IsLocalFile() const;
	PathName GetPathName() const;

	//Functions to check whether parts of the Web Address are empty
	const BOOL SchemeIsEmpty() const		{return Scheme.IsEmpty();}
	const BOOL NetLocIsEmpty() const		{return NetLoc.IsEmpty();}
	const BOOL PathIsEmpty() const			{return Path.IsEmpty();}
	const BOOL ParametersIsEmpty() const	{return Parameters.IsEmpty();}
	const BOOL QueryIsEmpty() const			{return Query.IsEmpty();}
	const BOOL FragmentIsEmpty() const		{return Fragment.IsEmpty();}

	const BOOL IsEmpty() const	{return ((Scheme.IsEmpty())&&
										(NetLoc.IsEmpty())&&
												(Path.IsEmpty())&&
												(Parameters.IsEmpty())&&
												(Query.IsEmpty())&&
												(Fragment.IsEmpty()));}

	//And functions to set parts of the Web Address
	void SetScheme(String_256& NewScheme)		{Scheme=NewScheme; SetFlags();}
	void SetNetLoc(String_256& NewNetLoc)		{NetLoc=NewNetLoc;}
	void SetPath(String_256& NewPath)			{Path=NewPath;}
	void SetParameters(String_256& NewPara)		{Parameters=NewPara;}
	void SetQuery(String_256& NewQuery)			{Query=NewQuery;}
	void SetFragment(String_256& NewFragment)	{Fragment=NewFragment;}

	//Correction functions
protected:
	void Correct(String_256* pstrCorrect, WebCorrectFlags wcfToUse=WebCorrectFlags());

	void CorrectBackslashes(String_256* pstrCorrect);
	void CorrectLocalFilenames(String_256* pstrCorrect);
	void CorrectNetscapeFilenames(String_256* pstrCorrect);
	void CorrectFTP(String_256* pstrCorrect);
	void CorrectHTTP(String_256* pstrCorrect);
	void CorrectMailto(String_256* pstrCorrect);
	void CorrectNoSlash(String_256* pstrCorrect);
	void CorrectNoNetLoc(String_256* pstrCorrect);

	BOOL ShouldCorrectLocalFilenames(String_256* pstrCorrect);
	BOOL ShouldCorrectNetscapeFilenames(String_256* pstrCorrect);
	BOOL ShouldCorrectFTP(String_256* pstrCorrect);
	BOOL ShouldCorrectHTTP(String_256* pstrCorrect);
	BOOL ShouldCorrectMailto(String_256* pstrCorrect);
	BOOL ShouldCorrectNoSlash(String_256* pstrCorrect);
	BOOL ShouldCorrectNoNetLoc(String_256* pstrCorrect);





		
};

#endif

