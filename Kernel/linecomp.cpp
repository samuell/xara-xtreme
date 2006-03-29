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

// Line Component class implementation - abstract sort-of base class from 
// which to derive line components like strokes, brushes.

#include "camtypes.h"
#include "linecomp.h"
#include "camfiltr.h"
#include "fixmem.h"
#include "spread.h"
#include "layer.h"
#include "cxfrec.h"
#include "cxfrech.h"
#include "cxftags.h"

CC_IMPLEMENT_DYNAMIC(LineComponent, DocComponent)

LineDefinition **LineComponent::m_pLineArray = NULL;
UINT32 LineComponent::m_CurrentSize = 0;
UINT32 LineComponent::m_Used = 0;

UINT32 LineComponent::m_ImportHandle	= 0;
UINT32 LineComponent::m_ImportFlags	= 0;
UINT32 LineComponent::m_ImportData1	= 0;
UINT32 LineComponent::m_ImportData2	= 0;
InsertTreeContext *LineComponent::m_pImportPreviousContext = NULL;
Node *LineComponent::m_pImportNewLine = NULL;
String_32 LineComponent::m_ImportedName;

/********************************************************************************************

>	LineComponent::LineComponent()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Constructor

********************************************************************************************/

LineComponent::LineComponent()
{
	
}



/********************************************************************************************

>	LineComponent::~LineComponent()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Destructor

********************************************************************************************/

LineComponent::~LineComponent()
{
	if (m_pImportPreviousContext != NULL)
		delete m_pImportPreviousContext;
	if (m_pImportNewLine != NULL)
		delete m_pImportNewLine;
}



/********************************************************************************************

>	static void LineComponent::DeleteLineList(void)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Static deinitialisation function, called on shutdown.
				Clears out the global Line list. Must be called when all users of the
				Line system (documents) have already been deleted.

********************************************************************************************/

void LineComponent::DeleteLineList(void)
{
	if (m_pLineArray != NULL)
	{
		for (UINT32 i = 0; i < m_Used; i++)
		{
			if (m_pLineArray[i] != NULL)
				delete m_pLineArray[i];

			m_pLineArray[i] = NULL;
		}

		CCFree(m_pLineArray);
		m_pLineArray = NULL;
	}
}



/********************************************************************************************

>	static LineHandle LineComponent::AddNewItem(LineDefinition *pLine)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pLine - (may not be NULL) A Line definition to add, which now
						  belongs to the LineComponent, so you mustn't delete it!

	Returns:	A handle which uniquely identifies the new Line, or
				LineHandle_NoLine if we ran out of memory for storing Lines

	Purpose:	Adds the given Line to the global Line list.

	Notes:		IMPORTANT!
				The Line definition you pass in belongs to the LineComponent,
				and will be deleted when finished with. 

				YOU MUST NOT use the pLine pointer after this call! If the call
				fails, or if the Line is merged with an existing one, the object
				you passed in will be IMMEDIATELY DELETED! If you wish to use the
				Line after Adding it, you must call FindLine with the returned
				Line handle!

	Technical:	Lines are recorded in an array of LineDefinition pointers.
				The LineHandle is just an index into this array, for fast lookup.
				When deleted, the array-entry is set NULL and never re-used, so that
				any handles floating around can be safely used (much better than
				getting IndexedColour deleted when in use errors, eh? ;-).
				New entries are thus always appended to the list.

********************************************************************************************/

LineHandle LineComponent::AddNewItem(LineDefinition *pItem, BOOL)
{
	// ****!!!!TODO - if we're multi-threadig, this probably needs to be a critical section
	// because the list is global

	ERROR3IF(pItem == NULL, "Illegal NULL param");

	// --- First, check if we can merge the given Line with one already in the list
	for (UINT32 i = 0; i < m_Used; i++)
	{
		if (m_pLineArray[i] != NULL && !pItem->IsDifferent(m_pLineArray[i]))
		{
			// The Line is the same as one we already have in our Line list, so we
			// delete the copy that was passed in, and return the existing ones handle
			// We copy the import handle value over, though, so that we don't "lose"
			// merged Lines during import!
			m_pLineArray[i]->SetIOStore(pItem->ReadIOStore());
			delete pItem;
			return((LineHandle) i);
		}
	}

	// --- Next, check if we have enough room in our Line array to add a new entry to the end
	if (m_Used >= m_CurrentSize)
	{
		if (!ExpandArray())
		{
			delete pItem;
			return(LineHandle_NoLine);
		}
	}

	// --- OK, we have a totally new Line, so add it to the end of the list
	LineHandle NewHandle = (LineHandle) m_Used;
	m_pLineArray[m_Used] = pItem;
	m_Used++;


	// --- And return the new handle
	return(NewHandle);
}



/********************************************************************************************

>	LineDefinition *LineComponent::FindDefinition(UINT32 Handle, BOOL Dummy = FALSE);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		Handle - A handle which uniquely identifies the new stroke
					
	Returns:	A pointer to a StrokeDefinition for that Stroke.
				If the stroke has been deleted, NULL will be returned, in which case,
				you should gracefully degrade (by rendering an old style line in place of
				the stroke)

	Purpose:	Find a line definition, given its unique identity handle

********************************************************************************************/

LineDefinition *LineComponent::FindDefinition(LineHandle Handle, BOOL)
{
	if (Handle == LineHandle_NoLine)
		return(NULL);

	if (Handle >= m_Used)
	{
		ERROR3("Out of range Line handle");
		return(NULL);
	}

	return(m_pLineArray[Handle]);
}


/********************************************************************************************

>	BOOL LineComponent::StartImport(BaseCamelotFilter *pFilter)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)

	Purpose:	Called before we start to import a file

	SeeAlso:	DocComponent

********************************************************************************************/

