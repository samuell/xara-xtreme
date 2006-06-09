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

                    
#include "camtypes.h"
#include "webaddr.h"

//#include "ccobject.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "resimmap.h"
	

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(WebAddress, CCObject)

/**********************************************************************************************

>	WebAddress::WebAddress()

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Default WebAddress class constructor
	Errors:		None

**********************************************************************************************/

WebAddress::WebAddress()
{
	Absolute = FALSE;
}

/**********************************************************************************************

>	WebAddress::WebAddress(const WebAddress& newPath)

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		InAddress
	Outputs:	None
	Returns:	None
	Purpose:	Copy Constructor for the WebAddress class
	Errors:		None

**********************************************************************************************/

WebAddress::WebAddress(const WebAddress& InAddress)
{
	Absolute = InAddress.Absolute;

	Scheme=InAddress.Scheme;
	NetLoc=InAddress.NetLoc;
	Path=InAddress.Path;
	Parameters=InAddress.Parameters;
	Query=InAddress.Query;					  
	Fragment=InAddress.Fragment;
}

/**********************************************************************************************

>	WebAddress& WebAddress::operator=(const WebAddress& InAddress)

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		InAddress
	Outputs:	None
	Returns:	None
	Purpose:	Assignment operator
	Errors:		None

**********************************************************************************************/

WebAddress& WebAddress::operator=(const WebAddress& InAddress)
{
	Absolute = InAddress.Absolute;

	Scheme=InAddress.Scheme;
	NetLoc=InAddress.NetLoc;
	Path=InAddress.Path;
	Parameters=InAddress.Parameters;
	Query=InAddress.Query;					  
	Fragment=InAddress.Fragment;

	return *this;
}

/**********************************************************************************************

>	WebAddress::WebAddress(const String_256&, WebCorrectFlags wcfToUse=WebCorrectFlags())

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		StringToParse	 String representing the path.
				wcfToUse		 Flags indicating how the string should be
								 corrected before it is parsed. Some options
								 are: www.xara.com should be corrected to 
								 http://www.xara.com/, d:\dir1\filename.gif
								 should be corrected to file://d|/dir1/filename.gif.

	Outputs:	None
	Returns:	None
	Purpose:	Corrects the string, then parses the corrected string.
	SeeAlso:	WebCorrectFlags

**********************************************************************************************/

WebAddress::WebAddress(const String_256& StringToParse, WebCorrectFlags wcfToUse)
{
	//First make a copy of the string we've been given
	String_256 strCopy=StringToParse;

	//And correct the copy
	Correct(&strCopy, wcfToUse);

	//Then parse the corrected string
	Parse(strCopy);

	//And finally set the class's flags
	SetFlags();
}

/**********************************************************************************************

>	INT32 WebAddress::operator==(WebAddress& Other)

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/97
	Inputs:		-

	Outputs:	-
	Returns:	-
	Purpose:	Equality operator
	SeeAlso:	WebCorrectFlags

**********************************************************************************************/

INT32 WebAddress::operator==(WebAddress& Other)
{
	return (Scheme==Other.Scheme &&
		NetLoc==Other.NetLoc &&
		Path==Other.Path &&
		Parameters==Other.Parameters &&
		Query==Other.Query &&
		Fragment==Other.Fragment);
}



/**********************************************************************************************

>	WebAddress::Parse()

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/96
	Inputs:		String representing the full URL
	Outputs:	None
	Returns:	None
	Purpose:	Takes the string and splits it into its component parts.

	Notes:		The parsing rules follow the guidelines given in 
				http://ds.internic.net/rfc/rfc1808.txt. So there! 
				(That means, don't change them unless you know what you're doing).

				These rules can be pretty unforgiving...if you get the query, fragment
				and parameters in the wrong order, the parsing rules don't accommodate
				this. But I'm sticking with the rules because of funny things like...
				a query could, theoretically, have a # in the middle of it.

				Note that this parsing function should split InString into
				several different strings and put those strings into
				member variables, but it should not change the actual text
				of InString. 
				
				In other words, if you concatenate the following member
				variables:

				Scheme+NetLoc+Path+Parameters+Query+Fragment

				You should *always* end up with the string InString you passed into
				the function.

				*Please* don't change the parsing rules unless you really know what
				you're doing and have read the guideline document referred to above.
				The parsing that is performed below may look easy, but it is
				performed by qualified professionals and is extremely dangerous.
				Do not imitate the parsing that you see below at home.

**********************************************************************************************/

