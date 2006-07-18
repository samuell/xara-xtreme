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

// This code provides the interface to the Camelot application that allows
// various preferences to be declared, and read/written from/to disk.
// It communicates with the OIL layer to to do the disk transfer.


#include "camtypes.h"

//#include "prefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "oilprefs.h"
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "basestr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "justin.h"
//#include "optsres.h"	// general options based resources
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]

static BOOL PrefsInitedOk = FALSE;

// Dynamic classing implementation
CC_IMPLEMENT_MEMDUMP(Preferences,       List)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

#if !defined(EXCLUDE_FROM_RALPH)

/********************************************************************************************

>	class PreferenceEntry : public CC_CLASS_MEMDUMP

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Purpose:	Instances of this class hold information about a single preference.
				This is a very simple public class with no member functions.
	SeeAlso:	PreferenceChunk; PreferenceSection; Preferences

********************************************************************************************/

class PreferenceEntry : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(PreferenceEntry)

public:
	// No constructor or destructor needed - simple public structure.
	
	LPTCHAR 		Name;		// Pointer to the name of the preference.
	PrefData		Data;		// Pointer to the variable that holds the preference value.
	PreferenceType	Type;		// Type of the preference - INT32, STRING, etc.
	
};


/********************************************************************************************

>	class PreferenceChunk : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Purpose:	Objects of this class hold a chunk of preferences for a particular section
				of the preferences.  There may be more than one chunk for a given section,
				depending on how accurate the initial declaration of the section's size was.
	SeeAlso:	-

********************************************************************************************/


class PreferenceChunk : public ListItem
{
	CC_DECLARE_MEMDUMP(PreferenceChunk)

public:
	PreferenceChunk(UINT32 InitialSize);
	~PreferenceChunk();
	
	void Write(OILPreferences* OILPrefs, LPTCHAR Section);
	
	// Adds a preference to the chunk.  Returns FALSE if there is not enough room to do
	// this (and so the caller should create a new chunk for it).
	BOOL AddPref(LPTCHAR Name, PrefData EntryData, PreferenceType Type);
	
private:
	UINT32 NumEntries;			// Number of entries in the chunk.
	UINT32 NumEntriesUsed;		// Number of entries in the chunk that have been used.

	PreferenceEntry *pEntries; 	// Pointer to the array of PreferenceEntry objects 
								// for this chunk.
public:								
	BOOL Valid; 				// Indicates that this is a valid chunk (i.e. it
								// initialised ok).

	// Allows application options dialog box to read and write values already declared in the
	// preference system.
	BOOL GetPrefValue(LPTCHAR Pref, PrefData EntryData, PreferenceType Type);
 	BOOL SetPrefValue(LPTCHAR Pref, PrefData EntryData, PreferenceType Type);

#ifdef _DEBUG
	BOOL PrefExists(LPTCHAR Pref);
#endif

};


/********************************************************************************************

>	class PreferenceSection : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Purpose:	Holds all the preferences for a particular section of the preference file.
				These preferences are stored in one or more instances of the PreferenceChunk
				class.  These instances are linked together as a List by this class.
	SeeAlso:	-

********************************************************************************************/


class PreferenceSection : public ListItem
{
	CC_DECLARE_MEMDUMP(PreferenceSection)

public:
	PreferenceSection(LPTCHAR SectionName, UINT32 InitialSize);
	~PreferenceSection();
	
	void Write(OILPreferences* OILPrefs);
	
	BOOL AddPref(OILPreferences* OILPrefs, LPTCHAR Pref,
				 PreferenceType Type, PrefData PrefVar);

	LPTCHAR Section;
	BOOL  Valid;
	
	List ChunkList;

	BOOL GetPrefValue(OILPreferences* OILPrefs, LPTCHAR Pref,
				 	  PreferenceType Type, PrefData PrefVar);
	BOOL SetPrefValue(OILPreferences* OILPrefs, LPTCHAR Pref,
				 	  PreferenceType Type, PrefData PrefVar);
};

