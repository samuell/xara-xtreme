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
// Author  : Mike
// Created : 31/07/96
// Purpose : The PrintMarks Doc Component and all its helper classes.

/*
*/

#include "camtypes.h"

#include "app.h"
#include "camfiltr.h"
#include "cxfrec.h"
#include "cxfrech.h"
#include "cxftags.h"
#include "layer.h"
#include "group.h"
#include "prnmkcom.h"
//#include "prnmks.h"

CC_IMPLEMENT_DYNAMIC(PrintMarksComponent, DocComponent)
CC_IMPLEMENT_DYNAMIC(PrintMarkComponentClass, DocComponentClass)
CC_IMPLEMENT_DYNAMIC(DocPrintMarkList, List)
CC_IMPLEMENT_DYNCREATE(DocPrintMark, ListItem)

DECLARE_SOURCE("$Revision: 662 $");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	DocPrintMark::DocPrintMark()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:
	SeeAlso:

********************************************************************************************/

DocPrintMark::DocPrintMark()
{
	Handle=0;
}

DocPrintMark::DocPrintMark(UINT32 hndle)
{
	Handle=hndle;
}

/********************************************************************************************

>	DocPrintMark::~DocPrintMark()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:
	SeeAlso:

********************************************************************************************/

DocPrintMark::~DocPrintMark()
{
}


/********************************************************************************************

>	DocPrintMarkList::DocPrintMarkList()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:
	SeeAlso:

********************************************************************************************/

DocPrintMarkList::DocPrintMarkList()
{
	Virgin=TRUE;
}


/********************************************************************************************

>	DocPrintMarkList::~DocPrintMarkList()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:
	SeeAlso:

********************************************************************************************/

DocPrintMarkList::~DocPrintMarkList()
{
	RemoveAllMarks();
}

BOOL DocPrintMarkList::AddMark(UINT32 handle)
{
	DocPrintMark* pMark = new DocPrintMark(handle);
	if (pMark)
	{
		AddTail(pMark);
		Virgin=FALSE;
	}
	return (pMark!=NULL);
}

void DocPrintMarkList::RemoveMark(UINT32 handle)
{
	DocPrintMark* pMark = FindMark(handle);
	if (pMark)
	{
		RemoveItem(pMark);
		delete pMark;
	}
	else
	{
		TRACEUSER( "Mike", _T("Unable to find mark during DocPrintMarkList::RemoveMark()"));
	}
}

void DocPrintMarkList::RemoveAllMarks()
{
	DocPrintMark* pItem;
	while ((pItem=((DocPrintMark*)RemoveTail()))!=NULL)
		delete pItem;
}

/*****************************************************************************************

>	DocPrintMark* DocPrintMarkList::FindMark(UINT32 SearchHandle) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Inputs:		SearchHandle = the handle of the mark to search for
	Returns:	A pointer to the print mark described by this handle (NULL if none)
	Purpose:	Try to find a print mark in the doc component, using its handle

*****************************************************************************************/

DocPrintMark* DocPrintMarkList::FindMark(UINT32 SearchHandle) const
{
	DocPrintMark* pItem = GetFirstMark();
	while (pItem)
	{
		if (pItem->GetHandle() == SearchHandle)
			return pItem;
		pItem = GetNextMark(pItem);
	}
	return NULL;
}


/********************************************************************************************

>	BOOL PrintMarkComponentClass::Init()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Register the print marks document component with the main application.
	Errors:		Out of memory.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL PrintMarkComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	PrintMarkComponentClass *pClass = new PrintMarkComponentClass;
	if (pClass == NULL)
		return FALSE;

	// Register it
	GetApplication()->RegisterDocComponent(pClass);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL PrintMarkComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Inputs:		pDocument - the document to add the print to.
	Returns:	TRUE if the printmark component was added ok;
				FALSE if not.
	Purpose:	Add a printmark component to the specified document.
	Errors:		Out of memory
	SeeAlso:	PrintComponentClass

********************************************************************************************/

