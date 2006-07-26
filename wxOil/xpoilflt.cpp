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

// The module that controls the OIL side of import/export filters.


#include "camtypes.h"

#include "xpoilflt.h"

#include "xpfilter.h"
//#include "xpfres.h"
//#include "mario.h"

#include "strlist.h"
#include "fileutil.h"
#include "sgliboil.h"

#include "ncntrcnt.h"
//#include "nev.h"		// For _R(IDN_USER_CANCELLED)
#include "xmlutils.h"
#include "fillattr2.h"

#include "camprocess.h"

CC_IMPLEMENT_MEMDUMP(PluginOILFilter, OILFilter)
CC_IMPLEMENT_DYNAMIC(PathNameListItem, ListItem)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

PropMapEntry aShadowTypes[] = { {_T("wall"), XPFP_SHADOWTYPE_WALL},
								{_T("floor"), XPFP_SHADOWTYPE_FLOOR},
								{_T("glow"), XPFP_SHADOWTYPE_GLOW},
								{_T("feather"), XPFP_SHADOWTYPE_FEATHER},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aBevelTypes[] = { {_T("flat"), XPFP_BEVELTYPE_FLAT},
								{_T("round"), XPFP_BEVELTYPE_ROUND},
								{_T("halfround"), XPFP_BEVELTYPE_HALFROUND},
								{_T("frame"), XPFP_BEVELTYPE_FRAME},
								{_T("mesa1"), XPFP_BEVELTYPE_MESA_1},
								{_T("mesa2"), XPFP_BEVELTYPE_MESA_2},
								{_T("smooth1"), XPFP_BEVELTYPE_SMOOTH_1},
								{_T("smooth2"), XPFP_BEVELTYPE_SMOOTH_2},
								{_T("point1"), XPFP_BEVELTYPE_POINT_1},
								{_T("point2a"), XPFP_BEVELTYPE_POINT_2a},
								{_T("point2b"), XPFP_BEVELTYPE_POINT_2b},
								{_T("ruffle2a"), XPFP_BEVELTYPE_RUFFLE_2a},
								{_T("ruffle2b"), XPFP_BEVELTYPE_RUFFLE_2b},
								{_T("ruffle3a"), XPFP_BEVELTYPE_RUFFLE_3a},
								{_T("ruffle3b"), XPFP_BEVELTYPE_RUFFLE_3b},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aBevelSides[] = { {_T("inner"), XPFP_BEVELSIDE_INNER},
								{_T("outer"), XPFP_BEVELSIDE_OUTER},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aColourEffects[] = { {_T("fade"), XPFP_COLOUREFFECT_FADE},
								{_T("rainbow"), XPFP_COLOUREFFECT_RAINBOW},
								{_T("altrainbow"), XPFP_COLOUREFFECT_ALTRAINBOW},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aMouldTypes[] = { {_T("envelope"), XPFP_MOULDTYPE_ENVELOPE},
								{_T("perspective"), XPFP_MOULDTYPE_PERSPECTIVE},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aLineCaps[] = { {_T("butt"), XPFP_LINECAP_BUTT},
								{_T("round"), XPFP_LINECAP_ROUND},
								{_T("square"), XPFP_LINECAP_SQUARE},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aLineJoins[] = { {_T("mitre"), XPFP_LINEJOIN_MITRE},
								{_T("round"), XPFP_LINEJOIN_ROUND},
								{_T("bevel"), XPFP_LINEJOIN_BEVEL},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aFillShapes[] = { {_T("flat"), FILLSHAPE_FLAT},
								{_T("linear"), FILLSHAPE_LINEAR},
								{_T("circular"), FILLSHAPE_CIRCULAR},
								{_T("elliptical"), FILLSHAPE_ELLIPTICAL},
								{_T("conical"), FILLSHAPE_CONICAL},
								{_T("diamond"), FILLSHAPE_DIAMOND},
								{_T("3point"), FILLSHAPE_3POINT},
								{_T("4point"), FILLSHAPE_4POINT},
								{_T("bitmap"), FILLSHAPE_BITMAP},
								{_T("clouds"), FILLSHAPE_CLOUDS},
								{_T("plasma"), FILLSHAPE_PLASMA},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aFillRepeats[] = { {_T("simple"), XPFP_FILLREPEAT_SIMPLE},
								{_T("repeat"), XPFP_FILLREPEAT_REPEAT},
								{_T("repeatinv"), XPFP_FILLREPEAT_REPEATINV},
								{_T("repeatex"), XPFP_FILLREPEAT_REPEATEXTRA},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aTransTypes[] = { {_T("none"), TT_NoTranspType},
								{_T("mix"), TT_Mix},
								{_T("stained"), TT_StainGlass},
								{_T("bleach"), TT_Bleach},
								{_T("contrast"), TT_CONTRAST},
								{_T("saturation"), TT_SATURATION},
								{_T("darken"), TT_DARKEN},
								{_T("lighten"), TT_LIGHTEN},
								{_T("brightness"), TT_BRIGHTNESS},
								{_T("luminosity"), TT_LUMINOSITY},
								{_T("hue"), TT_HUE},
								{_T("bevel"), TT_BEVEL},
								{NULL, XPFP_UNKNOWN}};

PropMapEntry aContentOnly[] = { {_T("text"), XPFP_CONTENTONLY_TEXT},
								{_T("plaintext"), XPFP_CONTENTONLY_PLAINTEXT},
								{NULL, XPFP_UNKNOWN}};


/********************************************************************************************

>	PluginOILFilter::PluginOILFilter()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	Sets up the list of filename extensions that this filter understands
				and handles connection to external plugin object

********************************************************************************************/

PluginOILFilter::PluginOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	m_bImport = FALSE;
	m_bExport = FALSE;
}




/****************************************************************************

>	BOOL PluginOILFilter::Init(const CLSID& rCLSID)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/2005

	Inputs:		rCLSID		- 
	Returns:	TRUE if ok, FALSE if we find a problem and shouldn't use it
	Purpose:	Initialises the OIL specific parts of this filter (file 
				extension, file type name, link to COM object etc).

****************************************************************************/

BOOL PluginOILFilter::Init( xmlNode* pFilterNode )
{
	// This would look a lot neater using functions from xmlutils.h

	m_InternalName = CXMLUtils::ConvertToWXString(xmlGetProp(pFilterNode, (xmlChar*)"name"));

	xmlNodePtr pChild = pFilterNode->children;
	// We will loop round until we run out of child elements
	// If an element has already been parsed then an error should be indicated

	while (pChild)
	{
		wxString strChildName = CXMLUtils::ConvertToWXString(pChild->name);
		
		if (strChildName == _T("#text") || xmlNodeIsText(pChild))
		{
			// ignore it
		}
		else if (strChildName == _T("DisplayName"))
		{
			xmlChar* pStr = xmlNodeListGetString(pChild->doc, pChild->xmlChildrenNode, 1);
			wxString sTemp = CXMLUtils::ConvertToWXString(pStr);
			FilterName = sTemp;
			xmlFree(pStr);
		}
		else if (strChildName == _T("Extensions"))
		{
			xmlChar* pStr = xmlNodeListGetString(pChild->doc, pChild->xmlChildrenNode, 1);
			wxString sTemp = CXMLUtils::ConvertToWXString(pStr);
			FilterExt = sTemp;
			xmlFree(pStr);
		}
		else if (strChildName == _T("CanImport"))
		{
			xmlChar* pStr = xmlNodeListGetString(pChild->doc, pChild->xmlChildrenNode, 1);
			m_CanImport = CXMLUtils::ConvertToWXString(pStr);
			xmlFree(pStr);
		}
		else if (strChildName == _T("DoImport"))
		{
			xmlChar* pStr = xmlNodeListGetString(pChild->doc, pChild->xmlChildrenNode, 1);
			m_DoImport = CXMLUtils::ConvertToWXString(pStr);
			xmlFree(pStr);
		}
		else if (strChildName == _T("PrepareExport"))
		{
			xmlChar* pStr = xmlNodeListGetString(pChild->doc, pChild->xmlChildrenNode, 1);
			m_PrepareExport = CXMLUtils::ConvertToWXString(pStr);
			xmlFree(pStr);
		}
		else if (strChildName == _T("DoExport"))
		{
			xmlChar* pStr = xmlNodeListGetString(pChild->doc, pChild->xmlChildrenNode, 1);
			m_DoExport = CXMLUtils::ConvertToWXString(pStr);
			xmlFree(pStr);
		}
		else
		{
			TRACEUSER("Gerry", _T("Ignoring '%s' element"), strChildName.c_str());
//			ERROR1(FALSE, _R(IDE_XPF_BADXML));
		}

		pChild = pChild->next;
	}

	// This should be set to some sensible path but I've hardcoded it for now
	// We should change to making ~/.XaraLX into a directory and store the main config
	// file and these filter config files in there
	m_XMLFile.AssignHomeDir();
	m_XMLFile.AppendDir(_T(".XaraLXFilters"));
	m_XMLFile.Mkdir(0777, wxPATH_MKDIR_FULL);

	m_XMLFile.SetName(m_InternalName);
	m_XMLFile.SetExt(_T("xml"));

	m_bImport = !(m_DoImport.IsEmpty());
	m_bExport = !(m_DoExport.IsEmpty());

	return(m_bImport || m_bExport);		// If it doesn't do either then it isn't a filter
}



/****************************************************************************

>	INT32 PluginOILFilter::HowCompatible(PathName& Filename)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/2005

	Inputs:		Filename	- The file to test compatability of
	Returns:	An integer from 0 to 10, 0 meaning not interested and 10 
				meaning it is definitely my format
	Purpose:	This function is called to determine if this plugin object 
				can accept the specified file.

****************************************************************************/

INT32 PluginOILFilter::HowCompatible(PathName& FileName)
{
	INT32 HowCompatible = 0;

	// Here we need to run the plugin synchronously with the following options
	// -c -f <filename>

	// Check stderr for errors
	// Get HowCompatible from stdout
	if (!m_CanImport.IsEmpty())
	{
		wxString sCommand(m_CanImport);
		sCommand.Replace(_T("%IN%"), (LPCTSTR)FileName.GetPath());

		TRACEUSER("Gerry", _T("Running '%s'"), sCommand.c_str());

		wxArrayString saOutput;
		wxArrayString saErrors;
		INT32 code = wxExecute(sCommand, saOutput, saErrors, wxEXEC_NODISABLE);
		TRACEUSER("Gerry", _T("wxExecute returned %d"), code);
		if (code == 0)
		{
			// Extract the value from saOutput
			if (saOutput.Count() > 0)
			{
				INT32 Val = wxAtoi(saOutput[0]);
				TRACEUSER("Gerry", _T("Command '%s' returned string '%s'"), sCommand.c_str(), saOutput[0].c_str());
				TRACEUSER("Gerry", _T("Value = %d"), Val);
				if (Val >= 0 && Val <= 10)
				{
					HowCompatible = Val;
				}
			}
			else
			{
				TRACEUSER("Gerry", _T("Command '%s' returned no output value"), sCommand.c_str());
			}
		}
		else
		{
			TRACEUSER("Gerry", _T("Command '%s' exited with code %d"), sCommand.c_str(), code);
		}
	}

	return(HowCompatible);
}




/****************************************************************************

>	BOOL PluginOILFilter::GetImportFile(CCLexFile* pFile, CCLexFile** ppNewFile)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/2005

	Inputs:		pFile		- pointer to a CCLexFile
				ppNewFile	- pointer to a pointer to a CCLexFile
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::GetImportFile(CCLexFile* pFile, CCLexFile** ppNewFile)
{
	ERROR2IF(pFile == NULL, FALSE,"PluginOILFilter::GetImportFile no file to import from");
	ERROR2IF(ppNewFile == NULL, FALSE,"PluginOILFilter::GetImportFile no newfile pointer");

	*ppNewFile = NULL;

	// Here we should really run the plugin asynchronously with the following options
	// -i -g -f <filename>

	// Redirect stdout to a CCLexFile
	// Check stderr during the Xar import and abort if an error is reported

	m_TempXarFile.SetPathName(_T("/tmp/xpftemp.xar"));

	CCDiskFile TempFile;
	if (!TempFile.open(m_TempXarFile, ios::out | ios::trunc | ios::binary))
	{
		// report an error here
		return FALSE;
	}

	PathName FileName = pFile->GetPathName();

	wxString sCommand(m_DoImport);
	sCommand.Replace(_T("%IN%"), (LPCTSTR)FileName.GetPath());

	TRACEUSER("Gerry", _T("Running '%s'"), sCommand.c_str());

	// Create a process with the TempFile as the stdout
	PluginFilterProcess* pTheProc = new PluginFilterProcess((PluginNativeFilter*)Parent, NULL, &TempFile);

	INT32 code = pTheProc->Execute(sCommand);
	TempFile.close();
	if (code != 0)
	{
		TRACEUSER("Gerry", _T("Execution of '%s' failed."), sCommand.c_str());
		// Extract error from a derived CamProcess class and report it
		pTheProc->ReportError();
		delete pTheProc;
		return(FALSE);
	}

	pTheProc->ReportWarning();
	delete pTheProc;
	pTheProc = NULL;

	CCDiskFile* pTempFile = new CCDiskFile();
	if (pTempFile)
	{
		if (pTempFile->open(m_TempXarFile, ios::in | ios::binary))
		{
			*ppNewFile = pTempFile;
			return(TRUE);
		}

		delete pTempFile;
	}

	return(FALSE);
}



/****************************************************************************

>	BOOL PluginOILFilter::GetExportFile(CCLexFile** ppNewFile)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Inputs:		ppNewFile	- pointer to a pointer to a CCLexFile
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::GetExportFile(PathName* pPath, CCLexFile** ppNewFile)
{
	ERROR2IF(ppNewFile == NULL, FALSE,"PluginOILFilter::GetExportFile no newfile pointer");

	*ppNewFile = NULL;

	// Here we should run the plugin asynchronously with the following options
	// -e -g -f <filename> -x <xmlfilename>

	// The xmlfilename is a path to a user and filter specific file
	// e.g. ~/.XaraLX/filtername.xml
	// Create a CCLexFile derived object that sends its data to stdin
	// Check stderr during the Xar export and abort if an error is reported

	// However, this will not be trivial so intsead we will just create a 
	// CCDiskFile attached to a temporary filename and run the export process
	// in DoExport instead

	// Generate a temporary file name
	m_TempXarFile.SetPathName(_T("/tmp/xpftemp.xar"));

	CCDiskFile* pFile = new CCDiskFile();
	if (pFile)
	{
		if (pFile->open(m_TempXarFile, ios::out | ios::binary | ios::trunc))
		{
			*ppNewFile = pFile;
			return(TRUE);
		}

		delete pFile;
	}

	return(FALSE);
}



/****************************************************************************

>	BOOL PluginOILFilter::GetCapabilities(CCLexFile* pFile, CapabilityTree* pCapTree)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Inputs:		pPlugCaps	- pointer to a PluginCapabilities
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::GetCapabilities(CCLexFile* pFile, PathName* pPath, CapabilityTree* pCapTree)
{
	// Here we need to run the plugin synchronously with the following options
	// -p -f <filename> -x <xmlfilename>

	// The xmlfilename is a path to a user and filter specific file
	// e.g. ~/.XaraLX/filtername.xml

	// The XML is returned via the file

	// Does this need double quotes to cope with spaces in filenames?
	wxString sCommand(m_PrepareExport);
	sCommand.Replace(_T("%OUT%"), (LPCTSTR)pPath->GetPath());
	sCommand.Replace(_T("%XML%"), m_XMLFile.GetFullPath());

	TRACEUSER("Gerry", _T("Running '%s'"), sCommand.c_str());

	wxArrayString saOutput;
	wxArrayString saErrors;
	INT32 code = wxExecute(sCommand, saOutput, saErrors, wxEXEC_NODISABLE);

#ifdef _DEBUG
	for (UINT32 i = 0; i < saErrors.GetCount(); i++)
	{
		TRACEUSER("Gerry", _T("stderr: %s"), saErrors[i].c_str());
	}
#endif

	if (code == 0)
	{
		BOOL bOK = BuildCapabilityTree(m_XMLFile.GetFullPath(), pCapTree);
		if (!bOK)
		{
			// Assume BuildCapabilityTree has already set an error
			return FALSE;
		}
	}
	else
	{
		TRACEUSER("Gerry", _T("Command '%s' exited with code %d"), sCommand.c_str(), code);

		// Get error message from saErrors

		// Look for the first entry that starts "ERROR:" and set the remainder of 
		// the line as the error message
		wxString line;
		size_t index = 0;
		while (index < saErrors.GetCount())
		{
			wxString rest;
			if (saErrors[index].StartsWith(_T("ERROR:"), &rest))
			{
				Error::SetError(0, rest.c_str(), 0);
				break;
			}
			index++;
		}

		if (index == saErrors.GetCount())
		{
			ERROR1(FALSE, _R(IDS_XPF_NO_ERROR_SET));
		}

		return(FALSE);
	}

	return(TRUE);
}



/****************************************************************************

>	BOOL PluginOILFilter::DoExport(CCLexFile* pXarFile, PathName* pPath)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Inputs:		pXarFile	- pointer to a CCLexFile
				pPath		- pointer to a PathName
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	

****************************************************************************/

BOOL PluginOILFilter::DoExport(CCLexFile* pXarFile, PathName* pPath)
{
	// Here we should just need to wait for the process started in GetExportFile 
	// to finish
	// Check stderr for errors and progress

	// However for now we will instead
	// Run the plugin with the following options
	// -e -g -f <filename> -x <xmlfilename>

	// The xmlfilename is a path to a user and filter specific file
	// e.g. ~/.XaraLX/filtername.xml

	// Check stderr for errors

	wxString sCommand(m_DoExport);
	sCommand.Replace(_T("%OUT%"), (LPCTSTR)pPath->GetPath());
	sCommand.Replace(_T("%XML%"), m_XMLFile.GetFullPath());

	TRACEUSER("Gerry", _T("Running '%s'"), sCommand.c_str());

	CCDiskFile TempFile(CCFILE_DEFAULTSIZE, FALSE, FALSE);
	if (!TempFile.open(m_TempXarFile, ios::in | ios::binary))
	{
		// report an error here
		return FALSE;
	}

	// Create a process with the TempFile as the stdin
	PluginFilterProcess* pTheProc = new PluginFilterProcess((PluginNativeFilter*)Parent, &TempFile, NULL);

	INT32 code = pTheProc->Execute(sCommand);
	TRACEUSER("Gerry", _T("Execute returned %d"), code);
	TempFile.close();
	if (code != 0)
	{
		TRACEUSER("Gerry", _T("Execution of '%s' failed (%d)"), sCommand.c_str(), code);
		// Extract error and report it
		pTheProc->ReportError();
		delete pTheProc;
		return(FALSE);
	}

	pTheProc->ReportWarning();
	delete pTheProc;

	return(TRUE);
}



/****************************************************************************

>	BOOL PluginOILFilter::BuildCapabilityTree(wxString strXmlFilename, CapabilityTree* pCapTree)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/2005

	Inputs:		bsXML		- The XML to load
				pCapTree	- pointer to a CapabilityTree to be filled in
	Returns:	Standard HRESULT
	Purpose:	Populates a CapabilityTree object with the XML

****************************************************************************/

BOOL PluginOILFilter::BuildCapabilityTree(wxString strXmlFilename, CapabilityTree* pCapTree)
{
	// First we need to load the XML into an XML DOM object

	// Set Parser flags here?
//	hRes = pDoc->setProperty(CComBSTR(_T("SelectionLanguage")), CComVariant(_T("XPath")));
//	hRes = pDoc->put_async(VARIANT_FALSE);
//	hRes = pDoc->put_preserveWhiteSpace(VARIANT_TRUE);
//	hRes = pDoc->put_validateOnParse(VARIANT_FALSE);
//	hRes = pDoc->put_resolveExternals(VARIANT_FALSE);

	BOOL bOK = TRUE;
    xmlDocPtr doc;

	// If string param contains xml (like original Windows version)
//	wxCharBuffer buf = strXML.mb_str(wxConvUTF8);
//	doc = xmlParseDoc((const xmlChar*)buf.data());	// buf will be deallocated when it goes out of scope

	// If string param gives xml filename (like new LX version)
	wxCharBuffer buf = strXmlFilename.ToAscii();
	doc = xmlParseFile(buf.data());					// buf will be deallocated when it goes out of scope
#if _DEBUG
	if (doc==NULL)
		doc = xmlParseFile("/tmp/XaraLX/capstest.xml");
#endif
	ERROR1IF(doc==NULL, FALSE, _R(IDE_XPF_BADXML));

	// The name of the root element should be XPFilterConfig

	xmlNodePtr node = xmlDocGetRootElement(doc);
	wxString strName = CXMLUtils::ConvertToWXString(node->name);
	if (strName!=_T("XPFilterConfig"))
		ERROR1(FALSE, _R(IDE_XPF_BADXML));

	xmlNodePtr pChild = node->children;
	INT32 Phase =0;

	// There are 7 phases to the parsing
	// We will loop round until we run out of child elements
	// After parsing a node the phase counter will be set to the phase just parsed
	// If an element should have already been parsed (using the phase counter)
	// then an error will be indicated

	while (pChild && bOK)
	{
		wxString strChildName = CXMLUtils::ConvertToWXString(pChild->name);
		
		if (strChildName == _T("#text") || xmlNodeIsText(pChild))
		{
			// ignore it
		}
		else if (strChildName == _T("Private"))
		{
			if (Phase > 0)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_PHASE0));
			}
			// Ignore the entire element
			Phase = 1;
		}
		else if (strChildName == _T("Options"))
		{
			if (Phase > 1)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_PHASE1));
			}
			bOK = ReadOptions(pChild, pCapTree);		// Read the options attributes
			Phase = 2;
		}
		else if (strChildName == _T("Rasterise"))
		{
			if (Phase > 2)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_PHASE2));
			}
			bOK = ReadRasterise(pChild, pCapTree);		// Read the dpi and alpha attributes
			Phase = 3;
		}
		else if (strChildName == _T("Spread"))
		{
			if (Phase > 3)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_PHASE3));
			}
			bOK = ReadSpread(pChild, pCapTree);		// Read the as attribute
			Phase = 4;
		}
		else if (strChildName == _T("Objects"))
		{
			if (Phase > 4)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_PHASE4));
			}
			bOK = ReadObjects(pChild, pCapTree);		// Build the tree of XPFCapability derived objects
			Phase = 5;
		}
		else if (strChildName == _T("Attributes"))
		{
			if (Phase > 5)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_PHASE5));
			}
			bOK = ReadAttributes(pChild, pCapTree);	// Build the tree of XPFCapability derived objects
			Phase = 6;
		}
		else if (strChildName == _T("Colour"))
		{
			if (Phase > 6)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_PHASE6));
			}
			bOK = ReadColour(pChild, pCapTree);		// Build the tree of XPFColour objects
			Phase = 7;
		}
		else
		{
			ERROR1(FALSE, _R(IDE_XPF_BADXML_UNEXPECTED_PHASE));
		}

		pChild = pChild->next;
	}

	xmlFreeDoc(doc);

	return bOK;
}