BOOL LineComponent::StartImport(BaseCamelotFilter *pFilter)
{
	if (pFilter == NULL)
	{
		ERROR3("LineComponent::StartImport filter is null!");
		return(TRUE);
	}

	// Set the IOStore in all the Lines to 0xffffffff. During import, this entry is used
	// to map imported handles to the real handles they have been assigned.
	if (m_pLineArray != NULL)
	{
		for (UINT32 Index = 0; Index < m_Used; Index++)
		{
			if (m_pLineArray[Index] != NULL)
				m_pLineArray[Index]->SetIOStore(0xffffffff);
		}
	}

	return(TRUE);
}



/********************************************************************************************

>	BOOL LineComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.

	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)

	Purpose:	Called on completion of an import

********************************************************************************************/

BOOL LineComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
	return TRUE;
}


/********************************************************************************************

>	BOOL LineComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - True if everything went swimmingly, False if just a clean up is required.

	Returns:	TRUE if the component was able to end the exporting;
				FALSE if not (e.g. out of memory)

	Purpose:	Called on completion of file export

********************************************************************************************/

BOOL LineComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
	BOOL ok = TRUE;
	TRACEUSER( "Diccon", _T("End Export Brush\n"));
#if !defined(EXCLUDE_FROM_RALPH)
	if (pFilter == NULL)
	{
		ERROR3("LineComponent::EndExport filter is null!");
		return(ok);
	}

	if (!Success)		// If we failed to export, then return immediately
		return(ok);

#endif
	return(ok);
}



/********************************************************************************************

>	static BOOL ImportLine(CXaraFileRecord* pRecord)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		pRecord - The TAG_LineDEFINITION record to import

	Returns:	TRUE if Import was successful

	Purpose:	To import a vector Line definition

********************************************************************************************/

BOOL LineComponent::StartImportLine(CamelotRecordHandler *pHandler, CXaraFileRecord *pRecord)
{
	m_ImportHandle = 0xffffffff;
	m_ImportFlags  = 0;
	m_ImportData1  = 0;
	m_ImportData2  = 0;
	m_pImportPreviousContext = NULL;
	m_pImportNewLine = NULL;

	String_256 TempName;	// Load the name into a nice large safe buffer

	// Import the data from the record
	if (!pRecord->ReadUINT32(&m_ImportHandle) || !pRecord->ReadUINT32(&m_ImportFlags) ||
		!pRecord->ReadUINT32(&m_ImportData1) || !pRecord->ReadUINT32(&m_ImportData2))
	{
		return(FALSE);
	}

	// If there is a name to be imported, read it as well
	if ((m_ImportFlags & 0x200) != 0 && !pRecord->ReadUnicode(&TempName))
		return(FALSE);

	// We remember the imported flags and suchlike, which we will use in EndImportLine.
	TempName.Left((StringBase *)&m_ImportedName, 31);

	// Create a spread and set up the import system to import the brush into it
	// If this fails, then it'll just find somewhere "sensible" to import into
	m_pImportNewLine = new Spread;
	if (m_pImportNewLine == NULL)
		return(FALSE);

	Layer *pLineLayer = new Layer(m_pImportNewLine, FIRSTCHILD, String_256(TEXT("Jason did this")));
	if (pLineLayer == NULL)
	{
		delete m_pImportNewLine;
		m_pImportNewLine = NULL;
		return(FALSE);
	}

	// Now, remember where we were importing into, and point the importer at our brush tree
	m_pImportPreviousContext = pHandler->GetInsertContext();
	pHandler->SetInsertContextNode(pLineLayer);
	return(TRUE);
}



/********************************************************************************************

>	LineHandle LineComponent::FindImportedLine(UINT32 ImportedHandle)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	13/12/99

	Inputs:		Importedhandle - The handle which is used in the file being imported
								 to reference the Line.

	Returns:	The LineHandle of the loaded Line, or LineHandle_NoLine
				if the Line could not be found.

	Purpose:	To match up a handle from the currently importing file to the
				real internal LineHandle of the imported LineDefinition

********************************************************************************************/

LineHandle LineComponent::FindImportedLine(UINT32 ImportedHandle)
{
	ImportedHandle &= 0x00ffffff;	// Only the bottom 24 bits are relevant
	for (UINT32 Index = 0; Index < m_Used; Index++)
	{
		if (m_pLineArray[Index] != NULL && m_pLineArray[Index]->ReadIOStore() == ImportedHandle)
			return((LineHandle)Index);
	}

	return(LineHandle_NoLine);
}


/********************************************************************************************

>	BOOL LineComponent::NameIsUnique(const String_32& NameString)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	3/7/2000

	Inputs:		NameString - the string to test
	Returns:	TRUE if none of the definitions in the component have this string as their name,
				FALSE if one of them does

	Purpose:	To find out if someone has already gotten this name

********************************************************************************************/

BOOL LineComponent::NameIsUnique(const String_32& NameString)
{
	ERROR3IF(NameString.IsEmpty(), "Empty string in LineComponent::NameIsUnique");
	
	BOOL Unique = TRUE;
	UINT32 Counter = 0;
	String_32* pDefName = NULL;
	LineDefinition* pDef = NULL;
	while (Counter < m_Used)
	{
		pDef = FindDefinition(Counter);
		if (pDef == NULL)
		{
			ERROR3("null brush definition in LineComponent::NameIsUnique");
			break;
		}
		pDefName = pDef->GetLineName();
		if( pDefName != NULL )
		{
			if (NameString == *pDefName)
			{
				Unique = FALSE;
				break;
			}
		}
		Counter++;
	}
	return Unique;

}
