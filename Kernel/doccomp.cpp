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

// Implementation of the Document Component objects.

/*
*/

#include "camtypes.h"

#include "doccomp.h"


DECLARE_SOURCE("$Revision$");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


CC_IMPLEMENT_DYNAMIC(DocComponentClass, ListItem)
CC_IMPLEMENT_DYNAMIC(DocComponent, 		ListItem)


/********************************************************************************************

>	BOOL DocComponentClass::AddComponent(Document *pDocument)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/94
	Inputs:		pDocument - the document object to add the document component to.
	Returns:	TRUE if the component was added without problems;
				FALSE if not.
	Purpose:	Add an instance of a document component to the specified document.
				This function should create a default instance of the relevant 
				DocComponent object for this class, and add it to the document.
				The function should check to see if one already exists in this document,
				and either replace it, or leave it alone, depending on the nature of the
				component.

				NB. This is a pure virtual function in the base class - you must override it.

	Errors:		Out of memory.
	SeeAlso:	DocComponentClass

********************************************************************************************/

/********************************************************************************************

>	BOOL DocComponent::EPSStartImport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the document component that a Native or Web import is about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocComponent::StartImport(BaseCamelotFilter *pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	BOOL DocComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the document component that a Native or Web import has just finished.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
	return TRUE;
}

/********************************************************************************************

>	BOOL DocComponent::StartExportBeforeComp(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Purpose:	Inform the document component that a Web or Native export is about to start.
				
				This is called before zlib compression is turned on, to give the doc component
				a chance to write out uncompressed records.

	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocComponent::StartExportBeforeComp(BaseCamelotFilter *pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	BOOL DocComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Purpose:	Inform the document component that a WEb or Native export is about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocComponent::StartExport(BaseCamelotFilter *pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	BOOL DocComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - True if everything went swimmingly, False if just a clean up is required.

	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)

	Purpose:	Inform the document component that a Web or Native export is just finishing.
				This gives the component a chance to write out any pending data, such as
				named colours that weren't referenced in the document, etc.

	Notes:		This saves any pending data (e.g. unreferenced Named colours) but DOES NOT
				DELETE ANY DATA STRUCTURES - these should be deleted in the CleanUpAfterExport
				function. This allows situations like: PrintComponent wants to save a colour
				reference in it's Endxport call, but is called after the ColourComponent
				EndExport. The ColourComponent EndExport does not delete its data structures,
				so is still able to supply colour references to other components' EndExport
				functions.

	SeeAlso:	DocComponent::StartExport; DocComponent::CleanUpAfterExport

********************************************************************************************/

BOOL DocComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
	return TRUE;
}



/********************************************************************************************

>	virtual void DocComponent::CleanUpAfterExport(BOOL Success)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/8/96
	Inputs:		Success - True if everything went swimmingly, False if the export failed

	Purpose:	Inform the document component that a Web or Native export has just finished.

	Notes:		This is called after EndExport, so that the componenbt can clean up any 
				during-export data structures. This allows situations like: PrintComponent
				wants to save a colour reference in it's Endxport call, but is called after
				the ColourComponent EndExport. The ColourComponent EndExport does not delete
				its data structures, so is still able to supply colour references to other
				components' EndExport functions.

				The base class implementation does nothing

	SeeAlso:	DocComponent::EndExport

********************************************************************************************/

void DocComponent::CleanUpAfterExport(BOOL Success)
{
	// Base class does nothing
}



/********************************************************************************************

>	BOOL DocComponent::EPSStartImport(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the document component that an EPS import is about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocComponent::EPSStartImport(EPSFilter *pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	void DocComponent::EPSEndImport(EPSFilter *pFilter, BOOL Success)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Purpose:	Inform the document component that an EPS import has just finished.
	SeeAlso:	DocComponent

********************************************************************************************/

void DocComponent::EPSEndImport(EPSFilter *pFilter, BOOL Success)
{
}