BOOL PluginOILFilter::ReadOptions(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	BoundsWriteLevel Level = BWL_NONE;

	wxString strLevel = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"boundslevel"));
	if (strLevel == _T("none"))
	{
		Level = BWL_NONE;
	}
	else if (strLevel == _T("compound"))
	{
		Level = BWL_COMPOUND;
	}
	else if (strLevel == _T("all"))
	{
		Level = BWL_ALL;
	}
	else if (strLevel != _T(""))
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML_OPTIONS_BOUNDSLEVEL));
	}

	pCapTree->SetBoundsLevel(Level);

	BOOL bSelection = FALSE;
	wxString strSelection = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"selection"));
	bSelection = (strSelection == _T("true"));

	pCapTree->SetSelection(bSelection);

	BOOL bPreviewBitmap = FALSE;
	wxString strPreviewBitmap = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"thumbnail"));
	bPreviewBitmap = (strPreviewBitmap == _T("true"));

	pCapTree->SetPreviewBitmap(bPreviewBitmap);

	return TRUE;
}


BOOL PluginOILFilter::ReadRasterise(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	double DPI = 96.0;
	BOOL bAlpha = TRUE;
	long Compression = 200;
	String_256 CommonTrans;
	wxString str;

	str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"dpi"));
	if (!str.IsEmpty())
	{
		str.ToDouble(&DPI);
	}

	str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"alpha"));
	if (!str.IsEmpty())
	{
		bAlpha = (str == _T("true"));
	}

	str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"compression"));
	if (!str.IsEmpty())
	{
		str.ToLong(&Compression);
	}

	str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"commontrans"));
	if (!str.IsEmpty())
	{
		CommonTrans = str;
	}

	pCapTree->SetRasterise(DPI, bAlpha, (INT32)Compression, CommonTrans);

	return TRUE;
}