BOOL WebAddress::Parse(const String_256& InString)
{
	//First set all the member strings to zero.
	Absolute=FALSE;

	Scheme.Empty();	
	NetLoc.Empty();
	Path.Empty();
	Parameters.Empty();
	Query.Empty();
	Fragment.Empty();
	Scheme.Empty();

	//If InString="", that's all we need to do...
	if (InString.IsEmpty())
		return TRUE;

	//And set up a couple of strings we can play around with
	String_256 StringToParse=InString;

	//If the parse string is empty, we need do nothing, because all our fields are 
	//already blank. Return TRUE.

	if (StringToParse.IsEmpty()) return TRUE;

	//Now set up some strings. These are all hard coded cos it reduces code size
	//This shouldn't matter for purposes of internationalisation, because URLs
	//are an international system.
	String_256 sNetloc="//";
		
	INT32 iLowerCaseA=INT32 ('a');
	INT32 iLowerCaseZ=INT32 ('z');
	INT32 iUpperCaseA=INT32 ('A');
	INT32 iUpperCaseZ=INT32 ('Z');

	char cBackslash='\\';
	char cSlash='/';
	char cHash='#';
	char cColon=':';
	char cQuestionmark='?';
	char cSemicolon=';';
	
	//First we want to find the Fragment section of the URL.
	//This should start with a #
	INT32 iFound=StringToParse.FindNextChar(cHash);

	//If we've found a #, copy the whole identifier into the "fragment" member variable
	if(iFound>=0)
		StringToParse.Split(&StringToParse, &Fragment, iFound, FALSE);

	//Now search the parse string for a scheme (the bit at the start,
	//e.g. http:). To do this we search for a colon.
	iFound=StringToParse.FindNextChar(cColon);

	//Have we found a colon?
	if(iFound>=0)
	{
		BOOL IsAScheme=TRUE;

		//Yes. We now need to check that everything before that colon is a letter.
		for (INT32 iStringPtr=(iFound-1); iStringPtr>=0; iStringPtr--)
		{
			if (!StringBase::IsAlpha(StringToParse[iStringPtr]))
			{
				IsAScheme=FALSE;
			}
		}

				
		//Was everything before the colon a letter?
		if (IsAScheme)
			//Yes, so split the string after that colon
			StringToParse.Split(&Scheme, &StringToParse, iFound, TRUE);
	}

	//Now look for a network location
	iFound=StringToParse.Sub(sNetloc);

	//Have we found a //?
	if(iFound>=0)
	{
		//Yes. So find the next / (or the end of the string)
		
		//To do this, set up a string pointer that starts from two
		//characters after iFound
		INT32 iStringPtr=iFound+2;

		//And move that string pointer forwards until
		//either it points at a slash or it reaches the end of the
		//string
		while (iStringPtr<StringToParse.Length() && StringToParse[iStringPtr]!=cSlash)
		{
			iStringPtr++;
		}
		
		StringToParse.Split(&NetLoc, &StringToParse, iStringPtr, FALSE);
	}

	//Now look for query information.
	iFound=StringToParse.FindNextChar(cQuestionmark);
		
	//Have we found a question mark?
	if(iFound>=0)
		StringToParse.Split(&StringToParse, &Query, iFound,FALSE);
	

	//Now look for parameter information.
	iFound=StringToParse.FindNextChar(cSemicolon);

	//Have we found a semicolon?
	if(iFound>=0)
		StringToParse.Split(&StringToParse, &Parameters, iFound, FALSE);

	//And whatever is left is the path.
	Path=StringToParse;
	

	return TRUE;
}

/**********************************************************************************************

>	void WebAddress::SetFlags()

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Purpose:	Sets the Web Address flags - at the moment the only flag is Absolute - by
				looking at the Web Address string member variables.

**********************************************************************************************/

BOOL WebAddress::SetFlags()
{
	Absolute=!Scheme.IsEmpty();

	return TRUE;
}

/**********************************************************************************************

>	void WebAddress::Combine(WebAddress Base)

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		A Base URL
	Purpose:	Combines this Web Address with a base Web Address.

				This is used when this Web Address is relative and the base
				web address is the address of the current document. This function
				allows this Web Address to inherit the appropriate components of the
				base address so this Address becomes relative.

				This is quite hard to explain, so here's an example: if this Web
				address is "myfile.xar" and the base Web Address is 
				"http://www.xara.com/currentfile.xar" then this function makes
				this Web Address into "http://www.xara.com/myfile.xar".

				This function follows the guidelines given in some international
				standard somewhere. (The one I was talking about above)

				Because it follows these guidelines (almost) exactly, this function
				contains one hell of a lot of loops within loops. Sorry! I've tried
				to comment them helpfully.

  Notes:		The parsing rules follow the guidelines given in 
				http://ds,internic.net/rfc/rfc1808.txt.

				*Please* don't change the parsing rules unless you really know what
				you're doing and have read the guideline document referred to above.
				The code below is written according to that document. Do not imitate
				at home what you have seen on Gladiators, etc, etc.

**********************************************************************************************/

