// $Id: strkcomp.cpp 662 2006-03-14 21:31:49Z alex $
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
// Stroke Doc-component classes.
// These store brush-stroke definitions as clipart subtrees

#include "camtypes.h"

#include "strkcomp.h"

#include "app.h"
#include "camfiltr.h"
//#include "colormgr.h"
#include "cxfrec.h"
#include "cxfrech.h"
#include "cxftags.h"
#include "fixmem.h"
#include "layer.h"
#include "ppvecstr.h"
//#include "sgline.h"
#include "spread.h"
#include "strkattr.h"
#include "linecomp.h"
#include "linedef.h"


CC_IMPLEMENT_DYNAMIC(StrokeDefinition,		LineDefinition);
CC_IMPLEMENT_DYNAMIC(StrokeComponent,		LineComponent)
CC_IMPLEMENT_DYNAMIC(StrokeComponentClass,	DocComponentClass)


DECLARE_SOURCE("$Revision: 662 $");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


// Statics
StrokeDefinition **StrokeComponent::pStrokeList = NULL;
UINT32 StrokeComponent::CurrentSize = 0;
UINT32 StrokeComponent::Used = 0;

UINT32 StrokeComponent::ImportHandle	= 0;
UINT32 StrokeComponent::ImportFlags	= 0;
UINT32 StrokeComponent::ImportData1	= 0;
UINT32 StrokeComponent::ImportData2	= 0;
InsertTreeContext *StrokeComponent::pImportPreviousContext = NULL;
Node *StrokeComponent::pImportNewBrush = NULL;
String_32 StrokeComponent::ImportedName;




/********************************************************************************************

>	StrokeDefinition::StrokeDefinition(Node *pStrokeTree, BOOL repeating = FALSE, INT32 repeats = 0);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/97

	Inputs:		pStrokeTree - A pointer to a Spread node which is the root of a clipart
							  subtree which should be used for this stroke definition. It
							  must obey these rules:
								* It must be a Spread
								* It must not be linked into any other parent tree structure
								* It should contain at least one path node (or else the stroke
								  will appear "blank"). You should do Make Shapes on the 
								  subtree so that text, blends, quickshapes, etc are all in
								  path form
								* It should be attribute complete, or close thereto

				repeating	- TRUE if this stroke definition should repeat along the stroke
							  rather than being stretched to fit.

				repeats		- The number of times the brush should repeat along the stroke, or
							  zero for 'work the best out at rendertime'

	Purpose:	Constructor

	SeeAlso:	StrokeComponent::AddNewStroke

********************************************************************************************/

StrokeDefinition::StrokeDefinition(Node *pStrokeTree, BOOL repeating, INT32 repeats) 
{
	ERROR3IF(pStrokeTree == NULL, "Illegal NULL param");
	ERROR3IF(!pStrokeTree->IsSpread(), "StrokeDefinitions must begin with a Spread (for now, at least)");
	ERROR3IF(pStrokeTree->FindParent() != NULL, "Stroke Definition looks like it's linked into a tree!");

	Repeating = repeating;
	Repeats   = repeats;
	pStroke	  = pStrokeTree;

	// Check the subtree to see if it contains any transparency
	NeedsTrans = pStroke->ChildrenNeedTransparency();

	IOStore = 0;

	Name = TEXT("Custom brush");

	OverridesFill  = FALSE;
	OverridesTrans = FALSE;
}



/********************************************************************************************

>	StrokeDefinition::~StrokeDefinition()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/97

	Purpose:	Destructor

********************************************************************************************/

StrokeDefinition::~StrokeDefinition()
{
	if (pStroke	!= NULL)
	{
		pStroke->CascadeDelete();
		delete pStroke;
	}
}



/********************************************************************************************

>	void StrokeDefinition::SetStrokeName(StringBase *pName)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97

	Purpose:	Sets the name text string for this stroke

********************************************************************************************/

void StrokeDefinition::SetStrokeName(StringBase *pName)
{
	ERROR3IF(pName == NULL, "Illegal NULL param");

	pName->Left(&Name, 31);
}


/********************************************************************************************

>	void StrokeDefinition::SetStrokeRepeating(BOOL Repeating)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/97

	Purpose:	Sets the repeating flag for this stroke

********************************************************************************************/