// Dynamic classing implementation
CC_IMPLEMENT_MEMDUMP(PreferenceEntry,   CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(PreferenceChunk,   ListItem)
CC_IMPLEMENT_MEMDUMP(PreferenceSection, ListItem)

/********************************************************************************************

>	PreferenceChunk::PreferenceChunk(UINT32 InitialSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		InitialSize - the number of preferences this chunk should be able to hold.
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a chunk of memory which can contain preferences.
	Errors:		The member variable 'Valid' is FALSE if the required memory was not
				allocated.
	SeeAlso:	-

********************************************************************************************/

PreferenceChunk::PreferenceChunk(UINT32 InitialSize)
{
	Valid = FALSE;
	
	// Sanity check
	ENSURE(InitialSize > 0, "Attempt to create a PreferenceChunk object of zero size");

	// Allocate the memory for this chunk.
	pEntries = new PreferenceEntry[InitialSize];
	
	if (pEntries == NULL)
	{
		// Not enough memory
		NumEntries = 0;
		NumEntriesUsed = 0;
	}
	
	// Set the entries fields correctly and tell the caller it worked
	NumEntries = InitialSize;
	NumEntriesUsed = 0;
	Valid = TRUE;
}

/********************************************************************************************

>	PreferenceChunk::~PreferenceChunk()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys the PreferenceChunk - it deallocates any memory previously
				allocated for PreferenceEntry objects by this object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PreferenceChunk::~PreferenceChunk()
{
	PreferenceEntry *pEntry = pEntries;
	
	for (UINT32 i = 0; i < NumEntriesUsed; i++)
	{
		if (pEntry->Name)
		{
			CCFree(pEntry->Name);
			pEntry->Name=NULL;
		}
		pEntry++;
	}	
	
	if (pEntries != NULL)
		delete [] pEntries;
}

	
/********************************************************************************************

>	BOOL PreferenceChunk::AddPref(LPTCHAR Name, PrefData EntryData, PreferenceType Type)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		Name 	  - the name of the section to add this preference to.
				EntryData - pointer to the memory that will hold the value of this
							preference.
				Type	  - the type of this preference - i.e., the type of data pointed to
						   	by the EntryData argument.
	Outputs:	-
	Returns:	TRUE if the preference could be added to this chunk - FALSE if the chunk
				is full and the preference could not be added.
	Purpose:	Adds the given preference to this chunk.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PreferenceChunk::AddPref(LPTCHAR Name, PrefData EntryData, PreferenceType Type)
{
	// Is there enough space left to do this?
	if ((!Valid) || (NumEntriesUsed >= NumEntries))
		return FALSE;
	
	// Fill in the entry (NB. obscure pointer arithmetic)
	PreferenceEntry *pEntry = pEntries + NumEntriesUsed;
	
	// Alex put in something to make a copy of the name
	UINT32 memsize = (camStrlen(Name) + 1) * sizeof(TCHAR);
	pEntry->Name = (LPTCHAR) CCMalloc( memsize );
	if (!pEntry->Name) return FALSE; // error already set
	camStrcpy(pEntry->Name, Name);
//	pEntry->Name 	= Name;

	pEntry->Data 	= EntryData;
	pEntry->Type 	= Type;
	
	// Update chunk information
	NumEntriesUsed++;
	
	// Return success
	return TRUE;
}

/********************************************************************************************

>	void PreferenceChunk::Write(OILPreferences* OILPrefs, LPTCHAR Section)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		OILPrefs - The OIL object to use to write the preferences out with.
				Section  - the name of the section that this chunk is a part of.
	Outputs:	-
	Returns:	-
	Purpose:	Writes out all the preferences contained in this chunk object, using the OIL.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PreferenceChunk::Write(OILPreferences* OILPrefs, LPTCHAR Section)
{
	// Write out each preference in this chunk

	PreferenceEntry *pEntry = pEntries;
	
	for (UINT32 i = 0; i < NumEntriesUsed; i++)
	{
		// Write this preference
		OILPrefs->Write(Section, pEntry->Name, pEntry->Type, pEntry->Data);

		// Move on to the next preference
		pEntry++;
	}
}



/********************************************************************************************

>	BOOL PreferenceChunk::GetPrefValue(LPTCHAR Pref, PrefData EntryData, PreferenceType Type)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Pref 	  - name of preference to look for.
				EntryData - pointer to the variable that will be used to pass the preference
							value back in.
				Type	  - the type of this preference - i.e., the type of data pointed to
						   	by the EntryData argument.
	Outputs:	EntryData - pointer to the variable that should now be set up to echo the
							value of the specified preference.
	Returns:	TRUE if the preference exists, FALSE otherwise.
	Purpose:	Checks to see if the given preference exists in this chunk and if so returns
				the value currently assigned to that preference value by placing it in the
				variable pointed to by the PrefData structure passed in. This means one routine
				can be used as opposed to one per preference type.
	Errors:		-
	SeeAlso:	PreferenceChunk::SetPrefValue

********************************************************************************************/

BOOL PreferenceChunk::GetPrefValue(LPTCHAR Pref, PrefData EntryData, PreferenceType Type)
{
	PreferenceEntry *pEntry = pEntries;
	
	for (UINT32 i = 0; i < NumEntriesUsed; i++)
	{
		if (camStricmp(Pref, pEntry->Name) == 0)
		{
			// Found it so it does exist
			// Check that the type corresponds to the correct one
			if (pEntry->Type == Type)
			{
				// Preference exits and is of the correct type so return it	by writing 
				// the value pointed to by the preference into the value pointed to by
				// the preference structure passed in.
				switch (Type)
				{
					case	PREF_INT:
						// integer supplied so use the pointer to integer
						*EntryData.pInt = *pEntry->Data.pInt;
					break;
					case	PREF_UINT:
						*EntryData.pUInt = *pEntry->Data.pUInt;
					break;
					case	PREF_DOUBLE:
						*EntryData.pDouble = *pEntry->Data.pDouble;
					break;
					case	PREF_STRING:
						// Shouldn't be required so disable
						//*EntryData.pString = *pEntry->Data.pString;
						return FALSE;
					break;
				}
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		// Move on to the next preference
		pEntry++;
	}
	
	// Not found
	return FALSE;
}

/********************************************************************************************

>	BOOL PreferenceChunk::SetPrefValue(LPTCHAR Pref, PrefData EntryData, PreferenceType Type)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Pref - name of preference to look for.
				EntryData - pointer to the memory that is holding the value of this
							preference that we wish to set.
				Type	  - the type of this preference - i.e., the type of data pointed to
						   	by the EntryData argument.
	Outputs:	-
	Returns:	TRUE if the preference exists, FALSE otherwise.
	Purpose:	Checks to see if the given preference exists in this chunk and if so sets the
				value stored in the preference user's variable to be the specified new value.
	Errors:		-
	SeeAlso:	PreferenceChunk::GetPrefValue

********************************************************************************************/

BOOL PreferenceChunk::SetPrefValue(LPTCHAR Pref, PrefData EntryData, PreferenceType Type)
{
	PreferenceEntry *pEntry = pEntries;
	
	for (UINT32 i = 0; i < NumEntriesUsed; i++)
	{
		if (camStricmp(Pref, pEntry->Name) == 0)
		{
			// Found it so it does exist
			// Check that the type corresponds to the correct one
			if (pEntry->Type == Type)
			{
				// Preference exits and is of the correct type so return it	by writing 
				// the value pointed to by the preference into the value pointed to by
				// the preference structure passed in.
				switch (Type)
				{
					case	PREF_INT:
						// integer supplied so use the pointer to integer
						*pEntry->Data.pInt = *EntryData.pInt;
					break;
					case	PREF_UINT:
						*pEntry->Data.pUInt = *EntryData.pUInt;
					break;
					case	PREF_DOUBLE:
						*pEntry->Data.pDouble = *EntryData.pDouble;
					break;
					case	PREF_STRING:
						// Shouldn't be required so disable
						//*EntryData.pString = *pEntry->Data.pString;
						return FALSE;
					break;
				}
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		// Move on to the next preference
		pEntry++;
	}
	
	// Not found
	return FALSE;
}


/********************************************************************************************

>	BOOL PreferenceChunk::PrefExists(LPTCHAR Pref)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/93
	Inputs:		Pref - name of preference to look for.
	Outputs:	-
	Returns:	TRUE if the preference exists, FALSE otherwise.
	Purpose:	Checks to see if the given preference exists in this chunk.
				Only exists in DEBUG builds.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#ifdef _DEBUG
                                                
BOOL PreferenceChunk::PrefExists(LPTCHAR Pref)
{
	PreferenceEntry *pEntry = pEntries;
	
	for (UINT32 i = 0; i < NumEntriesUsed; i++)
	{
		if (camStricmp(Pref, pEntry->Name) == 0)
			// Found it so it does exist
			return TRUE;
		
		// Move on to the next preference
		pEntry++;
	}
	
	// Not found
	return FALSE;
}

#endif

/********************************************************************************************

>	PreferenceSection::PreferenceSection(LPTCHAR SectionName, UINT32 InitialSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		SectionName - The name to be associated with this section object.
				InitialSize - The caller's estimate of how many preferences will typically
							  be stored in this section.  Used as a guideline for allocating
							  memory.
	Outputs:	-
	Returns:	-
	Purpose:	Creates a new section object.  A chunk of memory is allocated to hold the
				preferences for this object.  If the memory is not available for this, then
				the 'Valid' data member contains the value 'FALSE'.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PreferenceSection::PreferenceSection(LPTCHAR SectionName, UINT32 InitialSize)
{
	Valid = FALSE;
	
	// Remember the name of this section (note that it uses the original - it does not
	// take a copy).
	Section = SectionName;

	// Get a preference chunk for this section, using the size recommended
	// by the caller
	PreferenceChunk *pChunk = new PreferenceChunk(InitialSize);
	
	if ((pChunk == NULL) || (!pChunk->Valid))
	{
		delete pChunk;
		return; // Section is invalid
	}
		
	// Got a chunk ok, so add it to the head of the (empty) list.
	ChunkList.AddHead(pChunk);
	
	// Success
	Valid = TRUE;
}

/********************************************************************************************

>	PreferenceSection::~PreferenceSection()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes all PreferenceChunk objects used by this section.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PreferenceSection::~PreferenceSection()
{
	PreferenceChunk *pChunk;
	
	// Delete all the chunks for this section
	do
	{
		// Unlink the first item from the list
		pChunk = (PreferenceChunk *) ChunkList.RemoveHead();

		// It doesn't matter if pChunk is NULL - delete handles it
		delete pChunk;
		
	} while (pChunk != NULL);
}
	
/********************************************************************************************

>	BOOL PreferenceSection::AddPref(OILPreferences* OILPrefs,
									LPTCHAR Pref, PreferenceType Type, PrefData PrefVar)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		Pref     - the name of the preference to add (e.g. "autosaveinterval")
				Type     - the type of data used for this preference.
				PrefVar  - pointer to the memory that holds this preference's data.
				OILPrefs - the object to use to read the preference from the file.
	Outputs:	-
	Returns:	TRUE if the preference was added to the preference system successfully, 
				FALSE otherwise.
	Purpose:	Adds a preference to this PreferenceSection object.  If a new PreferenceChunk
				is needed to do this, it is allocated automatically, and added to this
				object's list of chunks.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PreferenceSection::AddPref(OILPreferences* OILPrefs,
								LPTCHAR Pref, PreferenceType Type, PrefData PrefVar)
{
#ifdef _DEBUG
	if ((camStrnicmp(Pref, _T("blobby"), 7) == 0) ||
		(camStrnicmp(Pref, _T("wobjob"), 7) == 0))
	{
		// Silly names not allowed
		ENSURE(FALSE, "Preference section has suffered a cliche overload");
		abort();
	}
#endif

	// Used for scanning the chunk list
	PreferenceChunk *pChunk;

	// In debug builds, check that this preference has not already been declared.
	// This check ensures that two preferences with the same name are not used.
	
#ifdef _DEBUG
	pChunk = (PreferenceChunk *) ChunkList.GetHead();
	
	while (pChunk != NULL)
	{
		ERROR3IF_PF((pChunk->PrefExists(Pref) != FALSE), ( "Preference '%s' in section '%s' declared more than once", Pref, Section ) );

		pChunk = (PreferenceChunk *) ChunkList.GetNext(pChunk);
	}
	
#endif

	// New chunks are added to the head of the list, so the partially unused one
	// is always the first in the list.
	pChunk = (PreferenceChunk *) ChunkList.GetHead();
	
	// Try to add the preference to this chunk.
	if (!pChunk->AddPref(Pref, PrefVar, Type))
	{
		// No room left in this chunk - try to get another one and put it in that.
		// The extension chunk is big enough to hold another 10 preferences (this is pretty
		// arbitrary).
		pChunk = new PreferenceChunk(10);
		
		if ((pChunk == NULL) || (!pChunk->Valid))
		{
			delete pChunk;
			return FALSE; // Failure - no memory left.
		}
			
		// Got a chunk ok, so add it to the head of the list.
		ChunkList.AddHead(pChunk);
		
		// Add the preference to it ('guaranteed' to work)
		pChunk->AddPref(Pref, PrefVar, Type);
	}

	// Try to read the preference
	if (OILPrefs != NULL)
		OILPrefs->Read(Section, Pref, Type, PrefVar);
	
	return TRUE;
}

/********************************************************************************************

>	BOOL PreferenceSection::GetPrefValue(OILPreferences* OILPrefs,
										 LPTCHAR Pref, PreferenceType Type, PrefData PrefVar)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Pref     - the name of the preference to add (e.g. "autosaveinterval")
				Type     - the type of data used for this preference.
				PrefVar  - pointer to the memory that holds this preference's data.
				OILPrefs - the object to use to read the preference from the file.
	Outputs:	-
	Returns:	TRUE if the preference was read from the preference system successfully, 
				FALSE otherwise.
	Purpose:	Reads a preference value from this PreferenceSection object by looking
				through all the chunks in this section for the named preference.
	Errors:		-
	SeeAlso:	PreferenceSection::GetPrefValue

********************************************************************************************/

BOOL PreferenceSection::GetPrefValue(OILPreferences* OILPrefs,
									 LPTCHAR Pref, PreferenceType Type, PrefData PrefVar)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Used for scanning the chunk list
	PreferenceChunk *pChunk;

	// Check that this preference has not already been declared.
	// New chunks are added to the head of the list, so the partially unused one
	// is always the first in the list.
	pChunk = (PreferenceChunk *) ChunkList.GetHead();
	
	while (pChunk != NULL )
	{
		// Try to read the preference value from this chunk.
		if (pChunk->GetPrefValue(Pref, PrefVar, Type))
			return TRUE;		// The read went ok so return that value to the caller

		// Preference not in that chunk so move onto next chunk in list 
		pChunk = (PreferenceChunk *) ChunkList.GetNext(pChunk);
	}
#endif

	return FALSE;		// Value was not read ok, not the correct type or not present
}

/********************************************************************************************

>	BOOL PreferenceSection::SetPrefValue(OILPreferences* OILPrefs,
										 LPTCHAR Pref, PreferenceType Type, PrefData PrefVar)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Pref     - the name of the preference to add (e.g. "autosaveinterval")
				Type     - the type of data used for this preference.
				PrefVar  - pointer to the memory that holds the new preference's data value.
				OILPrefs - the object to use to read the preference from the file.
	Outputs:	-
	Returns:	TRUE if the preference was found in the preference system successfully and
				the new value applied to it, FALSE otherwise.
	Purpose:	Finds a preference value from this PreferenceSection object by looking
				through all the chunks in this section for the named preference and then sets
				the value to the specifed new value.
	Errors:		-
	SeeAlso:	PreferenceSection::GetPrefValue

********************************************************************************************/

BOOL PreferenceSection::SetPrefValue(OILPreferences* OILPrefs,
									 LPTCHAR Pref, PreferenceType Type, PrefData PrefVar)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Used for scanning the chunk list
	PreferenceChunk *pChunk;

	// Check that this preference has not already been declared.
	// New chunks are added to the head of the list, so the partially unused one
	// is always the first in the list.
	pChunk = (PreferenceChunk *) ChunkList.GetHead();
	
	while (pChunk != NULL )
	{
		// Try to read the preference value from this chunk.
		if (pChunk->SetPrefValue(Pref, PrefVar, Type))
			return TRUE;		// The read went ok so return that value to the caller

		// Preference not in that chunk so move onto next chunk in list 
		pChunk = (PreferenceChunk *) ChunkList.GetNext(pChunk);
	}
#endif

	return FALSE;		// Value was not set ok, not the correct type or not present
}


/********************************************************************************************

>	void PreferenceSection::Write(OILPreferences* OILPrefs)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		OILPrefs - the OIL object to use to write out the preferences with.
	Outputs:	-
	Returns:	-
	Purpose:	Writes all the preferences in this section out to the preference file,
				using the OIL.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void PreferenceSection::Write(OILPreferences* OILPrefs)
{
	// Write out each chunk in this section
	PreferenceChunk *pChunk = (PreferenceChunk *) ChunkList.GetHead();
	
	while (pChunk != NULL)
	{
		// Write out this chunk and move on to the next
		pChunk->Write(OILPrefs, Section);
		pChunk = (PreferenceChunk *) ChunkList.GetNext(pChunk);	
	}
}

#endif

/********************************************************************************************

>	Preferences::Preferences()


	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the section caching system.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Preferences::Preferences()
{
	CurrentSection = NULL;
	OILPrefs 	   = NULL;
}

/********************************************************************************************

>	Preferences::~Preferences()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes all PreferenceSection objects used by this object.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Preferences::~Preferences()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Close the preferences file
	if (OILPrefs != NULL)
	{
		OILPrefs->CloseInput();
		delete OILPrefs;
	}
		
	PreferenceSection *pSection;
	
	// Delete all the sections for this preferences object
	do
	{
		// Unlink the first item from the list
		pSection = (PreferenceSection *) this->RemoveHead();

		// It doesn't matter if pSection is NULL - delete handles it
		delete pSection;
		
	} while (pSection != NULL);
#endif
}

	
/********************************************************************************************

>	void Preferences::Write()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This writes out all Camelot preferences to the application's preference
				file, using the OIL.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Preferences::Write()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// First check if there is a valid OILPrefs, as if there was a problem on start up such as
	// the MSCVRTn.DLL failing to load then the preferences may have been not fully set up.
	// Static variable used as the class may not have been created!
	if (PrefsInitedOk && (OILPrefs != NULL))
	{
		// Perform any preparation necessary to start writing preferences
		OILPrefs->OpenOutput();

		// Write out all the sections for this preferences object
		PreferenceSection *pSection = (PreferenceSection *) this->GetHead();
	
		while (pSection != NULL)
		{
			// Write out this section and move on to the next
			pSection->Write(OILPrefs);
			pSection = (PreferenceSection *) this->GetNext(pSection);	
		}

		// Perform any platform-deppy clean-up required after writing the preferences
		OILPrefs->CloseOutput();
	}
#endif
}
	
/********************************************************************************************

>	BOOL Preferences::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE if an error occured.
	Purpose:	This initialises the Preferences object.  This includes setting up a link
				to the OIL and opening the preference file for reading.
	Errors:		-
	SeeAlso:	Preferences::Deinit; OILPreferences

********************************************************************************************/

BOOL Preferences::Init()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Create a link to the OIL
	OILPrefs = OILPreferences::Init();
	
	// Flag error if it failed
	if (OILPrefs == NULL)
		return FALSE;

	PrefsInitedOk = TRUE;

	// Initialise the preference file for reading
	return OILPrefs->OpenInput();

#else //EXCLUDE_FROM_RALPH
	// Be gone you pesky prefs system :-)
	OILPrefs = NULL;
	PrefsInitedOk = FALSE;
	return TRUE;
#endif //EXCLUDE_FROM_RALPH
}

/********************************************************************************************

>	void Preferences::Deinit()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if ok, FALSE if an error occured.
	Purpose:	This de-initialises the Preferences object.  This includes closing down
				whatever OIL mechanisms were set up by Init().
	Errors:		-
	SeeAlso:	Preferences::Init; OILPreferences

********************************************************************************************/

void Preferences::Deinit()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Close the preferences file
	if (OILPrefs != NULL)
		OILPrefs->CloseInput();
#endif
}

/********************************************************************************************

>	void Preferences::WipePreferenceFile()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/93
	Purpose:	Used to remove an old (possibly incompatible) preference file.  It assumes
				that the preference system has been initialised and that it is still
				active (i.e. Deinit() has not been called).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Preferences::WipePreferenceFile()
{
#if !defined(EXCLUDE_FROM_RALPH)
	ENSURE(OILPrefs != NULL, "OILPreferences system not initialised");

	// Close the file, clear it, and re-open it
	OILPrefs->CloseInput();
	OILPrefs->WipePreferenceFile();
	OILPrefs->OpenInput();
#endif
}

/********************************************************************************************

>	void Preferences::WipePreferenceFile()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/2001
	Purpose:	Used to remove any dangerous sections from the registry, when upgrading from
				an old version
	SeeAlso:	Application::WipeDangerousPrefs

********************************************************************************************/

void Preferences::WipeDangerousPrefs()
{
#if !defined(EXCLUDE_FROM_RALPH)
	ENSURE(OILPrefs != NULL, "OILPreferences system not initialised");

	OILPrefs->WipeDangerousPrefs();
#endif
}

/********************************************************************************************

>	BOOL Preferences::DeclareSection(LPTCHAR Section, UINT32 InitialSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		Section     - name of the section being declared.
				InitialSize - caller's estimate of how many preferences will be stored in
							  this section.
	Outputs:	-
	Returns:	TRUE if the section was created successfully, FALSE otherwise.
				NB. If the section is subsequently re-declared, this function does nothing,
					but returns TRUE.
	Purpose:	This should be used to declare a new preference section to the system.
				No preferences of this section can be declared until the section itself
				has been declared.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Preferences::DeclareSection(LPTCHAR Section, UINT32 InitialSize)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Check that somebody is not trying to define a null section name or a zero length
	// string as this may cause problems in the string compares. (make it one or less for now)
	ERROR2IF(((Section == NULL) || (camStrclen(Section) <= 1)), FALSE, _R(IDN_BAD_DECLARE_SECTION));

	PreferenceSection* pSection = NULL;

	// Make sure this section hasn't already been declared
	pSection = GetSection(Section);
	if (pSection != NULL)
	{
		CurrentSection = pSection;
		return TRUE; // Should this be an error condition?
	}
		
	// Add a new section
	pSection = new PreferenceSection(Section, InitialSize);
	if ((pSection == NULL) || (!pSection->Valid))
	{
		// Not enough memory to do this
		delete pSection;
		return FALSE;
	}
	
	// It all worked, so add this section to the list, remember it for future use,
	// and return success.
	this->AddHead(pSection);
	CurrentSection = pSection;
	return TRUE;
#else
	// Bodge so all hell doesn't break loose :-)
	return TRUE;
#endif
}
	
/********************************************************************************************

>	BOOL Preferences::DeclarePref(LPTCHAR Section, LPTCHAR Pref, 
								  INT32 *PrefVar, INT32 Min, INT32 Max)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the integer variable that holds this preference's value.
				Min,Max - Legal range for this value - if the preferences file holds a value
						  outside this range, it is ignored and the default value is used
						  instead.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was added to the preference system successfully,
				FALSE otherwise.
	Purpose:	Declares a preference to the preference system.
	
				This has two effects:
				
					+ When this function returns, if this preference was found in the
					  preference file that was read on start-up, then the PrefVar variable
					  is filled in with the value from this file.
					  
					+ Whenever the preferences are saved (via Preferences::Write(), which
					  would happen on application exit, or when the user chooses "Save
					  Preferences"), this named preference is saved into the named section,
					  using whatever value the PrefVar points to *when the preferences
					  are saved*.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Preferences::DeclarePref(LPTCHAR Section, LPTCHAR Pref, 
							  INT32 *PrefVar, INT32 Min, INT32 Max)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Add the preference to the right section		
	PrefData Data;
	Data.pInt = PrefVar;
	
	// Remember default value
	INT32 Default = *PrefVar;
	
	if (!pSection->AddPref(OILPrefs, Pref, PREF_INT, Data))
	{
		*PrefVar = Default;
		ERROR(_R(IDE_BAD_INI_FILE), FALSE);
		return FALSE; 						// Something went wrong
	}

	// Ensure value is within specified range	
	if (*PrefVar < Min)
		*PrefVar = Min;
	if (*PrefVar > Max)
		*PrefVar = Max;
#endif
	
	return TRUE;
}

/********************************************************************************************

>	BOOL Preferences::DeclarePref(LPTCHAR Section, LPTCHAR Pref, 
								  UINT32 *PrefVar, UINT32 Min, UINT32 Max)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the unsigned integer variable that holds this 
						  preference's value.
				Min,Max - Legal range for this value - if the preferences file holds a value
						  outside this range, it is ignored and the default value is used
						  instead.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was added to the preference system successfully,
				FALSE otherwise.
	Purpose:	Declares a preference to the preference system.
	
				This has two effects:
				
					+ When this function returns, if this preference was found in the
					  preference file that was read on start-up, then the PrefVar variable
					  is filled in with the value from this file.
					  
					+ Whenever the preferences are saved (via Preferences::Write(), which
					  would happen on application exit, or when the user chooses "Save
					  Preferences"), this named preference is saved into the named section,
					  using whatever value the PrefVar points to *when the preferences
					  are saved*.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Preferences::DeclarePref(LPTCHAR Section, LPTCHAR Pref, 
							  UINT32 *PrefVar, UINT32 Min, UINT32 Max)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Add the preference to the right section		
	PrefData Data;
	Data.pUInt = PrefVar;
	
	// Remember default value
	UINT32 Default = *PrefVar;
	
	if (!pSection->AddPref(OILPrefs, Pref, PREF_UINT, Data))
	{
		*PrefVar = Default;
		ERROR(_R(IDE_BAD_INI_FILE), FALSE);
		return FALSE; // Something went wrong
	}

	// Ensure value is within specified range	
	if (*PrefVar < Min)
		*PrefVar = Min;
	if (*PrefVar > Max)
		*PrefVar = Max;
#endif		
	return TRUE;
}

/********************************************************************************************

>	BOOL Preferences::DeclarePref(LPTCHAR Section, LPTCHAR Pref, double *PrefVar)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the floating point variable that holds this preference's
						  value.
				Min,Max - Legal range for this value - if the preferences file holds a value
						  outside this range, it is ignored and the default value is used
						  instead.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was added to the preference system successfully,
				FALSE otherwise.
	Purpose:	Declares a preference to the preference system.
	
				This has two effects:
				
					+ When this function returns, if this preference was found in the
					  preference file that was read on start-up, then the PrefVar variable
					  is filled in with the value from this file.
					  
					+ Whenever the preferences are saved (via Preferences::Write(), which
					  would happen on application exit, or when the user chooses "Save
					  Preferences"), this named preference is saved into the named section,
					  using whatever value the PrefVar points to *when the preferences
					  are saved*.
					  
				Note that doubles are stored to 20 decimal places in the preference file.
				Of course, how many of these are accurate depends on the value, e.g.
				
				 2372645745823.3476354716324 would not be stored completely accurately
				 whereas 23.3245345646 probably would.
				 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Preferences::DeclarePref(LPTCHAR Section, LPTCHAR Pref, 
							  double *PrefVar, double Min, double Max)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Add the preference to the right section		
	PrefData Data;
	Data.pDouble = PrefVar;
	
	// Remember default value
	double Default = *PrefVar;
	
	if (!pSection->AddPref(OILPrefs, Pref, PREF_DOUBLE, Data))
	{
		*PrefVar = Default;
		ERROR(_R(IDE_BAD_INI_FILE), FALSE);		// Something went wrong
	}

	// Ensure value is within specified range	
	if (*PrefVar < Min)
		*PrefVar = Min;
	if (*PrefVar > Max)
		*PrefVar = Max;
#endif		
	return TRUE;
}

/********************************************************************************************

>	BOOL Preferences::DeclarePref(LPTCHAR Section, LPTCHAR Pref, StringBase *PrefVar)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the character buffer that holds this preference's value.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was added to the preference system successfully,
				FALSE otherwise.
	Purpose:	Declares a preference to the preference system.
	
				This has two effects:
				
					+ When this function returns, if this preference was found in the
					  preference file that was read on start-up, then the PrefVar variable
					  is filled in with the value from this file.
					  
					+ Whenever the preferences are saved (via Preferences::Write(), which
					  would happen on application exit, or when the user chooses "Save
					  Preferences"), this named preference is saved into the named section,
					  using whatever value the PrefVar points to *when the preferences
					  are saved*.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Preferences::DeclarePref(LPTCHAR Section, LPTCHAR Pref, StringBase *PrefVar)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Add the preference to the right section
	PrefData Data;
	Data.pString = PrefVar;
	ERRORIF(!pSection->AddPref(OILPrefs, Pref, PREF_STRING, Data), _R(IDE_BAD_INI_FILE), FALSE);
#endif
	return TRUE;
}

/********************************************************************************************

>	PreferenceSection *Preferences::GetSection(LPTCHAR SectionName)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/93
	Inputs:		SectionName - the name of the section to find.
	Outputs:	-
	Returns:	A pointer to the PreferenceSection object that corresponds to the section
				name specified, or NULL if no such section has yet been declared.
	Purpose:	Given a section name, this function will find the PreferenceSection object
				that stores the preferences for this section.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PreferenceSection *Preferences::GetSection(LPTCHAR SectionName)
{
#if !defined(EXCLUDE_FROM_RALPH)
#ifdef _DEBUG
	if ((SectionName != NULL) &&
	    ((camStrnicmp(SectionName, _T("blobby"), 7) == 0) ||
		 (camStrnicmp(SectionName, _T("wobjob"), 7) == 0)))
	{
		// Silly names not allowed
		ENSURE(FALSE, "Preference section has suffered a cliche overload");
		abort();
	}
#endif

	// Check that somebody is not trying to use a null section name when there
	// is no current section set up 
	ERROR2IF((CurrentSection == NULL)
	 		  && (SectionName == NULL), NULL, _R(IDN_BAD_CURRENT_SECTION));
	
	// A null section name means use the MRU section.
	if (SectionName == NULL)
		return CurrentSection;

	// Check that the specified section name is not specified as a zero length string
	// otherwise this could screw up the name comparing. Duplicate null check just in
	// case check above is removed.
	ERROR2IF(((SectionName != NULL) &&
			 (camStrclen(SectionName) <= 1)), NULL, _R(IDN_BAD_CURRENT_SECTION));
		
	// Do a quick check to see if it is the cached section.
	if ((CurrentSection != NULL) && 
		(camStricmp(SectionName, CurrentSection->Section) == 0))
		// It's the same one as we tried last time.
		return CurrentSection;
		
	// Search the list for the right section

	PreferenceSection *pSection = (PreferenceSection *) this->GetHead();
	
	while (pSection != NULL)
	{
		if (camStricmp(SectionName, pSection->Section) == 0)
		{
			// Found the right section - remember it for next time and return it.
			CurrentSection = pSection;
			return pSection;
		}
			
		// Try the next one in the list
		pSection = (PreferenceSection *) this->GetNext(pSection);
	}
#endif
	
	// Didn't find it...
	return NULL;
}




/********************************************************************************************

>	BOOL Preferences::GetPrefValue(LPTCHAR Section, LPTCHAR Pref, INT32 *PrefVar)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the integer variable that holds this preference's value.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was found in the preference system successfully,
				FALSE otherwise.
	Purpose:	Finds the value of the specified preference which should already be defined in
				the preference system and returns it in the variable PrefVar. 
				This is used by interested parties other than the owner of that preference
				value e.g. the application options dialog box.
	
	Errors:		-
	SeeAlso:	Preferences::SetPrefValue

********************************************************************************************/

BOOL Preferences::GetPrefValue(LPTCHAR Section, LPTCHAR Pref, INT32 *PrefVar)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section, error if not found and return FALSE
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Set up the data structure required to read the preference value		
	PrefData Data;
	Data.pInt = PrefVar;
	
	// Remember default value
	INT32 Default = *PrefVar;
	
	if (!pSection->GetPrefValue(OILPrefs, Pref, PREF_INT, Data))
	{
		*PrefVar = Default;
		ERROR(_R(IDE_BAD_INI_FILE), FALSE);
		return FALSE; 						// Something went wrong
	}
	
	// Value found should have been placed in PrefVar 

	*PrefVar = *Data.pInt;
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL Preferences::GetPrefValue(LPTCHAR Section, LPTCHAR Pref, UINT32 *PrefVar)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the unsigned integer variable that holds this
						  preference's value.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was found in the preference system successfully,
				FALSE otherwise.
	Purpose:	Finds the value of the specified preference which should already be defined in
				the preference system and returns it in the variable PrefVar. 
				This is used by interested parties other than the owner of that preference
				value e.g. the application options dialog box.
	
	Errors:		-
	SeeAlso:	Preferences::SetPrefValue

********************************************************************************************/

BOOL Preferences::GetPrefValue(LPTCHAR Section, LPTCHAR Pref, UINT32 *PrefVar)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section, error if not found and return FALSE
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Set up the data structure required to read the preference value		
	PrefData Data;
	Data.pUInt = PrefVar;
	
	// Remember default value
	UINT32 Default = *PrefVar;
	
	if (!pSection->GetPrefValue(OILPrefs, Pref, PREF_UINT, Data))
	{
		*PrefVar = Default;
		ERROR(_R(IDE_BAD_INI_FILE), FALSE);
		return FALSE; 						// Something went wrong
	}
	
	// Value found should have been placed in PrefVar 

	*PrefVar = *Data.pUInt;
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL Preferences::GetPrefValue(LPTCHAR Section, LPTCHAR Pref, double *PrefVar)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the double variable that holds this preference's value.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was found in the preference system successfully,
				FALSE otherwise.
	Purpose:	Finds the value of the specified preference which should already be defined in
				the preference system and returns it in the variable PrefVar. 
				This is used by interested parties other than the owner of that preference
				value e.g. the application options dialog box.
	
	Errors:		-
	SeeAlso:	Preferences::SetPrefValue

********************************************************************************************/

BOOL Preferences::GetPrefValue(LPTCHAR Section, LPTCHAR Pref, double *PrefVar)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section, error if not found and return FALSE
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Set up the data structure required to read the preference value		
	PrefData Data;
	Data.pDouble = PrefVar;
	
	// Remember default value
	double Default = *PrefVar;
	
	if (!pSection->GetPrefValue(OILPrefs, Pref, PREF_DOUBLE, Data))
	{
		*PrefVar = Default;
		ERROR(_R(IDE_BAD_INI_FILE), FALSE);
		return FALSE; 						// Something went wrong
	}
	
	// Value found should have been placed in PrefVar 

	*PrefVar = *Data.pDouble;
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL Preferences::SetPrefValue(LPTCHAR Section, LPTCHAR Pref, INT32 *PrefVar)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the integer variable that holds this preference's value.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was found in the preference system successfully,
				FALSE otherwise.
	Purpose:	Finds the value of the specified preference which should already be defined in
				the preference system and then writes the new value specified into that 
				variable.
				This is used by interested parties other than the owner of that preference
				value e.g. the application options dialog box.
	
	Errors:		-
	SeeAlso:	Preferences::GetPrefValue

********************************************************************************************/

BOOL Preferences::SetPrefValue(LPTCHAR Section, LPTCHAR Pref, INT32 *PrefVar)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section, error if not found and return FALSE
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Set up the data structure required to pass the new preference value
	// in with		
	PrefData Data;
	Data.pInt = PrefVar;
	
	if (!pSection->SetPrefValue(OILPrefs, Pref, PREF_INT, Data))
	{
		ERROR(_R(IDE_BAD_INI_FILE), FALSE);
		return FALSE; 						// Something went wrong
	}
#endif	
	return TRUE;
}