void WebAddress::Combine(WebAddress Base)
{
	//Strings are hard coded to reduce code size
	String_256 sSlashDotSlash="/./";
	String_256 sSlashDotDotSlash="/../";
	
	TCHAR cSlash='/';

	//Now start the parsing.

	//If the base Web Address is empty, this Web Address does not change
	if (Base.IsEmpty())
		goto ReturnNow;

	//If this Web Address is empty, then it inherits everything from the 
	//base Web Address
	if (IsEmpty())
	{
		Absolute = Base.Absolute;

		Scheme=Base.Scheme;
		NetLoc=Base.NetLoc;
		Path=Base.Path;
		Parameters=Base.Parameters;
		Query=Base.Query;
		Fragment=Base.Fragment;
		goto ReturnNow;
	}

	//If this Web Address is absolute, it does not change
	if (IsAbsolute())
		goto ReturnNow;

	//If none of the above apply, this Web Address inherits the scheme of
	//the Base address
	Scheme=Base.Scheme;

	//If this Web Address has a net location, we need do nothing more to it.
	if (!NetLoc.IsEmpty())
		goto ReturnNow;

	//Otherwise, this Web Address inherits the Net Location of the base address.
	NetLoc=Base.NetLoc;

	//If the path of this Web Address starts with a slash, it is non-relative
	//and we need only add it on to the Net Location
	if ((*(Path))==cSlash) goto ReturnNow;


	//This is where all the loops within loops start. Hang on to your curly brackets.

	//Does this Web Address have a path?
	if (!Path.IsEmpty())
	{
		//Yes. Then the full path is given by:
		//The Base URL path up to the rightmost slash + the URL of this Web Address
		
		//This string will hold the base URL path up to the rightmost slash
		String_256 sTempPath="";
		
		//First find the rightmost slash in Base.Path
		INT32 iFound=Base.Path.ReverseFind(cSlash);
		
		//Did we find a slash?
		if (iFound>=0)
		{
			//Yes. So copy everything up to that slash into sTempPath


			Base.Path.Split(&sTempPath, NULL, iFound, TRUE);
			
			sTempPath+=Path;

			Path=sTempPath;
		}//End IF there's a slash in Base.Path

		
		//We now need to check the path for ../ and ./
		//First let's search for /./
		iFound=Path.Sub(sSlashDotSlash);

		//If we've found a /./
		while (iFound>=0)
		{
			//Then remove the ./ part of it.
			Path.Remove(iFound+1, 2);

			//And look for the next /./ to remove
			iFound=Path.Sub(sSlashDotSlash);
		}

		//Now we want to remove all occurrences of /[path segment/../
		//So first let's do a search for /../
		
		iFound=Path.Sub(sSlashDotDotSlash);

		//If we've found a /./
		while (iFound>=0)
		{
			//Then go back until we find the start of the path segment before it
			INT32 iStartOfSection=iFound-1;

			while (iStartOfSection>=0 && Path[iStartOfSection]!=cSlash)
				iStartOfSection--;

			if (iStartOfSection>=0)
			{
				//Get iStartOfSection to represent the character after the slash,
				//rather than the slash
				iStartOfSection++;

				//So if we've found something, then set a pointer to the end of that
				//section
				INT32 iEndOfSection=iFound+3;

				Path.Remove(iStartOfSection, (iEndOfSection-iStartOfSection+1));
			}
			else
				//If we found an unparsable /../, then break now before we
				//get into trouble...
				break;

			//And find the next /../
			iFound=Path.Sub(sSlashDotDotSlash);
		}	
	}
	else
	{
		//No, this Web Address doesn't have a path.
		//So it inherits the path of the base URL.
		Path=Base.Path;

		//If this Web Address has some parameters, we need do nothin more to it
		if (!Parameters.IsEmpty())
			goto ReturnNow;

		//Otherwise this Web Address inherits the parameters of the base URL
		Parameters=Base.Parameters;

		//If this Web Address has a query, we need do nothin more to it
		if (!Query.IsEmpty())
			goto ReturnNow;

		//Otherwise this Web Address inherits the parameters of the base URL
		Query=Base.Query;

		//And that's it. (Fragments are never inherited).
		//So we can return.
		goto ReturnNow;

	}//End if (!Path.IsEmpty()) else...

ReturnNow:
	//Set the "Absolute" flag to whatever it's meant to be, and return
	SetFlags();
	return;
}

/**********************************************************************************************

>	String_256 WebAddress::GetWebAddress()

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		-
	Returns:	The full URL string
	Purpose:	Returns the URL string

**********************************************************************************************/

String_256 WebAddress::GetWebAddress()
{
	String_256 strToReturn=Scheme;

	strToReturn+=NetLoc;
	strToReturn+=Path;
	strToReturn+=Parameters;
	strToReturn+=Query;
	strToReturn+=Fragment;

	return strToReturn;


}


/**********************************************************************************************

>	BOOL WebAddress::IsHTTP()

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		-
	Returns:	TRUE if this Web Address starts with HTTP
	Purpose:	Finds whether this Web Address starts with "HTTP:"

**********************************************************************************************/