BOOL PluginOILFilter::ReadSpread(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	XPFConvertType Type = XPFCONVTYPE_UNKNOWN;
	wxString str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"as"));
	if (str.IsEmpty())
	{
		Type = XPFCONVTYPE_NATIVE;
	}
	else if (str == _T("bitmap"))
	{
		Type = XPFCONVTYPE_BITMAP;
	}
	else
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML_SPREAD_CONVERTAS));
	}

	if (Type != XPFCONVTYPE_UNKNOWN)
		pCapTree->SetSpreadType(Type);

	return TRUE;
}


BOOL PluginOILFilter::ReadObjects(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	// We must loop through the tree of elements

	// pNode is the Objects element so read the default as attribute

	XPFCapability* pObjects = NULL;
	XPFConvertType ObjectsType = XPFCONVTYPE_UNKNOWN;

	BOOL bOK = GetConvertAsType(pNode, &ObjectsType);
	if (!bOK)
	{
		TRACEUSER("Phil", _T("ReadObjects GetConvertAsType failed\n"));
		return FALSE;
	}

	// Loop through each child calling the CreateObjectNode recursive function
	// for each one

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCap = CreateObjectNode(pChild);

		if (pCap)
		{
			// If we have a capability node then add it to the list
			// If we do not have a node already then set pObjects
			if (pLast)
			{
				pLast->SetNext(pCap);
			}
			else
			{
				pObjects = pCap;
			}
			pLast = pCap;
		}

		pChild = pChild->next;
	}

	pCapTree->SetObjectsTree(pObjects, ObjectsType);

	return TRUE;
}