void StrokeDefinition::SetStrokeRepeating(BOOL SetMeToTrueIfYouReallyWantMeToBeRepeating)
{
	// TRUE if the stroke should repeat along the path, FALSE if it should stretch
	Repeating = SetMeToTrueIfYouReallyWantMeToBeRepeating;
}


/********************************************************************************************

>	void StrokeDefinition::SetNumStrokeRepeats(INT32 NumRepeats)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/3/97

	Purpose:	Sets the number of repeats for this stroke

********************************************************************************************/

void StrokeDefinition::SetNumStrokeRepeats(INT32 NumRepeats)
{
	// Number of times a brush should repeat along a stroke, or zero to just work out the
	// optimal value on the fly
	Repeats = NumRepeats;
}

/********************************************************************************************

>	BOOL StrokeDefinition::IsDifferent(StrokeDefinition *pOther)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/97

	Inputs:		pOther - the stroke to compare this stroke to

	Returns:	TRUE if they're different in any way,
				FALSE if they are identical definitions

	Purpose:	Determine if 2 StrokeDefinitions are considered different.
				Used when adding strokes to the global list, so that like strokes
				can be merged.

********************************************************************************************/

BOOL StrokeDefinition::IsDifferent(LineDefinition *pTest)
{
	ERROR3IF(pTest == NULL, "Illegal NULL param");

	// check to make sure that pOther is in fact a StrokeDef so that
	// we can recast it
	StrokeDefinition* pOther = NULL;
	if (pTest->IS_KIND_OF(StrokeDefinition))
		pOther = (StrokeDefinition*)pTest;
	else
	{
		ERROR3("Parameter is not a stroke definition");
		return TRUE;
	}

	if (pOther->Repeating != Repeating)
		return(TRUE);

	if (pOther->Repeats != Repeats)
		return(TRUE);

	if (!Name.IsIdentical(pOther->Name))
		return(TRUE);

	if (pOther->pStroke == NULL || pStroke == NULL)
	{
		ERROR3("StrokeDefinition has not been properly initialised");
		return(TRUE);
	}

	// --- Check to see if the brush bounds are equal
	DocRect OtherBounds = ((Spread *)(pOther->pStroke))->GetBoundingRect();
	DocRect Bounds = ((Spread *)pStroke)->GetBoundingRect();
	if (Bounds != OtherBounds)
		return(TRUE);

	// --- Check the subtrees node-for-node to see if they are the same
	Node *pCurNode1 = pStroke->FindFirstDepthFirst();
	Node *pCurNode2 = pOther->pStroke->FindFirstDepthFirst();

	while (pCurNode1 != NULL && pCurNode2 != NULL)
	{
		// See if the nodes are equivalent - if not, we can return immediately
		if (pCurNode1->IsDifferent(pCurNode2))
			return(TRUE);

		// And go to the next node in both brushes
		pCurNode1 = pCurNode1->FindNextDepthFirst(pStroke);
		pCurNode2 = pCurNode2->FindNextDepthFirst(pOther->pStroke);
	}

	// If we did the entire search and both pointers ended up NULL simultaneously, then
	// we have an exact match
	if (pCurNode1 == NULL && pCurNode2 == NULL)
		return(FALSE);

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL StrokeDefinition::NeedsTransparency() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97

	Returns:	TRUE if this stroke needs transparency in order to render

	Purpose:	Determine if this stroke needs transparency in order to render.

********************************************************************************************/

BOOL StrokeDefinition::NeedsTransparency() const
{
	return(NeedsTrans);
}










/********************************************************************************************

>	BOOL StrokeComponentClass::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Register the Stroke document component with the main application.
	Errors:		Out of memory.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL StrokeComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	StrokeComponentClass *pClass = new StrokeComponentClass;
	if (pClass == NULL)
		return(FALSE);

	// Register it
	GetApplication()->RegisterDocComponent(pClass);
	return(TRUE);
}



/********************************************************************************************

>	void StrokeComponentClass::DeInit()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97

	Returns:	TRUE if all went well;
				FALSE if not.

	Purpose:	De-initialises the vector stroke provider system

********************************************************************************************/

void StrokeComponentClass::DeInit()
{
	StrokeComponent::DeleteStrokeList();
}



/********************************************************************************************

>	BOOL StrokeComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97
	Inputs:		pDocument - the document to add the print to.
	Returns:	TRUE if the Stroke component was added ok;
				FALSE if not.
	Purpose:	Add a Stroke component to the specified document.
	Errors:		Out of memory
	SeeAlso:	PrintComponentClass

********************************************************************************************/

BOOL StrokeComponentClass::AddComponent(BaseDocument *pDocument)
{
	ERROR2IF(pDocument==NULL, FALSE, "NULL document passed to StrokeCompClass:Add");

	// Check to see if this document already has a colour list; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(StrokeComponent)) != NULL)
		return(TRUE);

	// Ok - create the print mark component.
	StrokeComponent *pComponent = new StrokeComponent();
	if (pComponent == NULL)
		return(FALSE);

	// All ok - add the component to the document.
	pDocument->AddDocComponent(pComponent);
	return(TRUE);
}











/********************************************************************************************

>	StrokeComponent::StrokeComponent()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97

	Purpose:	Constructor

********************************************************************************************/

StrokeComponent::StrokeComponent()
{
}



/********************************************************************************************

>	StrokeComponent::~StrokeComponent()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97

	Purpose:	Destructor

********************************************************************************************/

StrokeComponent::~StrokeComponent()
{
}



/********************************************************************************************

>	static void StrokeComponent::DeleteStrokeList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97

	Purpose:	Static deinitialisation function, called on shutdown.
				Clears out the global stroke list. Must be called when all users of the
				stroke system (documents) have already been deleted.

********************************************************************************************/

void StrokeComponent::DeleteStrokeList(void)
{
	if (pStrokeList != NULL)
	{
		for (UINT32 i = 0; i < Used; i++)
		{
			if (pStrokeList[i] != NULL)
				delete pStrokeList[i];

			pStrokeList[i] = NULL;
		}

		CCFree(pStrokeList);
		pStrokeList = NULL;
	}
}



/********************************************************************************************

>	static StrokeHandle StrokeComponent::AddNewStroke(StrokeDefinition *pStroke)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/2/97

	Inputs:		pStroke - (may not be NULL) A stroke definition to add, which now
						  belongs to the StrokeComponent, so you mustn't delete it!

	Returns:	A handle which uniquely identifies the new stroke, or
				StrokeHandle_NoStroke if we ran out of memory for storing strokes

	Purpose:	Adds the given stroke to the global stroke list.

	Notes:		IMPORTANT!
				The stroke definition you pass in belongs to the StrokeComponent,
				and will be deleted when finished with. 

				YOU MUST NOT use the pStroke pointer after this call! If the call
				fails, or if the stroke is merged with an existing one, the object
				you passed in will be IMMEDIATELY DELETED! If you wish to use the
				stroke after Adding it, you must call FindStroke with the returned
				stroke handle!

	Technical:	Strokes are recorded in an array of StrokeDefinition pointers.
				The StrokeHandle is just an index into this array, for fast lookup.
				When deleted, the array-entry is set NULL and never re-used, so that
				any handles floating around can be safely used (much better than
				getting IndexedColour deleted when in use errors, eh? ;-).
				New entries are thus always appended to the list.

********************************************************************************************/

StrokeHandle StrokeComponent::AddNewStroke(StrokeDefinition *pStroke)
{
	// ****!!!!TODO - if we're multi-threadig, this probably needs to be a critical section
	// because the list is global

	ERROR3IF(pStroke == NULL, "Illegal NULL param");

	// --- First, check if we can merge the given stroke with one already in the list
	for (UINT32 i = 0; i < Used; i++)
	{
		if (pStrokeList[i] != NULL && !pStroke->IsDifferent(pStrokeList[i]))
		{
			// The stroke is the same as one we already have in our stroke list, so we
			// delete the copy that was passed in, and return the existing ones handle
			// We copy the import handle value over, though, so that we don't "lose"
			// merged strokes during import!
			pStrokeList[i]->SetIOStore(pStroke->ReadIOStore());
			delete pStroke;
			return((StrokeHandle) i);
		}
	}

	// --- Next, check if we have enough room in our stroke array to add a new entry to the end
	if (Used >= CurrentSize)
	{
		if (!ExpandArray())
		{
			delete pStroke;
			return(StrokeHandle_NoStroke);
		}
	}

	// --- OK, we have a totally new stroke, so add it to the end of the list
	StrokeHandle NewHandle = (StrokeHandle) Used;
	pStrokeList[Used] = pStroke;
	Used++;

	// --- Now, make sure the stroke gallery display updates to show the new stroke
	// ****!!!!TODO - should notify the stroke gallery (broadcast a msg) so that it updates
	// I'll use a nasty bodgy direct call to the line gallery instead... eeek!
	{
		PathProcessorStrokeVector *pProcessor = new PathProcessorStrokeVector;
		if (pProcessor != NULL)
		{
			pProcessor->SetStrokeDefinition(NewHandle);

PORTNOTE("other","Removed LineGallery usage")
#ifndef EXCLUDE_FROM_XARALX
			StrokeTypeAttrValue *pStrokeAttr = new StrokeTypeAttrValue(pProcessor);
			if (pStrokeAttr != NULL)
				LineGallery::AddNewStrokeItem(pStrokeAttr);
			else
				delete pProcessor;
#endif
		}
	}

	// --- And return the new handle
	return(NewHandle);
}



/********************************************************************************************

>	static StrokeDefinition *StrokeComponent::FindStroke(StrokeHandle Handle);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/2/97

	Inputs:		Handle - A handle which uniquely identifies the new stroke

	Returns:	A pointer to a StrokeDefinition for that Stroke.
				If the stroke has been deleted, NULL will be returned, in which case,
				you should gracefully degrade (by rendering an old style line in place of
				the stroke)

	Purpose:	Find a stroke, given its unique identity handle

********************************************************************************************/

StrokeDefinition *StrokeComponent::FindStroke(StrokeHandle Handle)
{
	if (Handle == StrokeHandle_NoStroke)
		return(NULL);

	if (Handle >= Used)
	{
		ERROR3("Out of range stroke handle");
		return(NULL);
	}

	return(pStrokeList[Handle]);
}



/********************************************************************************************

>	BOOL StrokeComponent::StartImport(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)

	Purpose:	Called before we start to import a file

	SeeAlso:	DocComponent

********************************************************************************************/

BOOL StrokeComponent::StartImport(BaseCamelotFilter *pFilter)
{
	if (pFilter == NULL)
	{
		ERROR3("StrokeComponent::StartImport filter is null!");
		return(TRUE);
	}

	// Set the IOStore in all the strokes to 0xffffffff. During import, this entry is used
	// to map imported handles to the real handles they have been assigned.
	if (pStrokeList != NULL)
	{
		for (UINT32 Index = 0; Index < Used; Index++)
		{
			if (pStrokeList[Index] != NULL)
				pStrokeList[Index]->SetIOStore(0xffffffff);
		}
	}

	return(TRUE);
}



/********************************************************************************************

>	BOOL StrokeComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.

	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)

	Purpose:	Called on completion of an import

********************************************************************************************/

BOOL StrokeComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
	return TRUE;
}