BOOL WebAddress::IsHTTP() const
{
	return (Scheme.SubWithoutCase(String_256(_R(IDS_URL_HTTPCOLON)))==0);	
}

/**********************************************************************************************

>	BOOL WebAddress::IsValidHTTP()

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/97
	Inputs:		-
	Returns:	TRUE if this Web Address starts with HTTP and has a net location
	Purpose:	Finds whether this Web Address is a valid HTTP address.

				We say it's a valid HTTP address if:
				a. It starts with HTTP
				b. It has a net location

				Note that a valid http address may be valid but refer to a non-existent
				network location - for example:

				http://nonsensenonsense

				is a valid HTTP URL that refers to a server called "nonsensenonsense", which
				does not exist.

				

**********************************************************************************************/

BOOL WebAddress::IsValidHTTP() const
{
	return (IsHTTP() && !NetLocIsEmpty());	
}


/**********************************************************************************************

>	static BOOL WebAddress::IsHTTP(const String_256& strTest)

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/9/96
	Inputs:		-
	Returns:	TRUE if strTest starts with HTTP
	Purpose:	Finds whether strTest starts with "HTTP:"

**********************************************************************************************/

BOOL WebAddress::IsHTTP(const String_256& strTest)
{
	WebAddress urlTest(strTest);
	return (urlTest.IsHTTP());	
}


/********************************************************************************************

	BOOL WebAddress::IsLocalFile()  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/97
	Inputs:		-
	Outputs:	-

	Returns:	TRUE if we consider pstrCorrect to be a local filename
				FALSE otherwise
	
	Purpose:	Works out if we consider this Web Address to be a local filename.

				It's a local filename if its scheme is "file:"

	Errors:		-

	SeeAlso:	WebAddress::GetPathName()

********************************************************************************************/

BOOL WebAddress::IsLocalFile() const
{	
	return (Scheme=="file:");

}

/********************************************************************************************

	PathName WebAddress::GetPathName()  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/97
	Inputs:		-
	Outputs:	-

	Returns:	The PathName that is equivalent to this URL.
	
	Purpose:	This is used to convert a local file URL (that is, a URL
				whose scheme is "file:") to a pathname.

				Before calling this function, call "IsLocalFile()" to
				make sure the scheme is "file:".

				For example, this function converts:

				file://d|/dir1/file.htm to d:\dir1\file.htm

				file:////Netloc/dir1/file.htm to \\Netloc\dir1\file.htm

				It also converts the following URLs correctly. Note that
				these URLs are illegal - but they appear in the IE3 
				URL bar!

				file://d:\dir1\file.htm to d:\dir1\file.htm
				file://\\netloc\dir1\file.htm to \\netloc\dir1\file.htm

				It does this as follows:

				1. First, the "file:" part is removed

				2. Then, if the net location starts with "//", that "//"
				   is removed.

				3. All forward slashes are converted to backslashes

				4. All "|" characters are converted to colons

				Of course there's no guarantee that what's left is
				a valid path name.				

	Errors:		-

	SeeAlso:	WebAddress::IsLocalFile()

********************************************************************************************/

PathName WebAddress::GetPathName() const
{	
	//We needn't actually do anything to remove the Scheme - we just don't
	//use it

	//First concatenate the net location and the path
	String_256 strToReturn=NetLoc;
	strToReturn+=Path;
	
	//So, does that string start with "//"?
	if (strToReturn.Sub(String_256("//"))==0)
	{
		//Yes. So remove it
		strToReturn.Split(NULL, &strToReturn, 1, TRUE);
	}

	//Convert all forward slashes to backslashes
	strToReturn.SwapChar('/', '\\');

	//Convert all | characters to colons
	strToReturn.SwapChar('|', ':');

	//Put our string into a PathName object
	PathName pthToReturn(strToReturn);

	//And return it
	return pthToReturn;

}


/********************************************************************************************

	WebAddress correction functions

	These functions correct the URL. For example, www.xara.com might be corrected
	to http://www.xara.com/

********************************************************************************************/

/********************************************************************************************

	void WebAddress::Correct(String_256* pstrCorrect, WebCorrectFlags wcfToUse)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to correct
				wcfToUse	- a set of flags which detail how to correct
							  the string
	Outputs:	pstrCorrect	- the corrected string

	Returns:	-
	
	Purpose:	Corrects the URL string that the user has entered, according to the
				correction flags.

	Errors:		-

  SeeAlso:	WebAddress::ApplyWebAddressToSelection; WebCorrectFlags

********************************************************************************************/

