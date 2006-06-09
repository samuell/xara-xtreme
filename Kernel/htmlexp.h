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

#ifndef INC_HTMLEXPORTFILTERS
#define INC_HTMLEXPORTFILTERS

//#include "filters.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class CCLexFile;

const UINT32 HTMLEXPORT_DEFAULTWORDWRAP = 100;

/**************************************************************************************

>	class HTMLExportFilter : public Filter

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Purpose:	The HTML export filter base class.

				This filter itself will not be added into the filter system - 
				derived classes of this will be added. At the moment, the
				only derived class is ImagemapFilter.

				This class contains the functionality for writing out
				HTML tags.

				

**************************************************************************************/
class HTMLExportFilter : public Filter
{
	// Declare the class for memory tracking
	CC_DECLARE_DYNCREATE(HTMLExportFilter);

	//Constructor
public:
	HTMLExportFilter();

	//Write functions
	//These functions either write to files or text buffers
	//These functions are static so that any object can use them
	//to write itself out to a text file
public:
	INT32 Write(UINT32 uiResource, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	
	INT32 Write(TCHAR* pcToWrite, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);

	INT32 Write(DocCoord dcToWrite, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);
	INT32 WriteNumber(INT32 lToWrite, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);
						  			
	INT32 WriteTag(UINT32 uiResource, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	
	INT32 WriteCloseTag(UINT32 uiResource, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	INT32 WriteStartOfTag(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	
	INT32 WriteStartOfTag(UINT32 uiResource, CCLexFile* pfileToWrite, TCHAR* pcBuffer);
	INT32 WriteStartOfTag(TCHAR* pcTag, CCLexFile* pfileToWrite, TCHAR* pcBuffer);
											  																	  
	INT32 WriteStartOfCloseTag(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	INT32 WriteEndOfTag(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	INT32 WriteParameter(UINT32 uiParameter, UINT32 uiValue, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	
	INT32 WriteParameter(UINT32 uiParameter, TCHAR* pcValue, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	
	INT32 WriteParameter(TCHAR* pcParameter, TCHAR* pcValue, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	INT32 WriteParameterInQuotes(UINT32 uiParameter, UINT32 uiValue, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	
	INT32 WriteParameterInQuotes(UINT32 uiParameter, TCHAR* pcValue, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	
	INT32 WriteParameterInQuotes(TCHAR* pcParameter, TCHAR* pcValue, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	INT32 WriteCoords(DocRect rectToWrite, TCHAR* pcValue, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	
	INT32 WriteCoords(Path* ppthToWrite, TCHAR* pcValue, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	
	INT32 WriteCircleCoords(DocCoord dcCentre, INT32 lRadius, TCHAR* pcValue, CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);	

	INT32 WriteEOL(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);
	INT32 WriteTab(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);

	INT32 WriteIndent(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL);

	//Indent functions
public:
	INT32 GetIndent()
	{
		return m_lIndent;
	}

	void SetIndent(INT32 lIndent)
	{
		m_lIndent=lIndent;
	}

	void Indent(INT32 lIndentInc=1)
	{
		m_lIndent+=lIndentInc;
	}

	void Unindent(INT32 lIndentDec=1)
	{
		m_lIndent-=lIndentDec;
	}

	void CancelIndent()
	{
		m_lIndent=0;
	}

	//Word wrap functions
public:

	void EnableWordWrap(BOOL fShouldWordWrap=TRUE)
	{
		m_fShouldWordWrap=fShouldWordWrap;
	}

	INT32 GetWordWrap()
	{
		return m_lWordWrap;
	}

	void SetWordWrap(INT32 lWordWrap)
	{
		m_lWordWrap=lWordWrap;
	}

				 
	//GetString functions
public:
	String_256 GetTagString(UINT32 uiResource);	
	String_256 GetCloseTagString(UINT32 uiResource);	

	//Member variables
public:
	//The number of characters written on the current line
	INT32 m_lCharsOnThisLine;

	//Whether to do word wrapping
	BOOL m_fShouldWordWrap;
	
	//The number of characters that should be written on a line before this
	//filter automatically puts an EOL in
	INT32 m_lWordWrap;

	//The number of tab stops by which the current text is indented
	INT32 m_lIndent;


};



#endif	// INC_HTMLEXPORTFILTER
