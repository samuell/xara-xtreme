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

// The module that controls the OIL side of plugin import/export filters.

#ifndef INC_XMLUTILS
#define INC_XMLUTILS

#include <libxml/xpathInternals.h>

class CXMLUtils : public CCObject
{
public:
	CXMLUtils() {;}
	virtual ~CXMLUtils() {;}

	static BOOL			Initialise();
	static BOOL			DeInitialise();

	// All unicode data is stored as UTF-8 in libxml2 and xmlChar* is a pointer to a UTF-8 byte stream
	// Remember that unicode data is stored as UTF-16 in XAR files, as UTF-16 in memory on Windows builds
	// but as UTF-4/UTF-32 in memory on Linuxbuilds. So we need conversion functions!
	static wxString 	ConvertToWXString(const xmlChar*);
	static void			ConvertToXMLString(wxString str, xmlChar** pxmlstr);


#if _DEBUG
	static void Test();
	static INT32 example4(const char *filename, const xmlChar * xpathExpr, const xmlChar * value);
	static void update_xpath_nodes(xmlNodeSetPtr nodes, const xmlChar * value);
#endif


/*	static IXMLDOMDocument2Ptr NewDocument();
	static IXMLDOMDocument2Ptr NewDocument(IXMLDOMDocument2Ptr pCopySourceDoc);

	static PrepareDocument(IXMLDOMNodePtr pNode);
	static PrepareDocument(IXMLDOMDocument2Ptr pDoc2);
	static AddEncodingInstruction(IXMLDOMDocument2Ptr pDoc2,
									const CString& strVersion,
									const CString& strEncoding);

	static IXMLDOMElementPtr AppendElement(IXMLDOMElementPtr pContext, const CString& elname, const CString* value = NULL);
	static IXMLDOMElementPtr AppendElement(IXMLDOMElementPtr pContext, const CString& elname, const INT32 lValue);
	static IXMLDOMElementPtr AppendElement(IXMLDOMElementPtr pContext, const CString& elname, const BOOL bValue);

	static BOOL		ReplaceElement(IXMLDOMElementPtr pElement, const CString& strElPath, const CString& strText, const BOOL bIgnoreBlank = FALSE);
	static BOOL		ReplaceElement(IXMLDOMElementPtr pElement, const CString& strElPath, const INT32 lValue);
	static BOOL		ReplaceElement(IXMLDOMElementPtr pElement, const CString& strElPath, const BOOL bValue);

	static CString	GetNodeString(IXMLDOMNodePtr pNode, const CString& strNodePath, const CString& strDefault = _T(""));
	static INT32		GetNodeLong(IXMLDOMNodePtr pNode, const CString& strNodePath, const INT32 lDefault = 0);
	static BOOL		GetNodeBOOL(IXMLDOMNodePtr pNode, const CString& strNodePath, const BOOL bDefault = FALSE);

	static CString	GetAttributeString(IXMLDOMElementPtr pNode, const CString& strAttrName, const CString& strDefault = _T(""));
	static INT32		GetAttributeLong(IXMLDOMElementPtr pNode, const CString& strAttrName, const INT32 lDefault = 0);
	static BOOL		GetAttributeBOOL(IXMLDOMElementPtr pNode, const CString& strAttrName, const BOOL bDefault = FALSE);

	static BOOL		SetAttribute(IXMLDOMNodePtr pNode, const CString& strName, const CString& strValue);
	static BOOL		SetAttribute(IXMLDOMNodePtr pNode, const CString& strName, const INT32 lValue);
	static BOOL		SetAttribute(IXMLDOMNodePtr pNode, const CString& strName, const BOOL bValue);

	static CString	GetNodeName(IXMLDOMNodePtr pNode);
*/
};

#endif // INC_XMLUTILS

