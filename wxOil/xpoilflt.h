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

#ifndef INC_XPOILFLT
#define INC_XPOILFLT

#include "oilfltrs.h"
PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
#include <atlbase.h>	// For CRegKey and CComPtr

#include "xpfapi.h"
#endif

#include "xpfcaps.h"

class PluginNativeFilter;

PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
class PluginFilterCallback : public CCmdTarget
{
	DECLARE_DYNCREATE( PluginFilterCallback )

public:
	PluginFilterCallback(PluginNativeFilter* pFilter = NULL);	// Constructor
	~PluginFilterCallback();									// Destructor

	IXPFCallback* GetInterface() { return &m_x_XPFCallback; }

	DECLARE_INTERFACE_MAP()
	
	BEGIN_INTERFACE_PART(_XPFCallback, IXPFCallback)
		STDMETHOD(Progress)(/*[in]*/INT32 lProgress);
	END_INTERFACE_PART(_XPFCallback)

protected:
	PluginNativeFilter* m_pFilter;
};
#endif



/********************************************************************************************

>	class PluginOILFilter : public OILFilter

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This class is used to encapsulate the platform specific features of a filter.
				Every Filter object has a pointer to an OILFilter object.  For example, 
				under Windows, it holds the file extensions that this file understands.
	SeeAlso:	Filter

********************************************************************************************/

class PluginOILFilter : public OILFilter
{
	CC_DECLARE_MEMDUMP(PluginOILFilter);

public:
	PluginOILFilter(Filter *pFilter);
	BOOL Init(const CLSID& rCLSID);

	BOOL IsImport() { return(m_bImport); }
	BOOL IsExport() { return(m_bExport); }

	INT32 HowCompatible(PathName& Filename);
	BOOL GetImportFile(CCLexFile* pFile, CCLexFile** ppNewFile);

	BOOL GetExportFile(PathName* pPath, CCLexFile** ppNewFile);
	BOOL GetCapabilities(CCLexFile* pFile, PathName* pPath, CapabilityTree* pCapTree);
	BOOL DoExport(CCLexFile* pXarFile, PathName* pPath);

	void Cleanup();

protected:
PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	BOOL CreateFilterObject();
	void ReleaseFilterObject();

	HRESULT BuildCapabilityTree(BSTR bsXML, CapabilityTree* pCapTree);

	HRESULT ReadOptions(IXMLDOMNode* pNode, CapabilityTree* pCapTree);
	HRESULT ReadRasterise(IXMLDOMNode* pNode, CapabilityTree* pCapTree);
	HRESULT ReadSpread(IXMLDOMNode* pNode, CapabilityTree* pCapTree);
	HRESULT ReadObjects(IXMLDOMNode* pNode, CapabilityTree* pCapTree);
	HRESULT ReadAttributes(IXMLDOMNode* pNode, CapabilityTree* pCapTree);
	HRESULT ReadColour(IXMLDOMNode* pNode, CapabilityTree* pCapTree);

	HRESULT GetConvertAsType(IXMLDOMNode* pNode, XPFConvertType* pValue);
	HRESULT GetXPFBOOL(IXMLDOMNode* pNode, LPTSTR pAttrName, XPFBOOL* pbValue);
	HRESULT GetXPFProp(IXMLDOMNode* pNode, LPTSTR pAttrName, PropMapEntry aMap[], XPFProp* pValue);
	XPFCapability* CreateObjectNode(IXMLDOMNode* pNode);
	XPFCapability* CreateAttributeNode(IXMLDOMNode* pNode);
	XPFCapability* CreateColourNode(IXMLDOMNode* pNode);
#endif

protected:
	BOOL m_bImport;
	BOOL m_bExport;

PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	CLSID m_CLSID;
	CComPtr<IXPFilter> m_pFilterObj;
	CComPtr<IStream> m_pXarStream;
#endif

public:
PORTNOTE("other","PluginFilter COM bits removed")
#if !defined(EXCLUDE_FROM_XARALX)
	static BOOL AutoRegisterFilters();
	static BOOL RegisterDLL(String_256& sPath);
	static BOOL GetFileVersion(PathName* pPath, String_64* pVersion);
	static INT32 CompareVersions(const String_64& lhs, const String_64& rhs);
#endif
};


class AutoCleanOILFilter
{
public:
	AutoCleanOILFilter(PluginOILFilter* pOILFilter) : m_pOILFilter(pOILFilter) {}
	~AutoCleanOILFilter()
	{
		if (m_pOILFilter)
			m_pOILFilter->Cleanup();
	}

protected:
	PluginOILFilter* m_pOILFilter;
};


class PathNameListItem : public ListItem
{
public:
	CC_DECLARE_DYNAMIC( PathNameListItem )
	PathNameListItem(const PathName& Path)
	{
		m_Path = Path;
	}
	
	PathName m_Path;
};


#endif // INC_XPOILFLT