BOOL PrintMarkComponentClass::AddComponent(BaseDocument *pDocument)
{
	ERROR2IF(pDocument==NULL,FALSE,"NULL document passed to PrintMarkCompClass:Add");
	
	// Check to see if this document already has a colour list; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintMarksComponent)) != NULL)
		return TRUE;

	// Ok - create the print mark component.
	PrintMarksComponent *pComponent = new PrintMarksComponent();
	if (pComponent==NULL)
		return FALSE;

	// All ok - add the component to the document.
	pDocument->AddDocComponent(pComponent);
	return TRUE;
}



/********************************************************************************************

>	PrintMarksComponent::PrintMarksComponent()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:
	SeeAlso:

********************************************************************************************/

PrintMarksComponent::PrintMarksComponent()
{
	CustomPreviousContext = NULL;
	CustomCurrentContext  = NULL;
	pNewMark = NULL;
}

/********************************************************************************************

>	PrintMarksComponent::~PrintMarksComponent()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:
	SeeAlso:

********************************************************************************************/

PrintMarksComponent::~PrintMarksComponent()
{
}



/********************************************************************************************

>	BOOL PrintMarksComponent::AddMark(UINT32 handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:	Adds a print mark reference to our component list
	SeeAlso:

********************************************************************************************/

BOOL PrintMarksComponent::AddMark(UINT32 handle)
{
	// it may well be there already!
	if (DocMarks.FindMark(handle))
		return TRUE;
	return DocMarks.AddMark(handle);		
}



/********************************************************************************************

>	void PrintMarksComponent::RemoveMark(UINT32 handle)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:	Remove a print marks component from the list.
	SeeAlso:

********************************************************************************************/

void PrintMarksComponent::RemoveMark(UINT32 handle)
{
	DocMarks.RemoveMark(handle);
}


/********************************************************************************************

>	void PrintMarksComponent::RemoveAllMarks()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:	Empty the list of all marks
	SeeAlso:

********************************************************************************************/

void PrintMarksComponent::RemoveAllMarks()
{
	DocMarks.RemoveAllMarks();
}



/********************************************************************************************

>	DocPrintMark* PrintMarksComponent::FindMark(UINT32 handle) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Inputs:		handle = the handle of the mark to search for
	Returns:	A pointer to the print mark described by this handle (NULL if none)
	Purpose:	Try to find a print mark in the doc component, using the print mark handle
	SeeAlso:

********************************************************************************************/

DocPrintMark* PrintMarksComponent::FindMark(UINT32 handle) const
{
	return DocMarks.FindMark(handle);
}


/********************************************************************************************

>	BOOL PrintMarksComponent::IsVirgin() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Returns:	TRUE - if no print mark has ever been added to this print mark component
				FALSE - if some have in past history
	Purpose:	Is this a virgin print marks component, ie has it never had a print mark
				added to it?

********************************************************************************************/

BOOL PrintMarksComponent::IsVirgin() const
{
	return DocMarks.IsVirgin();
}


/********************************************************************************************

>	BOOL PrintMarksComponent::StartImport(BaseCamelotFilter *pFilter)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)
	Purpose:
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL PrintMarksComponent::StartImport(BaseCamelotFilter *pFilter)
{
	if (pFilter == NULL)
	{
		ERROR3("PrintMarksComponent::StartImport filter is null!");
		return TRUE;
	}

	CustomPreviousContext = NULL;
	CustomCurrentContext  = NULL;
	pNewMark = NULL;

	// If we're importing into an existing document, we ignore all printer mark
	// information in the file being imported.
	if (pFilter->IsImporting())
		return(TRUE);

	// Before importing, we delete all existing marks, so that the loaded file overrides
	// the settings already present in the document.
	RemoveAllMarks();

	return TRUE;
}


/********************************************************************************************

>	BOOL PrintMarksComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	Purpose:
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL PrintMarksComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
/*
	if (pFilter==NULL)
	{
		ERROR3("PrintMarksComponent::EndImport filter is null!");
		return TRUE;
	}

	if (Success)
	{

	}
	else
	{

	}
*/
	CustomPreviousContext = NULL;
	CustomCurrentContext  = NULL;
	pNewMark = NULL;

	return TRUE;
}