/********************************************************************************************

>	BOOL Preferences::SetPrefValue(LPTCHAR Section, LPTCHAR Pref, UINT32 *PrefVar)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the unsigned integer variable that holds this
						  preference's value.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was found in the preference system successfully,
				FALSE otherwise.
	Purpose:	Finds the value of the specified preference which should already be defined in
				the preference system and then writes the new value specified into that 
				variable.
				This is used by interested parties other than the owner of that preference
				value e.g. the application options dialog box.
	
	Errors:		-
	SeeAlso:	Preferences::GetPrefValue

********************************************************************************************/

BOOL Preferences::SetPrefValue(LPTCHAR Section, LPTCHAR Pref, UINT32 *PrefVar)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section, error if not found and return FALSE
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Set up the data structure required to pass the new preference value
	// in with		
	PrefData Data;
	Data.pUInt = PrefVar;
	
	if (!pSection->SetPrefValue(OILPrefs, Pref, PREF_UINT, Data))
	{
		ERROR(_R(IDE_BAD_INI_FILE), FALSE);
		return FALSE; 						// Something went wrong
	}
#endif	
	return TRUE;
}
/********************************************************************************************

>	BOOL Preferences::SetPrefValue(LPTCHAR Section, LPTCHAR Pref, double *PrefVar)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Inputs:		Section - the case-insensitive name of the section in which this preference
						  should be placed, (e.g. "Printing").
				Pref	- the case-insensitive name of this preference 
						  (e.g. "AutoSaveInterval").
				PrefVar - pointer to the double variable that holds this preference's value.
	Outputs:	PrefVar - contains the value found in the preference file for this
						  preference.  If no such preference was found, the value is
						  unchanged.
	Returns:	TRUE if the preference was found in the preference system successfully,
				FALSE otherwise.
	Purpose:	Finds the value of the specified preference which should already be defined in
				the preference system and then writes the new value specified into that 
				variable.
				This is used by interested parties other than the owner of that preference
				value e.g. the application options dialog box.
	
	Errors:		-
	SeeAlso:	Preferences::GetPrefValue

********************************************************************************************/