BOOL PluginOILFilter::ReadAttributes(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	// We must loop through the tree of elements

	// pNode is the Attributes element so read the default as attribute

	XPFCapability* pAttrs = NULL;
	XPFConvertType AttrType = XPFCONVTYPE_UNKNOWN;

	BOOL bOK = GetConvertAsType(pNode, &AttrType);
	if (!bOK)
	{
		TRACEUSER("Phil", _T("ReadAttributes GetConvertAsType failed\n"));
		return FALSE;
	}

	// Loop through each child calling the CreateAttributeNode recursive function
	// for each one

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCap = CreateAttributeNode(pChild);

		if (pCap)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCap);
			}
			else
			{
				pAttrs = pCap;
			}
			pLast = pCap;
		}

		pChild = pChild->next;
	}

	pCapTree->SetAttributesTree(pAttrs, AttrType);

	return TRUE;
}


BOOL PluginOILFilter::ReadColour(xmlNodePtr pNode, CapabilityTree* pCapTree)
{
	// We must loop through the tree of elements

	// pNode is the Attributes element so read the default as attribute

	XPFCapability* pCols = NULL;
	XPFConvertType ColType = XPFCONVTYPE_UNKNOWN;

	BOOL bOK = GetConvertAsType(pNode, &ColType);
	if (!bOK)
	{
		TRACEUSER("Phil", _T("ReadColour GetConvertAsType failed\n"));
		return FALSE;
	}

	// Loop through each child calling the CreateColourNode recursive function
	// for each one

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCap = CreateColourNode(pChild);

		if (pCap)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCap);
			}
			else
			{
				pCols = pCap;
			}
			pLast = pCap;
		}

		pChild = pChild->next;
	}

	pCapTree->SetColoursTree(pCols, ColType);

	return TRUE;
}