/********************************************************************************************

>	BOOL PrintMarksComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Purpose:
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL PrintMarksComponent::StartExport(BaseCamelotFilter *pFilter)
{
#if !defined(EXCLUDE_FROM_RALPH)
/*
	if (pFilter == NULL)
	{
		ERROR3("PrintMarksComponent::StartExport filter is null!");
		return TRUE;
	}

*/
#endif
	return TRUE;
}



/********************************************************************************************

>	BOOL PrintMarksComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - True if everything went swimmingly, False if just a clean up is required.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	Purpose:
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL PrintMarksComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
	BOOL ok = TRUE;

// WEBSTER - markn 14/2/97
// No print records needed in Webster
#ifndef WEBSTER
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (pFilter == NULL)
	{
		ERROR3("PrintMarksComponent::EndExport filter is null!");
		return(ok);
	}

	if (!Success || pFilter->IsWebFilter())	// Don't export to web files, or if we have failed
		return(ok);


	// Find the Print mark manager and it's PrintMark cache
	PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan == NULL)
		return(ok);

	// Export the list of marks
	DocPrintMark *pItem = DocMarks.GetFirstMark();
	while (ok && pItem != NULL)
	{
		// From our item, find the actual PrintMark definition in the PMM cache
		PrintMarkItem *pMarkItem = pMarksMan->PMMCache.FindMark(pItem->GetHandle());
		if (pMarkItem != NULL)
			ok = ExportPrintMark(pFilter, pMarkItem);

		pItem = DocMarks.GetNextMark(pItem);
	}
#endif
#endif // WEBSTER
	return ok;
}



/********************************************************************************************

>	BOOL PrintMarksComponent::ExportPrintMark(BaseCamelotFilter *pFilter, PrintMarkItem *pMarkItem)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/96

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
				pMarkItem - The print mark to be exported

	Returns:	TRUE if export was successful

	Purpose:	To export a print mark

	Notes:		Print marks, if disabled, will not be exported. i.e. only needed data
				is actually saved to the file.

				Print marks are saved in 2 formats:
					TAG_PRINTMARKDEFAULT - Default (built in) marks - a 1-byte record
					indicates which mark to use by quoting its ID byte.

					TAG_PRINTMARKCUSTOM - User (custom) marks - The record is zero length,
					but is followed by a TAG_DOWN, a clipart tree, and a TAG_UP.

					NOTE that custom marks are currently not handled.

********************************************************************************************/