/********************************************************************************************

>	BOOL DocComponent::EPSStartExport(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Purpose:	Inform the document component that an EPS export is about to start.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL DocComponent::EPSStartExport(EPSFilter *pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	void DocComponent::EPSEndExport(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Purpose:	Inform the document component that an EPS export has just finished.
	SeeAlso:	DocComponent

********************************************************************************************/

void DocComponent::EPSEndExport(EPSFilter *pFilter)
{
}


/********************************************************************************************

>	BOOL DocComponent::WriteEPSProlog(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/94
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the Prolog for this component was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	Write out the EPS prolog for this document component, using the EPS
				filter object supplied. 
				(The base class does nothing)

	SeeAlso:	DocComponent::WriteEPSSetup; 
				DocComponent::WriteEPSComments;
				DocComponent::ProcessEPSComment; 
				DocComponent; 
				EPSFilter

********************************************************************************************/

BOOL DocComponent::WriteEPSProlog(EPSFilter *pFilter)
{
	return TRUE;
}


/********************************************************************************************

>	BOOL DocComponent::WriteEPSSetup(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/94
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the Prolog for this component was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	Write out the EPS setup for this document component, using the EPS
				filter object supplied.
				(The base class does nothing)

	SeeAlso:	DocComponent::WriteEPSProlog; 
				DocComponent::WriteEPSComments;
				DocComponent::ProcessEPSComment; 
				DocComponent; 
				EPSFilter

********************************************************************************************/

BOOL DocComponent::WriteEPSSetup(EPSFilter *pFilter)
{
	return TRUE;
}



/********************************************************************************************

>	BOOL DocComponent::WriteEPSComments(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/94
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Outputs:	-
	Returns:	TRUE if the Prolog for this component was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	Write out any comments that need to be in the initial batch of EPS
				comments (i.e. before the %%EndComments line), using the specified EPS
				Filter object.
				(The base class does nothing)

	SeeAlso:	DocComponent::WriteEPSSetup; 
				DocComponent::WriteEPSProlog;
				DocComponent::ProcessEPSComment; 
				DocComponent; 
				EPSFIlter

********************************************************************************************/

BOOL DocComponent::WriteEPSComments(EPSFilter *pFilter)
{
	return TRUE;
}



/********************************************************************************************

>	BOOL DocComponent::WriteScript(EPSFilter* pFilter)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95
	Inputs:		pFilter - the EPS filter that is being used to export a file.
	Returns:	TRUE if the Script for this component was written out successfully;
				FALSE if not (e.g. out of disk space etc)
	Purpose:	This is called after all the Comments, Prolog etc have been output so that
				DocComponents get a chance to do something right at the start of the real
				EPS data, but before the tree starts to be exported.
				(The base class does nothing)
	SeeAlso:	DocComponent::WriteEPSSetup; 
				DocComponent::WriteEPSProlog;
				DocComponent::ProcessEPSComment; 
				DocComponent; 
				EPSFIlter

********************************************************************************************/

BOOL DocComponent::WriteScript(EPSFilter* pFilter)
{
	return TRUE;
}



/********************************************************************************************

>	ProcessEPSResult DocComponent::ProcessEPSComment(EPSFilter *pFilter, 
													 const char *pComment)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				pComment - read only buffer containing the whole EPS comment to parse.
	Returns:	EPSCommentUnknown	  - This EPS comment is not recognised by the document
									    component.
				EPSCommentError		  - This EPS comment was recognised by this document
									    component, but it contained an error.
				EPSCommentSystemError - This EPS comment was recognised by this document
									    component, but an error occured that was not caused
										directly by the comment, e.g. out of memory.
				EPSCommentOK		  - This EPS comment was recognised as a legal comment by 
									    this document component, and was processed
									    successfully.
	Purpose:	Process an EPS comment found in the file, if it 'belongs' to this
				document component.  If it does not, then the document component should 
				return EPSCommentUnknown, and the comment will be passed on to the next 
				document component.
				If the function returns EPSCommentOK, and the next line in the file starts 
				with "%%+", i.e. an EPS/PostScript continuation comment, then this will be
				passed to this document component immediately, without trying any of the
				other document components first.  This allows items such a colour lists
				to be imported easily, as they are usually specified on multiple lines in
				the file, using "%%+" comments.
				(The base class does nothing - it returns EPSCommentUnknown)

	SeeAlso:	DocComponent::EndOfEPSComment; 
				DocComponent::WriteEPSProlog; 
				DocComponent::WriteEPSSetup; 
				DocComponent::WriteEPSComments; 
				DocComponent; 
				ProcessEPSResult;
				EPSFIlter

********************************************************************************************/