BOOL PluginOILFilter::GetConvertAsType(xmlNodePtr pNode, XPFConvertType* pValue)
{
	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;

	if (pNode)
	{
		wxString str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)"as"));
		if (!str.IsEmpty())
		{
			if (str == _T("native"))
			{
				AsType = XPFCONVTYPE_NATIVE;
			}
			else if (str == _T("simple"))
			{
				AsType = XPFCONVTYPE_SIMPLE;
			}
			else if (str == _T("stroked"))
			{
				AsType = XPFCONVTYPE_STROKED;
			}
			else if (str == _T("bitmap"))
			{
				AsType = XPFCONVTYPE_BITMAP;
			}
			else if (str == _T("bitmapfill"))
			{
				AsType = XPFCONVTYPE_BITMAPFILL;
			}
			else if (str == _T("bitmaptrans"))
			{
				AsType = XPFCONVTYPE_BITMAPTRANS;
			}
			else if (str == _T("bitmapfilltrans"))
			{
				AsType = XPFCONVTYPE_BITMAPFILLTRANS;
			}
			else if (str == _T("bitmapspan"))
			{
				AsType = XPFCONVTYPE_BITMAPSPAN;
			}
			else if (str == _T("reformat"))
			{
				AsType = XPFCONVTYPE_REFORMAT;
			}
			else if (str == _T("remove"))
			{
				AsType = XPFCONVTYPE_REMOVE;
			}
			else if (str == _T("simplergb"))
			{
				AsType = XPFCONVTYPE_SIMPLERGB;
			}
			else if (str == _T("rgb"))
			{
				AsType = XPFCONVTYPE_RGB;
			}
			else if (str == _T("cmyk"))
			{
				AsType = XPFCONVTYPE_CMYK;
			}
			else
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_UNKNOWN_CONVTYPE));
			}
		}
	}
	else
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML_EXPECTED_CONVTYPE));
	}

	*pValue = AsType;

	return TRUE;
}