/********************************************************************************************

>	BOOL StrokeComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)

	Purpose:	Called before an export is started

********************************************************************************************/

BOOL StrokeComponent::StartExport(BaseCamelotFilter *pFilter)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (pFilter == NULL)
	{
		ERROR3("StrokeComponent::StartExport filter is null!");
		return(TRUE);
	}
	TRACEUSER( "Diccon", _T("Start Export Stroke Component\n"));
	// ****!!!!TODO - if we're multi-threadig, this probably needs to be a critical section
	// because the list is global

	// Flag all strokes as not having been saved. When we save one, we set its flag so that
	// we don't try to save it a second time.
	if (pStrokeList != NULL)
	{
		for (UINT32 Index = 0; Index < Used; Index++)
		{
			if (pStrokeList[Index] != NULL)
				pStrokeList[Index]->SetIOStore(FALSE);
		}

		// Write out an atomic tag definition in front of the vector stroke definition.
		// ****!!!!TODO - This should really only be done just before we export the first
		// STROKEDEFINITION tag, but it is not properly supported by the export system as yet.
		BOOL ok = TRUE;
		CXaraFileRecord Rec(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
		if (ok) ok = Rec.Init();
		if (ok) ok = Rec.WriteUINT32(TAG_STROKEDEFINITION);
		if (ok)	pFilter->Write(&Rec);		// And write out the record
	}
#endif
	return(TRUE);
}



