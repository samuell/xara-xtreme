// $Id: xmlutils.cpp 836 2006-04-18 16:06:15Z gerry $
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

#include "xmlutils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
//#include <libxml/xpath.h>

/************************************************************************************
> 	BOOL CXMLUtils::Initialise()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	08/May/2006
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if succeeded
    Purpose:	Set up application wide XML support

************************************************************************************/
BOOL CXMLUtils::Initialise()
{
	xmlInitParser();
    LIBXML_TEST_VERSION

	return TRUE;
}




/************************************************************************************
> 	BOOL CXMLUtils::DeInitialise()

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	08/May/2006
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if succeeded
    Purpose:	Set up application wide XML support

************************************************************************************/
BOOL CXMLUtils::DeInitialise()
{
	xmlCleanupParser();

#if _DEBUG
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
#endif

	return TRUE;
}




/************************************************************************************
>	static wxString CXMLUtils::ConvertToWXString(xmlChar* xmlString)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	08/May/2006
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to newly allocated StringBase object (caller takes ownership)
    Purpose:	Set up application wide XML support

************************************************************************************/
wxString CXMLUtils::ConvertToWXString(const xmlChar* xmlString)
{
	// Use wx to convert from UTF8 to native
	wxString strTemp((const char*)xmlString, wxConvUTF8);

	return strTemp;
}




/************************************************************************************
>	static void CXMLUtils::ConvertToXMLString(wxString str)

    Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	09/May/2006
	Inputs:		str - wxString input
	Outputs:	pxmlstr - xmlChar* output
	Returns:	Pointer to newly allocated StringBase object (caller takes ownership)
    Purpose:	Set up application wide XML support

************************************************************************************/
void CXMLUtils::ConvertToXMLString(wxString str, xmlChar** pxmlstr)
{
//	wxCharBuffer buf = str.mb_str(wxConvUTF8);
//	wxStrdup from buf to pxmlstr?
}




#if _DEBUG
#if defined(LIBXML_XPATH_ENABLED) && \
	defined(LIBXML_SAX1_ENABLED) && \
    defined(LIBXML_OUTPUT_ENABLED)

/*
 * section: 	XPath
 * synopsis: 	Load a document, locate subelements with XPath, modify
 *              said elements and save the resulting document.
 * purpose: 	Shows how to make a full round-trip from a load/edit/save
 * usage:	xpath2 <xml-file> <xpath-expr> <new-value>
 * test:	xpath2 test3.xml '//discarded' discarded > xpath2.tmp ; diff xpath2.tmp xpath2.res ; rm xpath2.tmp
 * author: 	Aleksey Sanin and Daniel Veillard
 * copy: 	see Copyright for the status of this software.
 */
void CXMLUtils::Test()
{
    /* Init libxml */
    xmlInitParser();
    LIBXML_TEST_VERSION

    /* Do the main job */
    example4("/tmp/testin.xml", (xmlChar*)"//fred", (xmlChar*)"jim");

    /* Shutdown libxml */
    xmlCleanupParser();
    
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
}

/**
 * example4:
 * @filename:		the input XML filename.
 * @xpathExpr:		the xpath expression for evaluation.
 * @value:		the new node content.
 *
 * Parses input XML file, evaluates XPath expression and update the nodes
 * then print the result.
 *
 * Returns 0 on success and a negative value otherwise.
 */
int CXMLUtils::example4(const char* filename, const xmlChar* xpathExpr, const xmlChar* value)
{
    xmlDocPtr doc;
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj; 
    
    assert(filename);
    assert(xpathExpr);
    assert(value);

    /* Load XML document */
    doc = xmlParseFile(filename);
    if (doc == NULL) {
	fprintf(stderr, "Error: unable to parse file \"%s\"\n", filename);
	return(-1);
    }

    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        fprintf(stderr,"Error: unable to create new XPath context\n");
        xmlFreeDoc(doc); 
        return(-1);
    }
    
    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if(xpathObj == NULL) {
        fprintf(stderr,"Error: unable to evaluate xpath expression \"%s\"\n", xpathExpr);
        xmlXPathFreeContext(xpathCtx); 
        xmlFreeDoc(doc); 
        return(-1);
    }

    /* update selected nodes */
    update_xpath_nodes(xpathObj->nodesetval, value);

    
    /* Cleanup of XPath data */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx); 

    /* dump the resulting document */
    xmlDocDump(stdout, doc);

	/* save the doc out to a temp file */
	xmlSaveFileEnc("/tmp/testout.xml", doc, "UTF-8");

    /* free the document */
    xmlFreeDoc(doc); 
    
    return(0);
}

/**
 * update_xpath_nodes:
 * @nodes:		the nodes set.
 * @value:		the new value for the node(s)
 *
 * Prints the @nodes content to @output.
 */
void CXMLUtils::update_xpath_nodes(xmlNodeSetPtr nodes, const xmlChar* value)
{
    int size;
    int i;
    
    assert(value);
    size = (nodes) ? nodes->nodeNr : 0;
    
    /*
     * NOTE: the nodes are processed in reverse order, i.e. reverse document
     *       order because xmlNodeSetContent can actually free up descendant
     *       of the node and such nodes may have been selected too ! Handling
     *       in reverse order ensure that descendant are accessed first, before
     *       they get removed. Mixing XPath and modifications on a tree must be
     *       done carefully !
     */
    for(i = size - 1; i >= 0; i--) {
	assert(nodes->nodeTab[i]);
	
	xmlNodeSetContent(nodes->nodeTab[i], value);
	/*
	 * All the elements returned by an XPath query are pointers to
	 * elements from the tree *except* namespace nodes where the XPath
	 * semantic is different from the implementation in libxml2 tree.
	 * As a result when a returned node set is freed when
	 * xmlXPathFreeObject() is called, that routine must check the
	 * element type. But node from the returned set may have been removed
	 * by xmlNodeSetContent() resulting in access to freed data.
	 * This can be exercised by running
	 *       valgrind xpath2 test3.xml '//discarded' discarded
	 * There is 2 ways around it:
	 *   - make a copy of the pointers to the nodes from the result set 
	 *     then call xmlXPathFreeObject() and then modify the nodes
	 * or
	 *   - remove the reference to the modified nodes from the node set
	 *     as they are processed, if they are not namespace nodes.
	 */
	if (nodes->nodeTab[i]->type != XML_NAMESPACE_DECL)
	    nodes->nodeTab[i] = NULL;
    }
}

#else
	#pragma error "XPath support not compiled in"
#endif

#endif