ProcessEPSResult DocComponent::ProcessEPSComment(EPSFilter *pFilter, 
												 const char *pComment)
{
	return EPSCommentUnknown;
}


/********************************************************************************************

>	void DocComponent::EndOfEPSComment(EPSFilter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Inputs:		pFilter - the EPS filter that is being used to import a file.
	Purpose:	Informs the document component that the comment it has been decoding has
				now finished, and it should take whatever action required to act on
				the comment.
				This call is made even for single-line comments, because by the nature of
				DSC comments, it is not known until the next comment starts or a
				non-comment line is encountered that the comment is over and is not
				multi-line.

	SeeAlso:	DocComponent::ProcessEPSComment; 
				DocComponent::WriteEPSProlog; 
				DocComponent::WriteEPSSetup; 
				DocComponent::WriteEPSComments;
				DocComponent; EPSFilter

********************************************************************************************/

void DocComponent::EndOfEPSComment(EPSFilter *pFilter)
{
}


/********************************************************************************************

>	INT32 DocComponent::GetSizeOfExport(Filter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		pFilter - the export filter that will be used.
	Returns:	An approximation of the size of data that will be exported by this
				document component, in terms of nodes. (see NodeRenderable::GetSizeOfExport).
	Purpose:	Find out how many nodes will be exported when this document component
				is exported.
				The node can use the filter to find out how it will be exported, e.g.
				bitmap lists should return a value that reflects whether or not bitmap
				indirection is possible with this filter and so on.
				NB. This is virtual - the default implementation just returns 1 - only
					override if this is not accurate.
	SeeAlso:	Node::NeedsToExport; Node::ExportRender; NodeRenderable::GetSizeOfExport

********************************************************************************************/

INT32 DocComponent::GetSizeOfExport(Filter*)
{
	// Default value is one node...
	return 1;
}




/********************************************************************************************

>	virtual BOOL DocComponent::StartComponentCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if we run out of memory
	Purpose:	This function is called prior to nodes being copied from some Source document to
				the document where this DocComponent lives. It gives the DocComponent a chance
				to make preperations for caching any possible updates.
	Errors:		-
	SeeAlso:	NodeRenderable::CopyComponentData
	SeeAlso:	DocComponent::EndComponentCopy
	SeeAlso:	DocComponent::AbortComponentCopy

********************************************************************************************/

BOOL DocComponent::StartComponentCopy() { return TRUE;}

/********************************************************************************************

>	virtual BOOL DocComponent::EndComponentCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function is called after a successful copy from another document. If 
				changes have been made to the components data then now is the time to
				commit them. 
	Errors:		-
	SeeAlso:	NodeRenderable::CopyComponentData
	SeeAlso:	DocComponent::StartComponentCopy
	SeeAlso:	DocComponent::AbortComponentCopy

********************************************************************************************/


BOOL DocComponent::EndComponentCopy() { return TRUE; }  


/********************************************************************************************

>	virtual void DocComponent::AbortComponentCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function is called if a node copy has failed. If any changes have been 
				made then they may need to be vaped at this point. Note this function could get
				called without a prior StartComponentCopy call.	Also it could get called more
				than once.
	Errors:		-
	SeeAlso:	NodeRenderable::CopyComponentData
	SeeAlso:	DocComponent::EndComponentCopy
	SeeAlso:	DocComponent::StartComponentCopy

********************************************************************************************/

void DocComponent::AbortComponentCopy() { }