/********************************************************************************************

>	BOOL StrokeComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - True if everything went swimmingly, False if just a clean up is required.

	Returns:	TRUE if the component was able to end the exporting;
				FALSE if not (e.g. out of memory)

	Purpose:	Called on completion of file export

********************************************************************************************/

BOOL StrokeComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
	BOOL ok = TRUE;
	TRACEUSER( "Diccon", _T("End Export Stroke Component\n"));
#if !defined(EXCLUDE_FROM_RALPH)
	if (pFilter == NULL)
	{
		ERROR3("StrokeComponent::EndExport filter is null!");
		return(ok);
	}

	if (!Success)		// If we failed to export, then return immediately
		return(ok);

#endif
	return(ok);
}



/********************************************************************************************

>	static BOOL ExportStroke(BaseCamelotFilter *pFilter, StrokeHandle Handle)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
				Handle  - The stroke to be exported

	Returns:	TRUE if export was successful
				FALSE if export was aborted - no error is set, as in this case, it usually
				means that the stroke has been deleted, and is being treated as an old-style
				line. In this case, the caller should simply not bother exporting the
				attribute using the stroke definition.

	Purpose:	To export a vector stroke definition. 

	Notes:		Stroke definitions (like colours) are only saved out when a node in the
				tree is found which makes use of the stroke. You should call this function
				before exporting any attribute which uses the stroke definition. It
				automatically checks if the stroke has already been saved, and will not save
				the definition more than once.

				When saving your reference to the stroke, save out the stroke's Handle
				as it's unique ID word.

********************************************************************************************/