BOOL PluginOILFilter::GetXPFBOOL(xmlNodePtr pNode, LPTSTR pAttrName, XPFBOOL* pbValue)
{
	XPFBOOL bValue = XPFB_UNKNOWN;

	if (pNode)
	{
		wxString strAttrName(pAttrName);
		wxCharBuffer buf = strAttrName.mb_str(wxConvUTF8);			// buf will remain allocated in this scope
		wxString str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)buf.data()));
		if (!str.IsEmpty())
		{
			if (str == _T("true"))
			{
				bValue = XPFB_TRUE;
			}
			else if (str == _T("false"))
			{
				bValue = XPFB_FALSE;
			}
			else
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_UNEXPECTED_BOOLVALUE));
			}
		}
	}
	else
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML_NULLNODE));
	}

	*pbValue = bValue;

	return TRUE;
}


BOOL PluginOILFilter::GetXPFProp(xmlNodePtr pNode, LPTSTR pAttrName, PropMapEntry aMap[], XPFProp* pValue)
{
	XPFProp Value = XPFP_UNKNOWN;

	if (pNode)
	{
		wxString strAttrName(pAttrName);
		wxCharBuffer buf = strAttrName.mb_str(wxConvUTF8);			// buf will remain allocated in this scope
		wxString str = CXMLUtils::ConvertToWXString(xmlGetProp(pNode, (xmlChar*)buf.data()));
		if (!str.IsEmpty())
		{
			// Loop through the map until we find it or the NULL indicating the end
			INT32 Index = 0;
			while (aMap[Index].pName)
			{
				if (str == aMap[Index].pName)
				{
					Value = aMap[Index].Value;
					break;
				}
				Index++;
			}

			if (Value == XPFP_UNKNOWN)
			{
				ERROR1(FALSE, _R(IDE_XPF_BADXML_UNEXPECTED_PROPVALUE));
			}
		}
	}
	else
	{
		ERROR1(FALSE, _R(IDE_XPF_BADXML_NULLNODE));
	}

	*pValue = Value;

	return TRUE;
}