void WebAddress::Correct(String_256* pstrCorrect, WebCorrectFlags wcfToUse)  
{	
	//If we should be correcting backslashes to forward slashes, do so now
	if (wcfToUse.CorrectBackslashes())
		CorrectBackslashes(pstrCorrect);

	//If we should be correcting local filenames
	if (wcfToUse.CorrectLocalFilenames() && ShouldCorrectLocalFilenames(pstrCorrect))
	{
		//Then correct it to a URL pointing to a local filename
		CorrectLocalFilenames(pstrCorrect);
	}

	//Now, if we should be correcting Netscape local URLs to IE3 ones
	if (wcfToUse.CorrectNetscapeFilenames() && ShouldCorrectNetscapeFilenames(pstrCorrect))
	{
		//Then correct it
		CorrectNetscapeFilenames(pstrCorrect);
	}
	//Otherwise if we should be correcting ftp addresses
	else if (wcfToUse.CorrectFTP() && ShouldCorrectFTP(pstrCorrect))
	{
		//Then add "ftp:" to the front
		CorrectFTP(pstrCorrect);
	}
	//Otherwise if we should be adding http:// to the front
	else if (wcfToUse.CorrectHTTP() && ShouldCorrectHTTP(pstrCorrect))
	{
		//Then add "http://" to the front
		CorrectHTTP(pstrCorrect);
			
	}
	//Otherwise if the string contains an @
	else if (ShouldCorrectMailto(pstrCorrect))
	{
		//Then add "mailto:" to the front
		CorrectMailto(pstrCorrect);
	}

	//If we should be ensuring that http addresses have a net location
	if (wcfToUse.CorrectNoNetLoc() && ShouldCorrectNoNetLoc(pstrCorrect))
	{
		//Then add a slash to the end
		CorrectNoNetLoc(pstrCorrect);
	}


	//And finally, if we should be adding a slash to the end
	if (wcfToUse.CorrectNoSlash() && ShouldCorrectNoSlash(pstrCorrect))
	{
		//Then add a slash to the end
		CorrectNoSlash(pstrCorrect);
	}

}

/********************************************************************************************

	void WebAddress::CorrectBackslashes(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to correct
	Outputs:	pstrCorrect	- the corrected string

	Returns:	-
	
	Purpose:	Corrects any backslashes in the entered string to slashes

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

void WebAddress::CorrectBackslashes(String_256* pstrCorrect)  
{	
	//Check our parameter
	if (pstrCorrect==NULL)
	{
		ERROR2RAW("WebAddress::CorrectBackslashes - NULL parameter");
		return;
	}

	//And correct all backslashes to forward slashes
	pstrCorrect->SwapChar('\\', '/');

}

/********************************************************************************************

	BOOL WebAddress::ShouldCorrectLocalFilenames(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to test
	Outputs:	-

	Returns:	TRUE if we consider pstrCorrect to be a local filename
				FALSE otherwise
	
	Purpose:	Works out if we consider this string to be a local filename.

				We consider this to be a local filename if
				a. It begins with a letter followed by a colon
				b. It begins with \\, in which case it is a UNC pathname

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

BOOL WebAddress::ShouldCorrectLocalFilenames(String_256* pstrCorrect)  
{	
	//Check our parameter
	ERROR2IF(pstrCorrect==NULL, FALSE, "WebAddress::CorrectBackslash - NULL parameter");
	
	//If the length of the string is less than two characters, return FALSE
	if (pstrCorrect->Length()<2)
		return FALSE;

	//Now, if the first character is a letter and the second character is a
	//colon, return TRUE
	if (StringBase::IsAlpha((*pstrCorrect)[0]) && (*pstrCorrect)[1]==':')
	{
		return TRUE;
	}
																			 
	//Otherwise, if the first two characters are forward slashes, return TRUE
	if ((*pstrCorrect)[0]=='/' && (*pstrCorrect)[1]=='/')
	{
		return TRUE;
	}
	
	//Otherwise return FALSE
	return FALSE;

}

/********************************************************************************************

	void WebAddress::CorrectLocalFilenames(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to correct
	Outputs:	pstrCorrect - the corrected string

	Returns:	-
	
	Purpose:	Assumes pstrCorrect is a local filename, then converts
				it to a URL referring to a local filename.

				It does this by calling the function PathName::GetWebAddress()

	Errors:		-

	SeeAlso:	WebAddress::Correct(), PathName::GetWebAddress()

********************************************************************************************/

void WebAddress::CorrectLocalFilenames(String_256* pstrCorrect)  
{	
	//Check our parameter
	if(pstrCorrect==NULL)
	{
		ERROR2RAW("WebAddress::CorrectBackslash - NULL parameter");
		return;
	}

	
	//Now, create a pathname out of our string
	PathName pthTemp=*pstrCorrect;

	//And call the GetWebAddress function
	*pstrCorrect=pthTemp.GetWebAddress();

}