BOOL StrokeComponent::ExportStroke(BaseCamelotFilter *pFilter, StrokeHandle Handle)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL params");
	TRACEUSER( "Diccon", _T("Exporting Stroke\n"));
	// Find the stroke, and baulk if it's all gone wrong
	StrokeDefinition *pStroke = FindStroke(Handle);
	if (pStroke == NULL)
	{
		ERROR3("Attempt to save a deleted or non-existent stroke");
		return(FALSE);
	}

	// Check if the definition has already been saved, in which case we don't need to do anything more
	if (pStroke->ReadIOStore())
		return(TRUE);

	// We've got a stroke definition now, so let's chuck it out to the file
	BOOL ok = TRUE;
	CXaraFileRecord Rec(TAG_STROKEDEFINITION, TAG_STROKEDEFINITION_SIZE);
	if (ok) ok = Rec.Init();

	if (ok) ok = Rec.WriteUINT32(Handle);

	UINT32 Flags = 0x200;				// All vector strokes include a name (Named flag == 0x200)
	if (ok)
	{
		// Write the flags out.
		if (pStroke->IsRepeating())		// Repeating flag = 0x100
			Flags |= 0x100;

		if (pStroke->OverrideFill())	// Fill override flag = 0x400
			Flags |= 0x400;

		if (pStroke->OverrideTrans())	// Transparency override flag = 0x800
			Flags |= 0x800;

		ok = Rec.WriteUINT32(Flags);
	}
	if (ok)
	{
		INT32 NumRepeats = 0;
		if(pStroke->IsRepeating())
		{
			// Write the flags out.
			NumRepeats = pStroke->NumRepeats();

			if(NumRepeats == 0)
				Flags |= 1;
			else
				Flags |= 2;
		}

		// Write Data1 - this contains the number of times the brush should repeat down the stroke
		ok = Rec.WriteUINT32(NumRepeats);		
	}

	if (ok) ok = Rec.WriteUINT32(0);		// Write Data2 - for now, this is always 0

	if (ok && ((Flags & 0x200) != 0))	// Write the (optional) name field
		ok = Rec.WriteUnicode((TCHAR *)*(pStroke->GetStrokeName()));

	if (ok)	pFilter->Write(&Rec);		// And write out the record

	// Now, follow the definition record with the brush subtree
	if (ok)
	{
		Node *pNode = pStroke->GetStrokeTree();	// Find the root Spread
		if (pNode != NULL)
			pNode = pNode->FindFirstChild();	// Find the Layer
		if (pNode != NULL)
			pNode = pNode->FindFirstChild();	// Find the NodeGroup

		if (pNode == NULL)
			ok = FALSE;

		ERROR3IF(pNode == NULL || !pNode->IsCompound(), "Vector brush not in the format I expected");

		// Write out the clipart subtree. We have to encapsulate it in DOWN and UP
		// records ourselves
		CXaraFileRecord DownRec(TAG_DOWN, 0);
		if (ok)	pFilter->Write(&DownRec);

		ok = pFilter->WriteNodes(pNode);

		CXaraFileRecord UpRec(TAG_UP, 0);
		if (ok)	pFilter->Write(&UpRec);
	}

	// Finally, mark this stroke as having been written out so we don't do it again
	if (ok) pStroke->SetIOStore(TRUE);

	return(ok);
}