XPFCapability* PluginOILFilter::CreateObjectNode(xmlNodePtr pNode)
{
	XPFCapability* pCap = NULL;

	wxString strName = CXMLUtils::ConvertToWXString(pNode->name);
	
	if (strName == _T("#text") || xmlNodeIsText(pNode))
	{
		wxString str = CXMLUtils::ConvertToWXString(xmlNodeGetContent(pNode));
		TRACEUSER("Phil", _T("CreateObjectNode ignoring text %s\n"), (LPCTSTR)str);
		return(NULL);
	}

	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;
	BOOL bOK = GetConvertAsType(pNode, &AsType);
	if (!bOK)
	{
		TRACEUSER("Phil", _T("CreateObjectNode GetConvertAsType failed\n"));
		return NULL;
	}

	if (strName == _T("Layer"))
	{
		// Read the visible and locked attributes
		XPFBOOL bVisible = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("visible"), &bVisible);
		XPFBOOL bLocked = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("locked"), &bLocked);
		XPFBOOL bPrintable = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("printable"), &bPrintable);
		XPFBOOL bActive = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("active"), &bActive);
		XPFBOOL bBackground = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("background"), &bBackground);
		XPFBOOL bGuide = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("guide"), &bGuide);
		XPFProp ContentOnly = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("onlycontent"), aContentOnly, &ContentOnly);
		pCap = new XPFCLayer(AsType, bVisible, bLocked, bPrintable, bActive, bBackground, bGuide, ContentOnly);
	}
	else if (strName == _T("Contour"))
	{
		pCap = new XPFCContour(AsType);
	}
	else if (strName == _T("Shadow"))
	{
		// Read the type attribute
		XPFProp Type = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("type"), aShadowTypes, &Type);
		pCap = new XPFCShadow(AsType, Type);
	}
	else if (strName == _T("Bevel"))
	{
		// Read the type and side attributes
		XPFProp Type = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("type"), aBevelTypes, &Type);
		XPFProp Side = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("side"), aBevelSides, &Side);
		pCap = new XPFCBevel(AsType, Type, Side);
	}
	else if (strName == _T("Blend"))
	{
		// Read the effect, oncurve, posprofile and attrprofile attributes
		XPFProp Effect = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("effect"), aColourEffects, &Effect);
		XPFBOOL bOnCurve = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("oncurve"), &bOnCurve);
		XPFBOOL bObjProfile = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("posprofile"), &bObjProfile);
		XPFBOOL bAttrProfile = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("attrprofile"), &bAttrProfile);
		pCap = new XPFCBlend(AsType, Effect, bOnCurve, bObjProfile, bAttrProfile);
	}
	else if (strName == _T("Mould"))
	{
		// Read the as and type attributes
		XPFProp Type = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("type"), aMouldTypes, &Type);
		XPFBOOL bGradFill = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("gradfill"), &bGradFill);
		pCap = new XPFCMould(AsType, Type, bGradFill);
	}
	else if (strName == _T("Rectangle"))
	{
		// Read the as, rounded and complex attributes
		XPFBOOL bComplex = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		XPFBOOL bRounded = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("rounded"), &bRounded);
		XPFBOOL bStellated = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("stellated"), &bStellated);
		XPFBOOL bReformed = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("reformed"), &bReformed);
		pCap = new XPFCRectangle(AsType, bComplex, bRounded, bStellated, bReformed);
	}
	else if (strName == _T("Ellipse"))
	{
		// Read the as and complex attributes
		XPFBOOL bComplex = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		pCap = new XPFCEllipse(AsType, bComplex);
	}
	else if (strName == _T("Polygon"))
	{
		// Read the as, rounded, stellated and reformed attributes
		XPFBOOL bRounded = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("rounded"), &bRounded);
		XPFBOOL bStellated = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("stellated"), &bStellated);
		XPFBOOL bReformed = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("reformed"), &bReformed);
		pCap = new XPFCPolygon(AsType, bRounded, bStellated, bReformed);
	}
	else if (strName == _T("Bitmap"))
	{
		// Read the complex and contone attributes
		XPFBOOL bComplex = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		XPFBOOL bContone = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("contone"), &bContone);
		pCap = new XPFCBitmap(AsType, bComplex, bContone);
	}
	else if (strName == _T("Text"))
	{
		// Read the onpath, complex and plain attributes
		XPFBOOL bOnPath = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("onpath"), &bOnPath);
		XPFBOOL bComplex = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("complex"), &bComplex);
		XPFBOOL bPlain = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("plain"), &bPlain);
		XPFBOOL bAutoKern = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("autokern"), &bAutoKern);
		XPFBOOL bJustified = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("justified"), &bJustified);
		pCap = new XPFCText(AsType, bOnPath, bComplex, bPlain, bAutoKern, bJustified);
	}
	else if (strName == _T("ClipView"))
	{
		pCap = new XPFCClipView(AsType);
	}
	else if (strName == _T("BitmapEffect"))
	{
		pCap = new XPFCBitmapEffect(AsType);
	}
	else if (strName == _T("Feather"))
	{
		pCap = new XPFCFeather(AsType);
	}
	else
	{
		ERROR1(NULL, _R(IDE_XPF_BADXML_UNEXPECTED_OBJTYPE));
	}

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCapNode = CreateObjectNode(pChild);

		if (pCapNode)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCapNode);
			}
			else
			{
				pCap->SetChild(pCapNode);
			}
			pLast = pCapNode;
		}

		pChild = pChild->next;
	}

	return(pCap);
}


XPFCapability* PluginOILFilter::CreateAttributeNode(xmlNodePtr pNode)
{
	XPFCapability* pCap = NULL;

	wxString strName = CXMLUtils::ConvertToWXString(pNode->name);
	
	if (strName == _T("#text") || xmlNodeIsText(pNode))
	{
		wxString str = CXMLUtils::ConvertToWXString(xmlNodeGetContent(pNode));
		TRACEUSER("Phil", _T("CreateAttributeNode ignoring text %s\n"), (LPCTSTR)str);
		return(NULL);
	}

	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;
	BOOL bOK = GetConvertAsType(pNode, &AsType);
	if (!bOK)
	{
		TRACEUSER("Phil", _T("CreateAttributeNode GetConvertAsType failed\n"));
		return NULL;
	}

	if (strName == _T("Fill"))
	{
		XPFProp Shape = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("shape"), aFillShapes, &Shape);
		XPFProp Repeat = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("repeat"), aFillRepeats, &Repeat);
		XPFBOOL bMultistage = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("multistage"), &bMultistage);
		XPFProp Effect = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("effect"), aColourEffects, &Effect);
		XPFBOOL bProfile = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("profile"), &bProfile);
		XPFBOOL bContone = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("contone"), &bContone);
		pCap = new XPFCFill(AsType, Shape, Repeat, bMultistage, Effect, bProfile, bContone);
	}
	else if (strName == _T("FillTrans"))
	{
		XPFProp Shape = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("shape"), aFillShapes, &Shape);
		XPFProp Type = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("type"), aTransTypes, &Type);
		XPFProp Repeat = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("repeat"), aFillRepeats, &Repeat);
		XPFBOOL bProfile = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("profile"), &bProfile);
		pCap = new XPFCFillTrans(AsType, Shape, Type, Repeat, bProfile);
	}
	else if (strName == _T("Line"))
	{
		XPFBOOL bDash = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("dash"), &bDash);
		XPFBOOL bArrowhead = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("arrowhead"), &bArrowhead);
		XPFProp Cap = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("cap"), aLineCaps, &Cap);
		XPFProp Join = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("join"), aLineJoins, &Join);
		XPFBOOL bStroke = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("stroke"), &bStroke);
		XPFBOOL bBrush = XPFB_UNKNOWN;
		bOK = GetXPFBOOL(pNode, _T("brush"), &bBrush);
		pCap = new XPFCLine(AsType, bDash, bArrowhead, Cap, Join, bStroke, bBrush);
	}
	else if (strName == _T("LineTrans"))
	{
		XPFProp Type = XPFP_UNKNOWN;
		bOK = GetXPFProp(pNode, _T("type"), aTransTypes, &Type);
		pCap = new XPFCLineTrans(AsType, Type);
	}
	else if (strName == _T("Feather"))
	{
		pCap = new XPFCFeather(AsType);
	}
	else
	{
		ERROR1(NULL, _R(IDE_XPF_BADXML_UNEXPECTED_ATTRTYPE));
	}

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCapNode = CreateAttributeNode(pChild);

		if (pCapNode)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCapNode);
			}
			else
			{
				pCap->SetChild(pCapNode);
			}
			pLast = pCapNode;
		}

		pChild = pChild->next;
	}

	return(pCap);
}


