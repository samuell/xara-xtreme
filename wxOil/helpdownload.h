// $Id: helpdownload.h 662 2006-03-14 21:31:49Z alex $
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


#ifndef INC_HELPDOWNLOAD
#define INC_HELPDOWNLOAD


#include "inetop.h"


///////////////////////////////////////////////////////////////////////////////
/////////////////		Help and Support files download		///////////////////
///////////////////////////////////////////////////////////////////////////////



/****************************************************************************

>	class HelpDownloadParam : public DownloadOpParam

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Purpose:	File Download param, download a file in the OpMenuImport::Do().
	Purpose:	Download operation parameter structure - passes the minimum necessary information to the Op

****************************************************************************/

class HelpDownloadParam : public DownloadOpParam
{
public:

	HelpDownloadParam();
	~HelpDownloadParam();

	PathName m_pathDestination;
	String_16 m_strResourceType;
	COleDateTime m_date;
	IXMLDOMDocumentPtr m_pIndexDoc;
	IXMLDOMNodePtr m_pOptionNode;
};


/****************************************************************************

>	class HelpDownloadOp : public DownloadOp

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2004
	Purpose:	File download operation. Download a file 
	SeeAlso: -

****************************************************************************/

#define OPTOKEN_HELPDOWNLOADOP _T("HelpDownloadOp")	// Optoken for the file download
#define OPTOKEN_HELPDOWNLOADDLG _T("HelpDownloadDlg")

class HelpDownloadOp : public DownloadOp
{
	CC_DECLARE_DYNCREATE(HelpDownloadOp)

public:
	static BOOL CheckForNewFiles(BOOL bForeground);

public:
	HelpDownloadOp() {};
	virtual ~HelpDownloadOp() {};

	static BOOL		Init();                        
	static OpState	GetState(String_256*, OpDescriptor*);
//	void			Do(OpDescriptor* od);
	virtual BOOL	OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State);
	static void		DeferFileCopy(const String_256& pTempFile, const String_256& pDestFile);
	static void		DoDeferredFileCopy();

protected:
	virtual void OnDownloadSuccess();
	virtual void OnDownloadFail();
	virtual void OnDownloadAbort();

protected:
	static CString GetSupportFilepath(const CString& strLeafName = _T(""), const CString& strSupportFolder = _T("HelpAndSupport\\"));
	static CString GetIndexFileForLocale(const CString& strLocale);
	static CString GetFileForLocale(const CString& strLocale, const CString& strFile);
	static CString GetFullResourcePath(const CString& strResourceType,
									   const CString& strFilename,
									   const CString& strLocale = _T(""));
	static BOOL MatchFileAttributes(const CString& strFilePath, const COleDateTime& datetime, UINT32 size);
	static BOOL CheckIndexFile();
	static BOOL FileOnCD(IXMLDOMDocumentPtr pDoc,
							  const CString& strName,
							  const COleDateTime& datetime,
							  UINT32 size,
							  const CString& strLocale,
							  const CString& strType);
	static BOOL LegalFileType(const CString& strFilename);
	static BOOL LegalPathname(const CString& strPathname);
	static BOOL HasBeenDownloaded(IXMLDOMNodePtr pNode);

	static BOOL ProcessDownloadIndex(IXMLDOMDocumentPtr pDoc, BOOL bSilent = FALSE, BOOL* pbDidSomething = NULL);
	static BOOL EnoughDiskSpace(const CString& strPath, const UINT32 sizeRequired);
	static IXMLDOMNodePtr FindFirstOption(IXMLDOMDocumentPtr pDoc,
										   CString* pstrResourceType,
										   CString* pstrLocale);

// Properties
public:
	static String_256 IndexBaseURL;
	static String_256 DeferredCopySrc;
	static String_256 DeferredCopyDest;
	static BOOL HighBandwidth;
	static String_32 IndexLeafName;
	static INT32 CurrentIndexRelease;
	
};


class HelpDownloadDlg : public Operation
{
	CC_DECLARE_DYNCREATE(HelpDownloadDlg)

public:
	HelpDownloadDlg() {};
	virtual ~HelpDownloadDlg() {};

	static BOOL		Init();                        
	static OpState	GetState(String_256*, OpDescriptor*);
	void			Do(OpDescriptor* od);
};


#endif // INC_HELPDOWNLOAD