/********************************************************************************************

>	static BOOL ImportStroke(CXaraFileRecord* pRecord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97

	Inputs:		pRecord - The TAG_STROKEDEFINITION record to import

	Returns:	TRUE if Import was successful

	Purpose:	To import a vector stroke definition

********************************************************************************************/

BOOL StrokeComponent::StartImportStroke(CamelotRecordHandler *pHandler, CXaraFileRecord *pRecord)
{
	ImportHandle = 0xffffffff;
	ImportFlags  = 0;
	ImportData1  = 0;
	ImportData2  = 0;
	pImportPreviousContext = NULL;
	pImportNewBrush = NULL;

	String_256 TempName;	// Load the name into a nice large safe buffer

	// Import the data from the record
	if (!pRecord->ReadUINT32(&ImportHandle) || !pRecord->ReadUINT32(&ImportFlags) ||
		!pRecord->ReadUINT32(&ImportData1) || !pRecord->ReadUINT32(&ImportData2))
	{
		return(FALSE);
	}

	// If there is a name to be imported, read it as well
	if ((ImportFlags & 0x200) != 0 && !pRecord->ReadUnicode(&TempName))
		return(FALSE);

	// We remember the imported flags and suchlike, which we will use in EndImportStroke.
	TempName.Left((StringBase *)&ImportedName, 31);

	// Create a spread and set up the import system to import the brush into it
	// If this fails, then it'll just find somewhere "sensible" to import into
	pImportNewBrush = new Spread;
	if (pImportNewBrush == NULL)
		return(FALSE);

	Layer *pBrushLayer = new Layer(pImportNewBrush, FIRSTCHILD, String_256(TEXT("Jason did this")));
	if (pBrushLayer == NULL)
	{
		delete pImportNewBrush;
		pImportNewBrush = NULL;
		return(FALSE);
	}

	// Now, remember where we were importing into, and point the importer at our brush tree
	pImportPreviousContext = pHandler->GetInsertContext();
	pHandler->SetInsertContextNode(pBrushLayer);
	return(TRUE);
}



/********************************************************************************************

>	static BOOL StrokeComponent::EndImportStroke(CamelotRecordHandler *pHandler);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97

	Inputs:		pRecord - The TAG_STROKEDEFINITION record to import

	Returns:	TRUE if Import was successful

	Purpose:	To import a vector stroke definition

********************************************************************************************/

BOOL StrokeComponent::EndImportStroke(CamelotRecordHandler *pHandler)
{
	ERROR3IF(pImportPreviousContext == NULL, "EndImportStroke - something hasn't gone too well");

	// Restore the previous import context node
	pHandler->RestoreInsertContext(pImportPreviousContext);
	pImportPreviousContext = NULL;

	// --- Now, convert all IndexedColours (which are document-dependent) into standalone DocColours
	// This wouldn't be necessary, except that the DocColours we saved have magically been turned
	// back into local Indexedcolours by the export/import process.
	// BLOCK
	{
		Node *pCurNode = pImportNewBrush->FindFirstDepthFirst();
		Node *pNextNode;

		while (pCurNode !=NULL)
		{
			// We may be about to chop this node out of the tree, so get the next node now
			pNextNode = pCurNode->FindNextDepthFirst(pImportNewBrush);

			// Use to scan the colour fields of the attribute.
			if (pCurNode->IsAnAttribute())
			{

PORTNOTE("colourmanager","Removed ColourManager usage")
#ifndef EXCLUDE_FROM_XARALX
				UINT32 Context = 0;
				NodeAttribute *pNodeAttr = (NodeAttribute *) pCurNode;
				// Get the next colour field from the attribute
				DocColour *pColour = pNodeAttr->EnumerateColourFields(Context++);

				while (pColour != NULL)
				{
					// For each colour field, make sure the colour is a local DocColour so that
					// the sub-tree is entirely stand-alone
					if (pColour->FindParentIndexedColour() != NULL)
					{
						ColourGeneric ColDef;
						ColourContext *cc = ColourManager::GetColourContext(pColour->GetColourModel());
						ERROR3IF(cc == NULL, "Can't find colour context?!");

						// Get the IndexedColour definition as a standalone colour definition
						cc->ConvertColour(pColour->FindParentIndexedColour(), &ColDef);

						// Make the DocColour into a simple standalone "lookalike" of the parent colour
						*pColour = DocColour(pColour->GetColourModel(), &ColDef);
					}

					pColour = pNodeAttr->EnumerateColourFields(Context++);
				}
#endif
			}
			pCurNode = pNextNode;
		}
	}

	// Create a new stroke definition from the imported brush
	// Read the imported data from ImportFlags, ImportData1, ImportData2
	// (Currently, we only handle the Repeating, Optimal Repeating and Number of Repeating flags)
	BOOL Repeating = ((ImportFlags & 0x100) != 0);

	INT32 Repeats = 0;

	// If repeating a certain number of times, get the number of times, else assume 0 - optimal
	if((ImportFlags & 0xff) == 2)
		Repeats = (INT32)ImportData1;

	StrokeDefinition *pNewStroke = new StrokeDefinition(pImportNewBrush, Repeating, Repeats);
	if (pNewStroke == NULL)
		return(FALSE);

	pNewStroke->SetOverrideFill((ImportFlags & 0x400) != 0);
	pNewStroke->SetOverrideTrans((ImportFlags & 0x800) != 0);

	// Remember the handle used for the stroke in the file with the stroke so that
	// we can map any incoming handles into the real handles we are using internally.
	pNewStroke->SetIOStore(ImportHandle & 0x00ffffff);

	// And set the stroke's name, if any was supplied
	if (!ImportedName.IsEmpty())
		pNewStroke->SetStrokeName(&ImportedName);

	// And add/merge this stroke into our global list
	AddNewStroke(pNewStroke);
	return(TRUE);
}