BOOL Preferences::SetPrefValue(LPTCHAR Section, LPTCHAR Pref, double *PrefVar)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Find the right section, error if not found and return FALSE
	PreferenceSection *pSection = GetSection(Section);
	ERRORIF(!pSection, _R(IDE_BAD_INI_FILE), FALSE);

	// Set up the data structure required to pass the new preference value
	// in with		
	PrefData Data;
	Data.pDouble = PrefVar;
	
	if (!pSection->SetPrefValue(OILPrefs, Pref, PREF_DOUBLE, Data))
	{
		ERROR(_R(IDE_BAD_INI_FILE), FALSE);
		return FALSE; 						// Something went wrong
	}
#endif	
	return TRUE;
}


/********************************************************************************************

>	BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, const TCHAR *pValue, BOOL Force)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/95
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
				Force - TRUE => always write the specified preference out
					   FALSE => only write the preference if it doesn't already exist.
	Returns:	TRUE if written ok;
				FALSE if not.
	Purpose:	Write a preference to the preference file directly, without 'declaring'
				an associated preference.  This can be used just to ensure that a 
				default value for the preference is in the file - if Force is FALSE, then
				if the preference already exists it is not changed.
	SeeAlso:	Preferences::GetPrefDirect

********************************************************************************************/

BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, const TCHAR *pValue, BOOL Force)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// First check if there is a valid OILPrefs, as if there was a problem on start up such as
	// the MSCVRTn.DLL failing to load then the preferences may have been not fully set up.
	// Static variable used as the class may not have been created!
	if (PrefsInitedOk && (OILPrefs != NULL))
	{
		String_256 PrefValue;
		PrefData Data;
		Data.pString = &PrefValue;

		// Check to see if we need to change the preference
		if (!Force)
		{
			// First, try to read the preference
			OILPrefs->OpenInput();
			OILPrefs->Read(Section, Pref, PREF_STRING, Data);
			OILPrefs->CloseInput();

			if (Data.pString->Length() > 0)
				// Preference already exists - do nothing
				return TRUE;
		}

		// Perform any preparation necessary to start writing preferences
		OILPrefs->OpenOutput();

		// Write out the preference...
		PrefValue = pValue;
		OILPrefs->Write(Section, Pref, PREF_STRING, Data);

		// Perform any platform-deppy clean-up required after writing the preferences
		OILPrefs->CloseOutput();

		// All done
		return TRUE;
	}