/********************************************************************************************

	BOOL WebAddress::ShouldCorrectNetscapeFilenames(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/97
	Inputs:		pstrCorrect	- the string to test
	Outputs:	-

	Returns:	TRUE if we consider pstrCorrect to be a Netscape local filename.
				FALSE otherwise
	
	Purpose:	Works out if we consider this string to be a Netscape local filename.

				We consider this to be a Netscape local filename if it begins
				with "file:///", but does not begin with "file:////".

				The problem here is that Netscape refers to local filenames as follows:

				file:///d|/dir1/myname.htm

				But IE3 refers to local filenames as:

				file://d|/dir1/myname.htm

				Note the difference in the number of slashes. We correct it to the IE3 version.
				

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

BOOL WebAddress::ShouldCorrectNetscapeFilenames(String_256* pstrCorrect)  
{	
	//Check our parameter
	ERROR2IF(pstrCorrect==NULL, FALSE, "WebAddress::CorrectNetscapeFilenames - NULL parameter");

	//Set up some strings to search for
	String_256 strThreeSlashes("file:///");
	String_256 strFourSlashes("file:////");

	//Now, if the string begins with "file:///"...
	if (pstrCorrect->SubWithoutCase(strThreeSlashes)==0)
	{
		//But the string doesn't begin with "file:////"
		if (pstrCorrect->SubWithoutCase(strFourSlashes)!=0)
			//Then we shoul correct it
			return TRUE;
	}
		
	//Otherwise return FALSE
	return FALSE;

}

/********************************************************************************************

	void WebAddress::CorrectNetscapeFilenames(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to correct
	Outputs:	pstrCorrect - the corrected string

	Returns:	-
	
	  Purpose:	Converts a Netscape local filename (file:///xxxxxxx) to an IE3 local filename
				(file://xxxxxxx) by simply removing the third slash.

	Errors:		-

	SeeAlso:	WebAddress::Correct(), PathName::GetWebAddress()

********************************************************************************************/

void WebAddress::CorrectNetscapeFilenames(String_256* pstrCorrect)  
{	
	//Check our parameter
	if(pstrCorrect==NULL)
	{
		ERROR2RAW("WebAddress::CorrectBackslash - NULL parameter");
		return;
	}
																		
	//Simply delete the fifth character, removing one of the slashes
	pstrCorrect->Remove(6, 1);

}