/********************************************************************************************

>	static StrokeHandle StrokeComponent::FindImportedStroke(UINT32 ImportedHandle)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97

	Inputs:		Importedhandle - The handle which is used in the file being imported
								 to reference the stroke.

	Returns:	The StrokeHandle of the loaded stroke, or StrokeHandle_NoStroke
				if the stroke could not be found.

	Purpose:	To match up a handle from the currently importing file to the
				real internal StrokeHandle of the imported StrokeDefinition

********************************************************************************************/

StrokeHandle StrokeComponent::FindImportedStroke(UINT32 ImportedHandle)
{
	ImportedHandle &= 0x00ffffff;	// Only the bottom 24 bits are relevant
	for (UINT32 Index = 0; Index < Used; Index++)
	{
		if (pStrokeList[Index] != NULL && pStrokeList[Index]->ReadIOStore() == ImportedHandle)
			return((StrokeHandle)Index);
	}

	return(StrokeHandle_NoStroke);
}



/******************************************************************************************

>	static BOOL StrokeComponent::ExpandArray(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97

	Outputs:	On succesful exit, the member array of StrokeDefinition pointers will be bigger
	Returns:	FALSE if it failed to allocate memory

	Purpose:	(Internal method)
				Expands the storage structure of the stroke list to allow more entries to be
				used. Called automatically by AddNewStroke as necessary.

	Notes:		Internal storage is an array of pointers to StrokeDefinitions
				NULL pointers beyond (& including) "Used" indicate free slots.
				NULL pointers before "Used" indicate deleted strokes - these slots
				should NOT be re-used, as there may still be references to them in
				the system.

******************************************************************************************/

BOOL StrokeComponent::ExpandArray(void)
{
	// ****!!!!TODO - if we're multi-threading, this probably needs to be a critical section
	// because the list is global

	const INT32 AllocSize = CurrentSize + 64;

	if (pStrokeList == NULL)
	{
		pStrokeList = (StrokeDefinition **) CCMalloc(AllocSize * sizeof(StrokeDefinition *));
		if (pStrokeList == NULL)
			return(FALSE);
	}
	else
	{
		// We have an array - we must make it larger
		StrokeDefinition **pNewBuf = (StrokeDefinition **)
									CCRealloc(pStrokeList, AllocSize * sizeof(StrokeDefinition *));
		if (pNewBuf == NULL)
			return(FALSE);

		pStrokeList = pNewBuf;
	}

	// Success. Initalise all the new pointers to NULL
	for (UINT32 i = Used; i < CurrentSize; i++)
		pStrokeList[i] = NULL;

	// Update the current size value, and return success
	CurrentSize = AllocSize;
	return(TRUE);
}