XPFCapability* PluginOILFilter::CreateColourNode(xmlNodePtr pNode)
{
	XPFCapability* pCap = NULL;

	wxString strName = CXMLUtils::ConvertToWXString(pNode->name);
	
	if (strName == _T("#text") || xmlNodeIsText(pNode))
	{
		wxString str = CXMLUtils::ConvertToWXString(xmlNodeGetContent(pNode));
		TRACEUSER("Phil", _T("CreateColourNode ignoring text %s\n"), (LPCTSTR)str);
		return(NULL);
	}

	XPFConvertType AsType = XPFCONVTYPE_UNKNOWN;
	BOOL bOK = GetConvertAsType(pNode, &AsType);
	if (!bOK)
	{
		TRACEUSER("Phil", _T("CreateColourNode GetConvertAsType failed\n"));
		return NULL;
	}

	if (strName == _T("Colour"))
	{
		pCap = new XPFCColour(AsType);
	}
	else
	{
		ERROR1(NULL, _R(IDE_XPF_BADXML_UNEXPECTED_COLOURTYPE));
	}

	xmlNodePtr pChild;
	pChild = pNode->children;
	XPFCapability* pLast = NULL;

	while (pChild)
	{
		XPFCapability* pCapNode = CreateColourNode(pChild);

		if (pCapNode)
		{
			// If we have a node then add it to the list
			// If we do not have a node already then set m_pObjects
			if (pLast)
			{
				pLast->SetNext(pCapNode);
			}
			else
			{
				pCap->SetChild(pCapNode);
			}
			pLast = pCapNode;
		}

		pChild = pChild->next;
	}

	return(pCap);
}

/****************************************************************************

>	void PluginOILFilter::Cleanup()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/2005

	Purpose:	

****************************************************************************/

void PluginOILFilter::Cleanup()
{
}



PluginFilterProcess::PluginFilterProcess(PluginNativeFilter* pFilter, CCLexFile* pInFile, CCLexFile* pOutFile) :
	CamProcess(pInFile, pOutFile)
{
	TRACEUSER("Gerry", _T("PluginFilterProcess::PluginFilterProcess"));
	m_pFilter = pFilter;
}


PluginFilterProcess::~PluginFilterProcess()
{
	TRACEUSER("Gerry", _T("PluginFilterProcess::~PluginFilterProcess"));
}


void PluginFilterProcess::ProcessStdErr()
{
//	TRACEUSER("Gerry", _T("PluginFilterProcess::ProcessStdErr"));

	if (IsErrorAvailable())
	{
		wxTextInputStream tis(*GetErrorStream());

		// This assumes that the output is always line buffered
//		while (!GetErrorStream()->Eof())
		while (IsErrorAvailable())
		{
			wxString line;
			line << tis.ReadLine();
//			TRACEUSER("Gerry", _T("(stderr):%s"), line.c_str());

			if (!line.IsEmpty())
			{
				// If line begins "MESSAGE:" then it is a debug message and can be discarded
				wxString rest;
				if (line.StartsWith(_T("MESSAGE:"), &rest))
				{
//					TRACEUSER("Gerry", _T("XPFDebug:%s"), rest.c_str());
				}
				else if (line.StartsWith(_T("PROGRESS:"), &rest))
				{
//					TRACEUSER("Gerry", _T("XPFProgress:%s"), rest.c_str());
					if (m_pFilter)
					{
						unsigned long /*TYPENOTE: Correct*/ Val = wxStrtoul(rest.c_str(), NULL, 10);
						if (Val > 0)
						{
//							TRACEUSER("Gerry", _T("Setting progress to %d"), Val);
							m_pFilter->SetProgressBarCount((UINT32)Val);
						}
					}
				}
				else if (line.StartsWith(_T("WARNING:"), &rest))
				{
//					TRACEUSER("Gerry", _T("XPFWarning:%s"), rest.c_str());
					m_Warnings.Add(rest);
				}
				else if (line.StartsWith(_T("ERROR:"), &rest))
				{
//					TRACEUSER("Gerry", _T("XPFError:%s"), rest.c_str());
					m_Errors.Add(rest);
				}
				else
				{
//					TRACEUSER("Gerry", _T("Skipping stderr:%s"), line.c_str());
//					m_Errors.Add(line);
				}
			}
		}
	}
}


BOOL PluginFilterProcess::ReportError()
{
	TRACEUSER("Gerry", _T("PluginFilterProcess::ReportError"));

	// Get the first entry from m_ErrorList and pass it to Error::SetError
	if (m_Errors.GetCount() > 0)
	{
		Error::SetError(0, m_Errors[0].c_str(), 0);
	}
	else
	{
		ERROR1(FALSE, _R(IDS_XPF_NO_ERROR_SET));
	}

	return(FALSE);
}


void PluginFilterProcess::ReportWarning()
{
	TRACEUSER("Gerry", _T("PluginFilterProcess::ReportWarning"));

	// Show the user the contents of m_WarningList
	for (size_t i = 0; i < m_Warnings.GetCount(); i++)
	{
		TRACEUSER("Gerry", _T("WARNING:%s"), m_Warnings[i].c_str());
	}
}