BOOL PrintMarksComponent::ExportPrintMark(BaseCamelotFilter *pFilter, PrintMarkItem *pMarkItem)
{
	BOOL ok = TRUE;

	PORTNOTETRACE("print","PrintMarksComponent::ExportPrintMark - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(pFilter == NULL || pMarkItem == NULL, "Illegal NULL params");

	PrintMark *pMarkInfo = pMarkItem->GetPrintMark();
	if (pMarkInfo == NULL)
	{
		ERROR3("PrintMarkItem with no PrintMark");
		return(FALSE);
	}

	if (pMarkInfo->IsCustom())
	{
		// A custom mark consists of a PrintMarkCustom record, defining where the
		// mark goes, what it's called, etc, followed records for the clipart subtree
		CXaraFileRecord Rec(TAG_PRINTMARKCUSTOM, TAG_PRINTMARKCUSTOM_SIZE);
		if (ok) ok = Rec.Init();

		// Write out the print mark info
		//	Type (bitmap ID), menu text string, orientation, positions
		if (ok)  ok = Rec.WriteBYTE((BYTE) pMarkInfo->GetType());
		if (ok)  ok = Rec.WriteBYTE((BYTE) pMarkInfo->GetOrientation());
		
		if (ok)
		{
			MarkPosition *Pos = pMarkInfo->GetFirstPosition();
			while (ok && Pos != NULL)
			{
				ok = Rec.WriteBYTE((BYTE) Pos->GetRegion());					// Write the region
				if (ok) ok = Rec.WriteBYTE(Pos->GetFormat().GetAsFlagByte());	// Write the format

				Pos = pMarkInfo->GetNextPosition(Pos);
			}

			if (ok) ok = Rec.WriteBYTE(MarkRegion_FileFormatTerminator);		// Terminate the list
		}

		if (ok)  ok = Rec.WriteUnicode(pMarkInfo->GetMarkMenuText());			// Write the mark name

		// Finish off this record
		if (ok)	pFilter->Write(&Rec);


		// OK, now write the print mark "clipart" subtree to the file...
		if (ok) ok = pFilter->WriteZeroSizedRecord(TAG_DOWN);
		
		// BLOCK
		{
			// Find the Glyph subtree. We could strip off the default attributes
			// that Mike keeps around, but they only add a tiny amount (100 bytes
			// or so) to the file, and save a lot of problems getting attribute-completeness
			// when reloading the mark.
			Node *GlyphRoot = pMarkItem->GetMarkGlyph();
			if (GlyphRoot == NULL)
			{
				ERROR3("Custom printers mark with no glyph");
				ok = FALSE;
			}

			if (ok) ok = pFilter->WriteNodes(GlyphRoot);
		}
		if (ok) ok = pFilter->WriteZeroSizedRecord(TAG_UP);
	}
	else
	{
		// --- Write the record
		CXaraFileRecord Rec(TAG_PRINTMARKDEFAULT, TAG_PRINTMARKDEFAULT_SIZE);
		if (ok) ok = Rec.Init();

		if (ok) ok = Rec.WriteBYTE(pMarkInfo->GetIDByte());

		// --- Finally, output the record
		if (ok)
			pFilter->Write(&Rec);
	}
#endif
	return(ok);
}



/********************************************************************************************

>	void PrintMarksComponent::ImportDefaultPrintMark(CXaraFileRecord* Rec)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96

	Inputs:		Rec - The TAG_PRINTMARKDEFAULT print mark record to be imported

	Purpose:	To import a default print mark

********************************************************************************************/

void PrintMarksComponent::ImportDefaultPrintMark(CXaraFileRecord* Rec)
{
	PORTNOTETRACE("print","PrintMarksComponent::ExportPrintMark - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(Rec == NULL, "Illegal NULL params");
	BYTE ID = 0;
	BOOL ok = Rec->ReadBYTE(&ID);

	if (!ok)
		return;

	// Find the Print mark manager and it's PrintMark cache
	PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan == NULL)
		return;

	// Search the PMMCache list for a PrintMark with the given ID byte
	UINT32 Handle = 0;
	BOOL Found = FALSE;
	PrintMarkItem *pItem = pMarksMan->PMMCache.GetFirstMark();
	while (pItem != NULL)
	{
		PrintMark *pMark = pItem->GetPrintMark();
		if (pMark != NULL && pMark->GetIDByte() == ID)
		{
			Handle = pItem->GetHandle();
			Found  = TRUE;
			break;
		}

		pItem = pMarksMan->PMMCache.GetNextMark(pItem);
	}

	// If we found a mark, then Handle is its PMM handle - Add an entry to our
	// list of Enabled marks.
	if (Found)
		AddMark(Handle);
#endif
}



/********************************************************************************************

>	void PrintMarksComponent::StartImportCustomPrintMark(CamelotRecordHandler *pHandler,
															CXaraFileRecord* Rec)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96

	Inputs:		pHandler - the record handler that called us
				Rec - The TAG_PRINTMARKCUSTOM print mark record to be imported
	
	Purpose:	To start import of a custom print mark
				This is called when a TAG_PRINTMARKCUSTOM is found in the importing file.
				The tag is followed by a subtree (beginning with a NodeGroup).

				We create a blank layer node, and point the importer at it, so that the
				subtree imports where _we_ want it. When the final TAG_UP record of the
				subtree is read, we will be clled back in EndImportCustomPrintMark to
				complete the job.

	SeeAlso:	PrintMarksComponent::EndImportCustomPrintMark

********************************************************************************************/

void PrintMarksComponent::StartImportCustomPrintMark(CamelotRecordHandler *pHandler, CXaraFileRecord* Rec)
{
	PORTNOTETRACE("print","PrintMarksComponent::StartImportCustomPrintMark - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(CustomCurrentContext != NULL || CustomPreviousContext != NULL ||
				pNewMark != NULL, "StartImportCustomPrintMark called while importing another custom mark?!");

	// Find the Print mark manager and it's PrintMark cache
	PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan == NULL)
		return;

	// Create a PrintMark encapsulating all the data in this record
	pNewMark = new PrintMarkItem;
	if (!pNewMark)
		return;

	PrintMark *pMarkInfo = pNewMark->GetPrintMark();
	pMarkInfo->SetCustomOrDefault(TRUE);								// This is a custom mark


	// --- Now read the print mark info
	BOOL ok = TRUE;
	BYTE Temp;

	if (ok)  ok = Rec->ReadBYTE(&Temp);
	if (ok)  pMarkInfo->SetType((MarkType) Temp);

	if (ok)  ok = Rec->ReadBYTE(&Temp);
	if (ok)  pMarkInfo->SetOrientation((MarkOrient) Temp);
	
	while (ok)															// Read all format positions
	{
		ok = Rec->ReadBYTE(&Temp);
		if (ok)
		{
			if (Temp == (BYTE) MarkRegion_FileFormatTerminator)			// End of list - break out of loop
				break;

			MarkPosition *NewPos = new MarkPosition;					// New position
			if (NewPos == NULL)
				ok = FALSE;
			else
			{
				NewPos->Initialise();
				NewPos->SetRegion((MarkRegion) Temp);					// Set region from first byte

				ok = Rec->ReadBYTE(&Temp);
				if (ok)
				{
					MarkFormat NewFormat;
					NewFormat.SetFromFlagByte(Temp);					// Set format from second byte

					NewPos->SetFormat(NewFormat);

					pMarkInfo->AddNewPosition(NewPos);					// And add to the mark's positions
				}
				else
					delete NewPos;
			}
		}
	}

	String_256 MenuName;
	if (ok)  ok = Rec->ReadUnicode((TCHAR *)MenuName, 255);				// Read the mark name
	if (ok)  pMarkInfo->SetMarkMenuText(&MenuName);

	if (ok)
	{
		// --- Finally, prepare for import of the following clipart subtree
		// Remember the current import context node
		CustomPreviousContext = pHandler->GetInsertContextNode();

		// Create a layer and set up the import system to import the mark into it
		// If this fails, then it'll just find somewhere "sensible" to import into
		CustomCurrentContext  = new Layer;
		pHandler->SetInsertContextNode(CustomCurrentContext);
	}

	// And now sit back and wait for the End call...
#endif
}