/********************************************************************************************

	BOOL WebAddress::ShouldCorrectFTP(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to test
	Outputs:	-

	Returns:	TRUE if we consider pstrCorrect to be an ftp address
				FALSE otherwise
	
	Purpose:	Works out if we consider this string to be a ftp address with
				the "ftp:" missing.

				We do so if the string starts "ftp.".

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

BOOL WebAddress::ShouldCorrectFTP(String_256* pstrCorrect)  
{	
	//Check our parameter
	ERROR2IF(pstrCorrect==NULL, FALSE, "WebAddress::CorrectBackslash - NULL parameter");
	
	//Are the first four characters "ftp." ?
	if (pstrCorrect->SubWithoutCase(String_256("ftp."))==0)
		return TRUE;
	
	//Otherwise return FALSE
	return FALSE;

}

/********************************************************************************************

	void WebAddress::CorrectFTP(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to correct
	Outputs:	pstrCorrect - the corrected string

	Returns:	-
	
	  Purpose:	Puts "ftp:" on the start of pstrCorrect.

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

void WebAddress::CorrectFTP(String_256* pstrCorrect)  
{	
	//Check our parameter
	if(pstrCorrect==NULL)
	{
		ERROR2RAW("WebAddress::CorrectBackslash - NULL parameter");
		return;
	}
	
	//If our string is over two hundred and fifty characters long, return
	if (pstrCorrect->Length()>250)
		return;

	//Otherwise, create a new string containing "ftp://"
	String_256 strToReturn="ftp://";

	//Add our other string onto the end of it
	strToReturn+=*pstrCorrect;

	//And copy our new string into our old string
	*pstrCorrect=strToReturn;
}

/********************************************************************************************

	BOOL WebAddress::ShouldCorrectHTTP(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to test
	Outputs:	-

	Returns:	TRUE if we consider pstrCorrect to be an http address
				FALSE otherwise
	
	Purpose:	Works out if we consider this string to be a http address with
				the "http:" missing.

				We do so if the string starts with a string of the following
				characters:
				
				  ;?:&=%$-_.+!*'(),abcdefghijklmnopqrstuvwxyz
				  ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890

				...which contains at least two full stops. (This string
				of characters is taken from http://ds.internic.net/rfc/rfc1808.txt,
				page 4, but altered slightly).
				
				The first character should not be a full stop and there
				should not be two full stops together.

				For example:		

				abc-def12.gh4i3.jkl.mno

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

BOOL WebAddress::ShouldCorrectHTTP(String_256* pstrCorrect)  
{	
	//Check our parameter
	ERROR2IF(pstrCorrect==NULL, FALSE, "WebAddress::CorrectBackslash - NULL parameter");

	//Get the length of the string
	INT32 iLength=pstrCorrect->Length();

	//If the string is empty, return FALSE
	if (iLength<=0)
		return FALSE;

	//Now, here is the set of characters to test for
	TCHAR* strAllowed=";?&=%abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890$-_.+!*'(),";
	
	//If the first character is a full stop, return FALSE
	if ((*pstrCorrect)[0]=='.')
		return FALSE;

	//And this variable will keep a count of the number of full stops
	INT32 iFullStops=0;

	//This variable will tell us if the last character we looked at
	//was a full stop
	BOOL fLastCharWasFullStop=FALSE;

	//Now, starting at the first character in the string, scan through
	//each character in turn until we get to something that is not
	//in our set of allowed characters
	for (INT32 i=0; i<iLength && (camStrchr(strAllowed, (*pstrCorrect)[i]))!=NULL; i++)
		 {
			 //If the letter we are looking at is a full stop,
			  if ((*pstrCorrect)[i]=='.')
			  {
				  //Then, was the last character we looked at a full stop?
				  if (fLastCharWasFullStop)
				  {
					  //Yes. So we've found two full stops together. Return FALSE
					  return FALSE;
				  }
				  else
				  {
					  //No. So add one to our count of full stops and remember
					  //that this character was a full stop
					   iFullStops++;
					   fLastCharWasFullStop=TRUE;
				  }
			  }
			  else
			  {
				  //No, this character is not a full stop
				  fLastCharWasFullStop=FALSE;
			  }
		 }

	//And return true if we found more than one full stop	
	return (iFullStops>=2);

}

/********************************************************************************************

	void WebAddress::CorrectHTTP(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to correct
	Outputs:	pstrCorrect - the corrected string

	Returns:	-
	
	  Purpose:	Puts "http:" on the start of pstrCorrect.

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

void WebAddress::CorrectHTTP(String_256* pstrCorrect)  
{	
	//Check our parameter
	if(pstrCorrect==NULL)
	{
		ERROR2RAW("WebAddress::CorrectBackslash - NULL parameter");
		return;
	}
	
	//If our string is over two hundred and fifty characters long, return
	if (pstrCorrect->Length()>250)
		return;

	//Otherwise, create a new string containing "http:"
	String_256 strToReturn="http://";

	//Add our other string onto the end of it
	strToReturn+=*pstrCorrect;

	//And copy our new string into our old string
	*pstrCorrect=strToReturn;
}

/********************************************************************************************

	BOOL WebAddress::ShouldCorrectNoSlash(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to test
	Outputs:	-

	Returns:	TRUE if we consider that pstrCorrect is a URL to which the
				user has forgotten to add a trailing slash
				FALSE otherwise
	
	Purpose:	Works out if we consider this string to be a URL with a trailing
				slash missing.

				We do this by creating a Web Address object to parse the string.
				(Note that if this function is called, then the URL will already
				have had an "http://" added to the front if necessary).

				Then we consider that the URL should have a slash added if
				a. The string consists of a net location but no path 
				(e.g. "http://www.xara.com", which should become 
				"http://www.xara.com/")

				b. The path portion of the URL does not end in a slash, and
				   the final segment of the path does not have a full stop
				   character in it.

					That is, "http://www.bbc.co.uk/radio1" should have a slash
					added on the end, but "http://www.bbc.co.uk/index.htm" should
					not.


	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

BOOL WebAddress::ShouldCorrectNoSlash(String_256* pstrCorrect)  
{	
	//Check our parameter
	ERROR2IF(pstrCorrect==NULL, FALSE, "WebAddress::CorrectNoSlash - NULL parameter");
	
	//First create a Web Address from our string
	WebAddress urlTest(*pstrCorrect);

	//Now, does the Web Address start with HTTP:, and does it have a net location
	//specified?
	if (urlTest.IsHTTP() && !urlTest.NetLocIsEmpty())
	{
		//Yes.

		//So, does the Web Address have an empty path?
		if (urlTest.PathIsEmpty())
		{
			//Yes. So we should add a slash to it.
			return TRUE;
		}
		else
		{
			//No, there is something in the path.

			//So, does the path end in something other than a slash?
			INT32 iLength=urlTest.Path.Length();

			if (urlTest.Path[iLength-1]!='/')
			{
				//Yes. So we should add a slash only if 
				//the last segment of the path does not contain a full stop.

				//First try and find a slash in the path string, starting
				//from the end
				INT32 iSlashFound=urlTest.Path.ReverseFind('/');

				//Now try and find a full stop in the path string, starting 
				//from the end
				INT32 iFullStopFound=urlTest.Path.ReverseFind('.');

				//Now...

				//If we didn't find a full stop, we should add a slash
				if (iFullStopFound==-1)
					return TRUE;

				//Otherwise, we should only add a slash if the slash
				//was found after the full stop
				if (iSlashFound>iFullStopFound)
					return TRUE;

				//Otherwise, we don't add a slash
			}
		}
	}

	//So, none of our conditions were met.
	//So we don't add a slash
	return FALSE;
}

/********************************************************************************************

	void WebAddress::CorrectNoSlash(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to correct
	Outputs:	pstrCorrect - the corrected string

	Returns:	-
	
	Purpose:	Adds a slash to the end of the string


	Errors:		-

	SeeAlso:	WebAddress::Correct(); WebAddress::ShouldCorrectNoSlash()

********************************************************************************************/