#endif
	// Something went wrong
	return FALSE;
}

/********************************************************************************************

>	BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, PreferenceType Type, PrefData Data)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/97
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
	Returns:	TRUE if written ok;
				FALSE if not.
	Purpose:	Write a preference to the preference file directly, without 'declaring'
				an associated preference.  This can be used just to ensure that a 
				default value for the preference is in the file.
	SeeAlso:	Preferences::GetPrefDirect

********************************************************************************************/

BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, PreferenceType Type, PrefData Data)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// First check if there is a valid OILPrefs, as if there was a problem on start up such as
	// the MSCVRTn.DLL failing to load then the preferences may have been not fully set up.
	// Static variable used as the class may not have been created!
	if (PrefsInitedOk && (OILPrefs != NULL))
	{
		// Perform any preparation necessary to start writing preferences
		OILPrefs->OpenOutput();

		// Write out the preference...
		OILPrefs->Write(Section, Pref, Type, Data);

		// Perform any platform-deppy clean-up required after writing the preferences
		OILPrefs->CloseOutput();

		// All done
		return TRUE;
	}
#endif
	// Something went wrong
	return FALSE;
}

/********************************************************************************************

>	BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, INT32 *pValue)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/97
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
	Returns:	TRUE if written ok;
				FALSE if not.
	Purpose:	Write a preference to the preference file directly, without 'declaring'
				an associated preference.  This can be used just to ensure that a 
				default value for the preference is in the file.
	SeeAlso:	Preferences::GetPrefDirect

********************************************************************************************/

BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, INT32 *pValue)
{
	PrefData Data;
	Data.pInt = pValue;
	return SetPrefDirect(Section, Pref, PREF_INT, Data);
}

/********************************************************************************************

>	BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, UINT32 *pValue)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/97
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
	Returns:	TRUE if written ok;
				FALSE if not.
	Purpose:	Write a preference to the preference file directly, without 'declaring'
				an associated preference.  This can be used just to ensure that a 
				default value for the preference is in the file.
	SeeAlso:	Preferences::GetPrefDirect

********************************************************************************************/

BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, UINT32 *pValue)
{
	PrefData Data;
	Data.pUInt = pValue;
	return SetPrefDirect(Section, Pref, PREF_UINT, Data);
}

/********************************************************************************************

>	BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, double *pValue)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/97
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
	Returns:	TRUE if written ok;
				FALSE if not.
	Purpose:	Write a preference to the preference file directly, without 'declaring'
				an associated preference.  This can be used just to ensure that a 
				default value for the preference is in the file.
	SeeAlso:	Preferences::GetPrefDirect

********************************************************************************************/

BOOL Preferences::SetPrefDirect(LPTCHAR Section, LPTCHAR Pref, double *pValue)
{
	PrefData Data;
	Data.pDouble = pValue;
	return SetPrefDirect(Section, Pref, PREF_DOUBLE, Data);
}