/********************************************************************************************

>	void PrintMarksComponent::EndImportCustomPrintMark(CamelotRecordHandler *pHandler)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96

	Inputs:		pHandler - the record handler that called us

	Purpose:	To end the import a custom print mark - see the SeeAlso for details.

	SeeAlso:	PrintMarksComponent::StartImportCustomPrintMark

********************************************************************************************/

void PrintMarksComponent::EndImportCustomPrintMark(CamelotRecordHandler *pHandler)
{
	PORTNOTETRACE("print","PrintMarksComponent::EndImportCustomPrintMark - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(CustomCurrentContext == NULL || pNewMark == NULL, "EndImportCustomPrintMark - StartImportCustomMark not called/failed");

	// Find the Print mark manager and it's PrintMark cache
	PrintMarksMan* pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan == NULL)
		return;

	// Restore the previous import context node
	pHandler->SetInsertContextNode(CustomPreviousContext);

	// Complete the mark and add it to the PMM cache
	NodeGroup *Glyph = (NodeGroup *)CustomCurrentContext->FindFirstChild();
	ERROR3IF(Glyph == NULL, "Import of custom mark subtree must have failed");
	if (Glyph != NULL)
	{
		ERROR3IF(!Glyph->IsKindOf(CC_RUNTIME_CLASS(NodeGroup)), "Imported print mark doesn't start with a group");

		Glyph->UnlinkNodeFromTree(NULL);
		UINT32 MarkHandle = pMarksMan->PMMCache.AddNewMark(pNewMark, Glyph);

		// Add a reference to the mark to ourself, so the mark is enabled
		AddMark(MarkHandle);
	}
	else
	{
		// Try not to leak too much!
		delete pNewMark;
	}

	// Tidy up
	pNewMark = NULL;
	CustomPreviousContext = NULL;
	delete CustomCurrentContext;
	CustomCurrentContext = NULL;
#endif
}