void WebAddress::CorrectNoSlash(String_256* pstrCorrect)  
{	
	//Check our parameter
	if(pstrCorrect==NULL)
	{
		ERROR2RAW("WebAddress::CorrectNoSlash - NULL parameter");
		return;
	}
	
	//And if the string is long enough
	if (pstrCorrect->Length()<255)
	{
		//Add a slash to the end
		*pstrCorrect+='/';
	}
}





/********************************************************************************************

	BOOL WebAddress::ShouldCorrectMailto(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to test
	Outputs:	-

	Returns:	TRUE if we consider pstrCorrect to be a mail address
				FALSE otherwise
	
	Purpose:	Works out if we consider this string to be a mail address with
				the "mailto:" missing.

				We do so if it contains an @ character and does not
				contain a colon

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

BOOL WebAddress::ShouldCorrectMailto(String_256* pstrCorrect)  
{	
	//Check our parameter
	ERROR2IF(pstrCorrect==NULL, FALSE, "WebAddress::CorrectBackslash - NULL parameter");
	
	//Find the first @ symbol
	INT32 iLocationOfAtSymbol=pstrCorrect->FindNextChar('@');

	//If we found one
	if (iLocationOfAtSymbol>0)
	{
		//Then check for a colon
		INT32 iLocationOfColon=pstrCorrect->FindNextChar(':');

		//If we didn't find one, return TRUE
		if (iLocationOfColon<0)
			return TRUE;
	}
	
	return FALSE;

}

/********************************************************************************************

	void WebAddress::CorrectMailto(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to correct
	Outputs:	pstrCorrect - the corrected string

	Returns:	-
	
	  Purpose:	Puts "mailto:" on the start of pstrCorrect.

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

void WebAddress::CorrectMailto(String_256* pstrCorrect)  
{	
	//Check our parameter
	if(pstrCorrect==NULL)
	{
		ERROR2RAW("WebAddress::CorrectBackslash - NULL parameter");
		return;
	}
	
	//If our string is over two hundred and fifty characters long, return
	if (pstrCorrect->Length()>250)
		return;

	//Otherwise, create a new string containing "mailto:"
	String_256 strToReturn="mailto:";

	//Add our other string onto the end of it
	strToReturn+=*pstrCorrect;

	//And copy our new string into our old string
	*pstrCorrect=strToReturn;
}

/********************************************************************************************

	BOOL WebAddress::ShouldCorrectNoNetLoc(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/97
	Inputs:		pstrCorrect	- the string to test
	Outputs:	-

	Returns:	TRUE if pstrCorrect starts with "http" but does not
				have "//" afterwards to signify a net location
				FALSE otherwise
	
	Purpose:	Works out if we consider this string to be an HTTP address
				with the "//" to signify a network location missing. For example:

				http:www.xara.com

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

	BOOL WebAddress::ShouldCorrectNoNetLoc(String_256* pstrCorrect)  
{	
	//Check our parameter
	ERROR2IF(pstrCorrect==NULL, FALSE, "WebAddress::CorrectBackslash - NULL parameter");
	
	//Return TRUE if the string starts with "http:" but doesn't start with "http://"
	String_256 strHTTP("http:");
	String_256 strHTTPSlashSlash("http://");

	return (pstrCorrect->SubWithoutCase(strHTTP)==0 && pstrCorrect->SubWithoutCase(strHTTPSlashSlash)!=0);

}

/********************************************************************************************

	void WebAddress::CorrectNoNetLoc(String_256* pstrCorrect)  

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/97
	Inputs:		pstrCorrect	- the string to correct
	Outputs:	pstrCorrect - the corrected string

	Returns:	-
	
	  Purpose:	Ensures that pstrCorrect starts with "http://"

	Errors:		-

	SeeAlso:	WebAddress::Correct()

********************************************************************************************/

void WebAddress::CorrectNoNetLoc(String_256* pstrCorrect)  
{	
	//Check our parameter
	if(pstrCorrect==NULL)
	{
		ERROR2RAW("WebAddress::CorrectBackslash - NULL parameter");
		return;
	}
	
	//We already know that our string does not start with "http://"
	//But does it start with "http:/"?
	if (pstrCorrect->SubWithoutCase(String_256("http:/"))==0)
	{
		//Yes it does. So insert an extra slash
		pstrCorrect->Insert(&String_256("/"), 5);
	}
	else
	{
		//No it doesn't. So insert an extra two slashes after the HTTP
		pstrCorrect->Insert(&String_256("//"), 5);
	}
}