/********************************************************************************************

>	BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, PreferenceType Type, PrefData Data)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/97
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
				BufSize - number of bytes in the buffer pointed to by pValue.
	Returns:	TRUE if the preference exists and was small enough to fit in the buffer.
				FALSE if the preference does not exist or could not fit in the buffer.
	Purpose:	Read a preference from the file directly, without the need to declare a
				preference beforehand.
	SeeAlso:	Preferences::SetPrefDirect

********************************************************************************************/

BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, PreferenceType Type, PrefData Data)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// First check if there is a valid OILPrefs, as if there was a problem on start up such as
	// the MSCVRTn.DLL failing to load then the preferences may have been not fully set up.
	// Static variable used as the class may not have been created!
	if (PrefsInitedOk && (OILPrefs != NULL))
	{
		// Try to read the preference
		OILPrefs->OpenInput();
		OILPrefs->Read(Section, Pref, Type, Data);
		OILPrefs->CloseInput();

		if (Type == PREF_STRING)
		{
			// See if anything was put into the string
			return (Data.pString->Length() > 0);
		}

		return TRUE;
	}
#endif
	// Something went wrong
	return FALSE;
}

/********************************************************************************************

>	BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, StringBase *pValue)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/01/95
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
				BufSize - number of bytes in the buffer pointed to by pValue.
	Returns:	TRUE if the preference exists and was small enough to fit in the buffer.
				FALSE if the preference does not exist or could not fit in the buffer.
	Purpose:	Read a preference from the file directly, without the need to declare a
				preference beforehand.
	SeeAlso:	Preferences::SetPrefDirect

********************************************************************************************/

BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, StringBase *pValue)
{
	// core moved into common function above 25/3/97
	PrefData Data;
	Data.pString = pValue;
	return GetPrefDirect(Section, Pref, PREF_STRING, Data);
}

/********************************************************************************************

>	BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, INT32 *pValue)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/97
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
				BufSize - number of bytes in the buffer pointed to by pValue.
	Returns:	TRUE if the preference exists and was small enough to fit in the buffer.
				FALSE if the preference does not exist or could not fit in the buffer.
	Purpose:	Read a preference from the file directly, without the need to declare a
				preference beforehand.
	SeeAlso:	Preferences::SetPrefDirect

********************************************************************************************/

BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, INT32 *pValue)
{
	PrefData Data;
	Data.pInt = pValue;
	return GetPrefDirect(Section, Pref, PREF_INT, Data);
}

/********************************************************************************************

>	BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, UINT32 *pValue)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/97
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
				BufSize - number of bytes in the buffer pointed to by pValue.
	Returns:	TRUE if the preference exists and was small enough to fit in the buffer.
				FALSE if the preference does not exist or could not fit in the buffer.
	Purpose:	Read a preference from the file directly, without the need to declare a
				preference beforehand.
	SeeAlso:	Preferences::SetPrefDirect

********************************************************************************************/

BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, UINT32 *pValue)
{
	PrefData Data;
	Data.pUInt = pValue;
	return GetPrefDirect(Section, Pref, PREF_UINT, Data);
}

/********************************************************************************************

>	BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, double *pValue)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/97
	Inputs:		Section, Pref - the preference to write.
				pValue - the value to write to the preference.
				BufSize - number of bytes in the buffer pointed to by pValue.
	Returns:	TRUE if the preference exists and was small enough to fit in the buffer.
				FALSE if the preference does not exist or could not fit in the buffer.
	Purpose:	Read a preference from the file directly, without the need to declare a
				preference beforehand.
	SeeAlso:	Preferences::SetPrefDirect

********************************************************************************************/

BOOL Preferences::GetPrefDirect(LPTCHAR Section, LPTCHAR Pref, double *pValue)
{
	PrefData Data;
	Data.pDouble = pValue;
	return GetPrefDirect(Section, Pref, PREF_DOUBLE, Data);
}

