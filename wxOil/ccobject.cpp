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

/*
*/

#include "camtypes.h"

#if defined( __WXMSW__ )
#include <new.h>
#elif defined( __WXMAC__ )
#include <new>
#endif

#include "ccobject.h"

#include "ensure.h"
#include "app.h"
#include "errors.h"
//#include "resource.h"


#ifdef _DEBUG
BOOL SimpleCCObject::CheckMemoryFlag = FALSE;
#endif
	
/////////////////////////////////////////////////////////////////////////////
// SimpleCCObject


/********************************************************************************************

<	struct MemTrackExtra

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/94
	Purpose:	Holds the info that we use in the memory tracking code. It is a doubly linked
				list that tells us how big a block is, where it was allocated etc

********************************************************************************************/

typedef struct MemTrackExtra
{
	UINT32 GuardWord;
	struct MemTrackExtra* pNext;
	struct MemTrackExtra* pPrev;
	size_t Size;
	void*  Start;
	char   Filename[22];
	INT32	   LineNum;
} MemTrackExtra;

#ifdef _DEBUG
// a global pointer to the first item in the memory tracking list
static MemTrackExtra* pHead = NULL;
#endif

// some constants used by the memory tracker
#define GUARDWORD	0xEFBEADDE		// DEADBEEF with the byte in intel order
#define DEADGUARD	0xEEFFC000 		// 00C0FFEE with the byts in intel order
#define NEWLANDFILL	0xCD			// The value to fill new block with
#define OLDLANDFILL	0xAF			// The value to fill blocks with as they are deleted 

//#define _OUTPUT_DEBUG				// Comment this out to disable debug messages during allocation
//#define _OUTPUT_DEBUG_TRACKING	// Comment this out to disable debug messages about allocation

#ifdef _OUTPUT_DEBUG
	static INT32 TotalMemAllocated = 0;		// For tracing memory allocations
#endif

// Static declarations for low memory test
#ifdef _DEBUG
	size_t SimpleCCObject::BytesBeforeFail = 0;
	BOOL SimpleCCObject::FailingMemory = FALSE;
#endif

// set this to 1 for structure integrity checks on news/deletes. Defaults to using
// the MFC flag for the same purpose
//#define	CHECK_MEMORY	( afxMemDF & checkAlwaysMemDF )
#ifdef _DEBUG
#define	CHECK_MEMORY	(SimpleCCObject::CheckMemoryFlag)
#else
#define	CHECK_MEMORY	0
#endif

/********************************************************************************************

>	BOOL SimpleCCObject::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/4/94
	Returns:	TRUE if it worked OK, FALSE if not
	Purpose:	Declares a preference that allows you to clear memory in delete().

********************************************************************************************/

BOOL SimpleCCObject::Init()
{
	// return TRUE as we do not want Camelot to fail to start over this
	return TRUE;
}


#ifdef _DEBUG
/********************************************************************************************
>	BOOL SimpleCCObject::CheckMemory( BOOL Assert )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Purpose:	Checks the CCObject memory structures, returns TRUE if OK, FALSE if
				broken.
				If Assert is TRUE then asserts within the function, allowing deeper investigation.
				Only exists in debug builds.
********************************************************************************************/
BOOL SimpleCCObject::CheckMemory( BOOL Announce )
{
	char *WhyFailed = NULL;
	MemTrackExtra *Cur = pHead;

	while (Cur)
	{
		// check guardword
		if (Cur->GuardWord != GUARDWORD)
		{
			WhyFailed = "Guardword missing";	// Debug
			break;
		}

		// get the next one
		MemTrackExtra *Next = Cur->pNext;

		// check the back pointer
		if (Next)
		{
			if (Next->pPrev != Cur)
			{
				WhyFailed = "Previous pointer missing";	// Debug
				break;
			}
		}

		// try the next block
		Cur = Next;
	}

	// did it work or not?

	if (WhyFailed)
	{
		if (Announce)
			ERROR2RAW( WhyFailed );
		return FALSE;
	}
	else
		return TRUE;
}



/********************************************************************************************
>	static void SimpleCCObject::EnableLowMemoryTesting()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/96
	Purpose:	Turns on the low memory simluator.  After this memory allocation will fail
				Debug builds only, obviously.
	SeeAlso:	SimpleCCObject::DisableLowMemoryTesting
				SimpleCCObject::SetClaimLimit
********************************************************************************************/
void SimpleCCObject::EnableLowMemoryTesting()
{
	FailingMemory = TRUE;
	TRACE( _T("Low memory simulation ENABLED\n"));
}



/********************************************************************************************
>	static void SimpleCCObject::DisableLowMemoryTesting()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/96
	Purpose:	Turns off the low memory simluator.  After this memory allocation will work
				Debug builds only, obviously.
	SeeAlso:	SimpleCCObject::EnableLowMemoryTesting
				SimpleCCObject::SetClaimLimit
********************************************************************************************/
void SimpleCCObject::DisableLowMemoryTesting()
{
	FailingMemory = FALSE;
	TRACE( _T("Low memory simulation DISABLED\n"));
}



/********************************************************************************************
>	static void SimpleCCObject::SetClaimLimit()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/96
	Purpose:	Sets the number of bytes that will be allocated before memory claims fail.
				You must call EnableLowMemoryTesting before claims will fail.
				Debug builds only, obviously.
	SeeAlso:	SimpleCCObject::DisableLowMemoryTesting
				SimpleCCObject::EnableLowMemoryTesting
********************************************************************************************/
void SimpleCCObject::SetClaimLimit(UINT32 BytesToAllow)
{
	BytesBeforeFail	= BytesToAllow;
	TRACE( _T("Low memory simulation will fail all claims after %d bytes\n") ,BytesBeforeFail);
}

#endif



/********************************************************************************************

>	void SimpleCCObject::MemoryTrackAdd(void* p, LPCSTR FileName, INT32 Line)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/93
	Inputs:		p - pointer to the block of memory that was allocated
				FileName - pointer to the string containing the filename
				Line - The line at which the allocation happened
	Purpose:	This functions adds an item into the memory tracking list
				and stores the filename and line number if it is passed in.
				Both versions of new call this function to handle all there
				DEBUG stuff.

********************************************************************************************/

#ifdef _DEBUG
void SimpleCCObject::MemoryTrackAdd(void* pStart, LPCSTR FileName, INT32 Line)
{
	// Find out how much memory malloc actually gave us
#if !defined(__WXMAC__)
	size_t				BlockLen = _msize(pStart);
#else
	size_t				BlockLen = malloc_size(pStart);
#endif
	
	// Fill the block with the Land Fill value
	memset(pStart, NEWLANDFILL, BlockLen);

	// Find the start of my strange structure
	MemTrackExtra* pExtraInfo = (MemTrackExtra*) pStart;

	// fill the structure with the extra data
	pExtraInfo->GuardWord = GUARDWORD;
	pExtraInfo->Size = BlockLen;
	pExtraInfo->Start = ((char*)pStart + sizeof(MemTrackExtra));

	// Fill in the details about the file position
	if (FileName!=NULL)
	{	
		// copy the string passed in and note the line number
		size_t			Count = strlen( FileName );
		if (Count>21)
		{
			// The string was too long, so we have to copy just the last 22 chars in it
			FileName += Count - 21;
		}

		// copy the end of the string into the space I have for it
		strcpy( pExtraInfo->Filename, FileName );
		pExtraInfo->LineNum = Line;
	}
	else
	{
		// set the fields to null values
		strcpy( pExtraInfo->Filename, "Unknown File" );		// Debug only by the smell of things
		pExtraInfo->LineNum = 0;
	}

	// Build the linked list on this node
	pExtraInfo->pNext = pHead;
	pExtraInfo->pPrev = NULL;

	// update the list on the surrounding nodes
	if (pExtraInfo->pNext!=NULL)
		pExtraInfo->pNext->pPrev = pExtraInfo;

	// update the head of list pointer
	pHead = pExtraInfo;

	// make sure memory is still OK
#ifdef _DEBUG
	if (CHECK_MEMORY)
		CheckMemory( TRUE );
#endif
}
#endif

/********************************************************************************************

>	void* SimpleCCObject::operator new(size_t size, LPCSTR FileName, INT32 Line)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/93
	Inputs:		size - Number of bytes required
				FileName - The name of the file that caused the allocation
				Line - The line number in the file that caused the allocation
	Returns:	a pointer to the allocated block of memory, or NULL if the 
				allocation failed
	Purpose:	A version of the new operator that stores the filename etc
				into the memory tracking structure to help in debugging.
				This can be activated by including the following line :-
				MonoOn #define new CAM_DEBUG_NEW MonoOff
				after all the IMPLEMENT_DYNAMIC() etc macros.

********************************************************************************************/

void* SimpleCCObject::operator new(size_t Size, LPCSTR FileName, INT32 Line)
{
	// First we have to make a note of the function pointer maintained by VC++ thats
	// called by malloc if it fails (as it currently throws an exception)
	// We have to set it to NULL to stop malloc doing anything when mem runs out
#if !defined(__WXMAC__)
	_PNH old_handler = _set_new_handler( NULL );
#else
	new_handler pnh = set_new_handler(NULL);
#endif
	
#ifdef _OUTPUT_DEBUG
	TotalMemAllocated += Size;
	TRACE( _T("Allocated %ld\n"),TotalMemAllocated);
#endif

	#ifdef _DEBUG
		// We actually want more memory to build our list out of
		Size += sizeof(MemTrackExtra);
	#endif

	void* pStart = NULL;

	// See if we are supposed to fail this memory claim for low memory simulation
#ifdef _DEBUG
	if (FailingMemory)
	{
		if (Size < BytesBeforeFail)
			BytesBeforeFail -= Size;
		else
			goto Allocated;
	}
#endif

	// ask for the actual memory
	pStart = malloc(Size);

	// finally repair the function pointer to its old value and return our pointer
#if !defined(__WXMAC__)
	_set_new_handler( old_handler );
#else
	set_new_handler(pnh);
#endif

	// Do lots of tracking if in Debug mode
#ifdef _DEBUG
	if (pStart!=NULL)
	{
		// Build in all the extra memory tracking info that we like to have
		MemoryTrackAdd(pStart, FileName, Line);
	}

Allocated:
// We'll remove the label as well (to get rid of the warning)
#endif
	// if the pointer is NULL then we should set the error handler
	if (pStart==NULL)
	{
		TRACE( wxT("ALERT! ALERT! new is about to return NULL, Watch out for unhandled errors\n") );
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
	}

//	TRACE( _T("SimpleCCObject::new at 0x%08x (0x%08x)\n"),((char*)pStart) + sizeof(MemTrackExtra), pStart);

#ifdef _DEBUG
	// return the pointer to the space after our tracking info
	return (((char*)pStart) + sizeof(MemTrackExtra));
#else
	// return the real start of the block
	return (pStart);
#endif
}


/********************************************************************************************

>	void *SimpleCCObject::operator new(size_t size)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/93
	Inputs:		size - The number of bytes to allocate
	Returns:	a pointer to the allocated memory (or NULL if the allocation failed)
	Purpose:	This function allocates a block of memory to create a new SimpleCCObject
				derived thingy. This does not throw an exception, but returns NULL
				in the event of a failure. In debug builds it also keeps track of 
				all allocations and will give a dump of all reamining blobs of memory
				when camelot exits
	SeeAlso:	SimpleCCObject::delete; SimpleCCObject::MemoryDump

********************************************************************************************/

void *SimpleCCObject::operator new(size_t Size)
{
	// First we have to make a note of the function pointer maintained by VC++ thats
	// called by malloc if it fails (as it currently throws an exception)
	// We have to set it to NULL to stop malloc doing anything when mem runs out
#if !defined(__WXMAC__)
	_PNH old_handler = _set_new_handler( NULL );
#else
	new_handler pnh = set_new_handler(NULL);
#endif

	#ifdef _DEBUG
		// We actually want more memory to build our list out of
		Size += sizeof(MemTrackExtra);
	#endif

#ifdef _OUTPUT_DEBUG
	TotalMemAllocated += Size;
	TRACE( _T("Allocated %ld\n"),TotalMemAllocated);
#endif

	// ask for the actual memory
	void* pStart = malloc(Size);

	// finally repair the function pointer to its old value and return our pointer
#if !defined(__WXMAC__)
	_set_new_handler( old_handler );
#else
	set_new_handler(pnh);
#endif

	// Do lots of tracking if in Debug mode
#ifdef _DEBUG
	// see if we have a NULL value returned from new
	if (pStart!=NULL)
	{
		// Build in all the extra memory tracking info that we like to have
		MemoryTrackAdd(pStart, NULL, 0);
	}
#endif

	// if the pointer is NULL then we should set the error handler
	if (pStart==NULL)
	{
		TRACE( wxT("ALERT! ALERT! new is about to return NULL, Watch out for unhandled errors\n") );
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
	}

//	TRACE( _T("SimpleCCObject::new at 0x%08x (0x%08x)\n"),((char*)pStart) + sizeof(MemTrackExtra), pStart);

#ifdef _DEBUG
	// return the pointer to the space after our tracking info
	return (((char*)pStart) + sizeof(MemTrackExtra));
#else
	// return the real start of the block
	return (pStart);
#endif
}




/********************************************************************************************

>	void SimpleCCObject::operator delete(void *p)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/93
	Inputs:		p - pointer to the block of mem to free
	Purpose:	I think you can work this one out! In debug builds this will also
				remove items from the memory tracking list.
	Errors:		Asserts if it discovers that the block you are trying to delete has
				already been deleted, never existed or is otherwise not valid.

********************************************************************************************/

void SimpleCCObject::operator delete(void *p)
{
	// behave like normal delete
	if (p==NULL)
		return;

#ifdef _DEBUG
//		TRACE( _T("SimpleCCObject::delete at 0x%08x (0x%08x)\n"),p, ((char*)p) - sizeof(MemTrackExtra));
	
		// Get the real start of the block
		p = ((char*)p) - sizeof(MemTrackExtra);

		// find out how big the block is
#if !defined(__WXMAC__)
		size_t BlockLen = _msize(p);
#else
		size_t BlockLen = malloc_size(p);
#endif

#ifdef _OUTPUT_DEBUG
		TotalMemAllocated -= BlockLen;
		TRACE( _T("DeAllocated %ld\n"),TotalMemAllocated);
#endif

		// Find the start of my strange structure
		MemTrackExtra* pExtraInfo = (MemTrackExtra*)p;

		// make sure that the guard word is still there
		if (pExtraInfo->GuardWord == GUARDWORD)
		{
			// yep, all seems well, so remove this item from the list
			// Update the head of the list if needed
			if (pExtraInfo == pHead)
				pHead = pExtraInfo->pNext;

			// Update the next item in the list
			if (pExtraInfo->pNext!=NULL)
				pExtraInfo->pNext->pPrev = pExtraInfo->pPrev;

			// update the previous item in the list
			if (pExtraInfo->pPrev!=NULL)
				pExtraInfo->pPrev->pNext = pExtraInfo->pNext;

			// Write over the block with the dead memory land fill value
			memset(p, OLDLANDFILL, BlockLen);

			// Destroy the guard word
			pExtraInfo->GuardWord = DEADGUARD;
		}
		else
		{
			if (pExtraInfo->GuardWord == DEADGUARD)
				ENSURE(FALSE, "Memory was de-alloced more than once...\n"); // Debug
			else
				ENSURE(FALSE, "ALERT!!!! Memory Tracker thinks that memory is toast...\n"); // Debug

			// either way, the memory is not there to be freed
			return;
		}
	#endif
	
	// free up the allocation
	free(p);

	#ifdef _DEBUG
	// and check everything is OK
	if (CHECK_MEMORY)
		CheckMemory( TRUE );
	#endif
}




/********************************************************************************************

>	void SimpleCCObject::MemoryDump()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/93
	Purpose:	Dumps the details of all objects allocated at the time of the call to 
				the Debug window

********************************************************************************************/

void SimpleCCObject::MemoryDump()
{
#ifdef _DEBUG
	// somewhere to hold the output string
	TCHAR				OutputMsg[256];

	// Get a pointer to the head of the list of memory items
	MemTrackExtra	   *pItem = pHead;
	if( pItem != NULL )
		TRACE( _T("......Memory Leaks Detected......\n"));

	// loop through it
	while( pItem != NULL )
	{
PORTNOTE("other","Removed MFC CDumpContext")
#ifndef EXCLUDE_FROM_XARALX
			((CCObject*)(pItem->Start))->Dump(afxDump);
#endif
#if 0 != wxUSE_UNICODE
		TCHAR pFilename[22];
		camMbstowcs(pFilename, pItem->Filename, 22);
#else
		TCHAR* pFilename = pItem->Filename;
#endif
		camSnprintf( OutputMsg, 256, _T("allocated in %s at line %d.\n"), pFilename, pItem->LineNum );
		TRACE(OutputMsg);

		// Get the next one along
		pItem = pItem->pNext;
		if (pItem==NULL)
			TRACE( _T("\n......End Of Memory Dump......\n"));
	}
#endif
}




/********************************************************************************************

>	void CheckForUsedPointer(void *PtrToCheck)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/95
	Purpose:	Secret-squirrel (global, 'cos ccobject.h is a tad critical) function
				called by IndexedColours when they detect that they are "in use".

				This function rampages through all CCSimpleObject derived objects
				looking for a word which matches the supplied pointer. If such a value
				is found, the object containing it is 'Dump'ed. 

				This allows us to track down the object(s) which reference an IndexedColour
				when it is found to be "in use" - with this function we can track down
				where references are, and, more importantly, where they were allocated!

				Change this function very carefully - it is prototyped in colourix.cpp
				and possibly anywhere else that use it.

				IMPORTANT
				This function is NOT AVAILABLE on debug builds. ONLY call it from
				code inside an #ifdef _DEBUG construct.
				

********************************************************************************************/

#ifdef _DEBUG
void CheckForUsedPointer(void *PtrToCheck)
{
	// Get a pointer to the head of the list of memory items
	MemTrackExtra* pItem = pHead;

	// loop through it
	while (pItem!=NULL)
	{
		DWORD *Ptr = (DWORD *) pItem->Start;
		for (INT32 Index = 0; Index < (INT32)pItem->Size; Index += sizeof(DWORD))
		{
			if (*Ptr == (DWORD_PTR)PtrToCheck)
			{
				TRACEALL( _T("    UsedPointer: ") );
PORTNOTE("other","Removed MFC CDumpContext")
#ifndef EXCLUDE_FROM_XARALX
				((CCObject*)(pItem->Start))->Dump(afxDump);
#endif
				TRACEALL( _T(" allocated in %s at line %d.\n"), pItem->Filename, pItem->LineNum );
			}

			Ptr += 1;		// Pointer Arithmetic!
		}

		// Get the next one along
		pItem = pItem->pNext;
	}
}
#endif





/******************************\
** 							  **
** Objects for Runtime Typing **
**							  **
\******************************/

// The textual name of CCObject
static TCHAR BASED_CODE szCCObject[] = wxT("CCObject");

// Special CCRuntimeClass object for CCObject (no base class)
CCRuntimeClass CCObject::cc_classCCObject =
{
	szCCObject, 			// Name of the class
	sizeof(CCObject), 		// Size of an object of this class
	0xffff, 				// Schema number for serialisation (none)
	NULL, 					// Pointer to the default contructor (none)
	NULL					// Pointer to the CCRuntimeClass object for the base class (none)
};

// Static CC_CLASSINIT object to ensure CCObject's runtime information is linked
// into CCRuntimeClass's list.
static CC_CLASSINIT _init_CCObject(&CCObject::cc_classCCObject);

/********************************************************************************************

>	BOOL CCRuntimeClass::IsKindOf(const CCRuntimeClass* pClass) const

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		pClass - pointer to the CCRuntimeClass object of the class to compare
				against.
	Returns:	TRUE if it is a kind of the specified class;
				FALSE if not.
	Purpose:	This function will return TRUE if the object is of the same class as that
				specified by pClass, or if it is derived from the class specified by
				pClass.
	SeeAlso:	CCObject::GetRuntimeClass; CC_RUNTIME_CLASS; CCObject

********************************************************************************************/

BOOL CCRuntimeClass::IsKindOf(const CCRuntimeClass* pClass) const
{
	// Do some sanity checking
	ENSURE(	this != NULL,
			"CCRuntimeClass::IsKindOf: Object pointer is null");

	// It had better be in valid memory, at least for CCObject size
	ENSURE(	IsValidAddress(this, sizeof(CCRuntimeClass)),
			"CCRuntimeClass::IsKindOf: Object is in invalid memory area");

	// Get the runtime class information for this object
	register const CCRuntimeClass* pClassThis = this;
	
	// Sanity checks
	ENSURE(	pClass != NULL, 
			"CCRuntimeClass::IsKindOf: pClass pointer is null");
	ENSURE(pClassThis != NULL, 
			"CCRuntimeClass::IsKindOf: pClassThis pointer is null");
	
	// Check for objects of the same class, or walk the base class chain until
	// it runs out, or we find a match.
	while (pClassThis != NULL)
	{
		if (pClassThis == pClass)
			return TRUE;
		pClassThis = pClassThis->m_pBaseClass;
	}
	
	// No match - these classes are not the same.
	return FALSE;
}

/********************************************************************************************

>	CCRuntimeClass* CCObject::GetRuntimeClass() const

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to the CCRuntimeClass object associated with CCObject.
	Purpose:	This function allows access to the CCRuntimeClass object for this class,
				which in turn allows dynamic classing and construction.
				This function is automatically overridden on derived classes if you use
				the CC_DECLARE_... and CC_IMPLEMENT_... macros.
	Errors:		-
	SeeAlso:	CCObject; CCRuntimeClass; CCObject::IsKindOf; CCRuntimeClass::CreateObject;
				CC_DECLARE_DYNAMIC; CC_IMPLEMENT_DYNAMIC;
				CC_DECLARE_DYNCREATE; CC_IMPLEMENT_DYNCREATE;
				CC_CLASS_MEMDUMP; CC_DECLARE_MEMDUMP; CC_IMPLEMENT_MEMDUMP;
				CCObject::GetRuntimeClass;

********************************************************************************************/

CCRuntimeClass* CCObject::GetRuntimeClass() const
{
	return &CCObject::cc_classCCObject;
}

// static version of above

CCRuntimeClass* CCObject::GetMyClass()
{
	return &CCObject::cc_classCCObject;
}
				
/********************************************************************************************

>	CCRuntimeClass* CCObject::GetRuntimeClassByName(LPCSTR* pClassName) const 

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Inputs:		Pointer to the string containing the textual class name.
	Outputs:	-
	Returns:	Pointer to the CCRuntimeClass object associated with that name, or nul if
				not found.
	Purpose:	This function allows access to the CCRuntimeClass object for this class,
				which in turn allows dynamic classing and construction.
				This function is automatically overridden on derived classes if you use
				the CC_DECLARE_... and CC_IMPLEMENT_... macros.
	Errors:		-
	SeeAlso:	CCObject; CCRuntimeClass; CCObject::IsKindOf; CCRuntimeClass::CreateObject;
				CC_DECLARE_DYNAMIC; CC_IMPLEMENT_DYNAMIC;
				CC_DECLARE_DYNCREATE; CC_IMPLEMENT_DYNCREATE;
				CC_CLASS_MEMDUMP; CC_DECLARE_MEMDUMP; CC_IMPLEMENT_MEMDUMP;
				CCObject::GetRuntimeClass;

********************************************************************************************/

CCRuntimeClass* CCObject::GetRuntimeClassByName(LPCTSTR pClassName)
{
	CCRuntimeClass* pClass = CCRuntimeClass::pFirstClass; 
	do
	{
		if( 0 == camStrcmp(pClassName,pClass->m_lpszClassName) )
		{
			return (pClass);
		}
	}
	while( ( pClass = pClass->m_pNextClass ) != NULL );
	
	return (NULL);
}



/********************************************************************************************

>	BOOL CCObject::IsKindOf(const CCRuntimeClass* pClass) const

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		pClass - pointer to the CCRuntimeClass object of the class to compare
				against.
	Outputs:	-
	Returns:	-
	Purpose:	This function will return TRUE if the object is of the same class as that
				specified by pClass, or if it is derived from the class specified by
				pClass.
	Errors:		-
	SeeAlso:	CCObject::GetRuntimeClass; CC_RUNTIME_CLASS; CCObject

********************************************************************************************/
/********************************************************************************************

Technical Notes:

This function relies on the fact that all classes derived from CCObject which have been
declared and defined as having dynamic class support, will have a static CCRuntimeClass
object as a data member.  This object will also have been inserted into CCRuntimeClass's
list of CCObject-derived classes (see CC_CLASSINIT).  Therefore, this function simply 
uses GetRuntimeClass() to get the CCRuntimeClass object for the CCObject-derived object in
question, and compares the pointers - if the objects are of the same class, both pointers
will point to the same static CCRuntimeClass data member.

If the pointers are not the same, the function follows the base class chain backwards
until either the object is found to be derived from the class specified by pClass, or until
the base class chain comes to an end, in which case the objects are not of the same class,
so FALSE is returned.

********************************************************************************************/

BOOL CCObject::IsKindOf(const CCRuntimeClass* pClass) const
{
	// Do some sanity checking
	ENSURE(	this != NULL,
			"CCObject::IsKindOf: Object pointer is null");

	// It had better be in valid memory, at least for CCObject size
	ENSURE(	IsValidAddress(this, sizeof(CCObject)),
			"CCObject::IsKindOf: Object is in invalid memory area");

	// Get the runtime class information for this object
	register CCRuntimeClass* pClassThis = GetRuntimeClass();
	
	// Sanity checks
	ENSURE(	pClass != NULL, 
			"CCObject::IsKindOf: pClass pointer is null");
	ENSURE(pClassThis != NULL, 
			"CCObject::IsKindOf: pClassThis pointer is null");
	
	// Check for objects of the same class, or walk the base class chain until
	// it runs out, or we find a match.
	while (pClassThis != NULL)
	{
		if (pClassThis == pClass)
			return TRUE;
		pClassThis = pClassThis->m_pBaseClass;
	}
	
	// No match - these classes are not the same.
	return FALSE;
}

/********************************************************************************************

>	CCObject* CCRuntimeClass::CreateObject()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to a new object.
	Purpose:	Dynamic creation of objects.  This function should be used when you want to
				create an object and you do not know what type it should be until runtime.
				An example would be copying another object derived from CCObject.
				
				\i See CCObject and CCRuntimeClass for more details. \i0
				
	Errors:		-
	SeeAlso:	CCObject; CCRuntimeClass; CC_RUNTIME_CLASS; CCObject::IsKindOf

********************************************************************************************/

CCObject* CCRuntimeClass::CreateObject()
{
	void* pObject = NULL;

	// Allocate memory for the new object.
	pObject = CCObject::operator new(m_nObjectSize);

	// Attempt to construct it using dynamic creation facilities
	if ((pObject != NULL) && ConstructObject(pObject))
		// All ok - return pointer to new object
		return (CCObject*) pObject;
	else
	{
		// Something went wrong...
		TRACE0( wxT("Warning: CCRuntimeClass::CreateObject failed\n") );
		
		// Did the 'new' succeed? If so, deallocate the storage
		if (pObject != NULL)
			CCObject::operator delete(pObject);  // clean up
			
		// Object creation failed.
		return NULL;
	}
}

/********************************************************************************************

>	BOOL CCRuntimeClass::ConstructObject(void* pThis)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Dynamically construct an instance of this class in the memory pointed to by
				'pThis'.  Return FALSE if can't construct (i.e. an abstract class).
				
				\i Do not call this function directly - use CreateObject() instead. \i0
				
	Errors:		-
	SeeAlso:	CCObject::CreateObject

********************************************************************************************/


BOOL CCRuntimeClass::ConstructObject(void* pThis)
{
	// Make sure memory we've been given is not a brown cylindrical object.
	ENSURE(	IsValidAddress(pThis, m_nObjectSize), 
			"CCRuntimeClass::ConstructObject: pThis pointer is invalid");

	// Try to create the object.
	if (m_pfnConstruct != NULL)
	{
		// It's not an abstract class, so call the constructor
		(*m_pfnConstruct)(pThis);
		return TRUE;
	}
	else
	{
		// Error - cannot construct an instance of an abstract class.
		TRACE( _T("Error: Trying to construct object of an abstract class %s.\n"),
			m_lpszClassName);
		return FALSE;
	}
}


#ifdef _CAMDLL
//
// For the shared DLL, we build a sub-list of the CRuntimeClasses during
// a single DLL or EXE init.  This sub-list gets moved from the
// global 'pFirstClass' to some other place once the DLL or EXE
// is properly initialized.
//
// !!! This has not been implemented yet !!!
//
#endif

// Initialise the list of CCRuntimeClass objects used for dynamic classing.
CCRuntimeClass* CCRuntimeClass::pFirstClass = NULL;

/********************************************************************************************

>	CC_CLASSINIT::CC_CLASSINIT(CCRuntimeClass* pNewClass)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		pNewClass - pointer to the CCRuntimeClass object associated with the class
				that is to be registered.
	Outputs:	-
	Returns:	-
	Purpose:	This constructor adds a given class (which should be derived from CCObject)
				to the list of classes that support dynamic classing/creation.
				
				\i Do not use this directly - use the CC_DECLARE/IMPLEMENT macros instead. 
				\i0
	Errors:		-
	SeeAlso:	CCObject; CCRuntimeClass; CCObject::GetRuntimeClass; CCObject::IsKindOf;
				CC_RUNTIME_CLASS; CC_DECLARE_DYNAMIC; CC_IMPLEMENT_DYNAMIC;
				CC_DECLARE_DYNCREATE; CC_IMPLEMENT_DYNCREATE; CC_CLASS_MEMDUMP;
				CC_DECLARE_MEMDUMP; CC_IMPLEMENT_MEMDUMP

********************************************************************************************/

CC_CLASSINIT::CC_CLASSINIT(CCRuntimeClass* pNewClass)
{
	// Make sure it hasn't been added already
	ENSURE(	pNewClass->m_pNextClass == NULL, 
			"CC_CLASSINIT::CC_CLASSINIT: pNewClass has been added already!");
	
	// Insert the object at the head of the list used by CCRuntimeClass.
	pNewClass->m_pNextClass = CCRuntimeClass::pFirstClass;
	CCRuntimeClass::pFirstClass = pNewClass;
}

/********************************************************************************************

>	CCAssertValidObject(const CCObject* pOb, LPCSTR lpszFileName, INT32 nLine)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		pOb - pointer to the object to check.
				lpszFileName - filename of the source file this is called from.
				nLine - line number of the source file this is called from.
	Outputs:	-
	Returns:	-
	Purpose:	
	Errors:		Assertion failure if object is not valid, i.e. if:

				* pOb is a NULL pointer
				
				* pOb does not point to memory which is valid & writeable for the whole
				  of the object's area.
				
				* pOb has an illegal virtual function table pointer.
				
				\i Do not use this function directly - use CC_ASSERT_VALID instead. \i0
				
	SeeAlso:	CC_ASSERT_VALID

********************************************************************************************/


#ifdef _DEBUG
extern "C" void CCAssertValidObject(const CCObject* pOb, LPCSTR lpszFileName, INT32 nLine)
{
	if (pOb == NULL)
	{
		TRACE0( _T("CC_ASSERT_VALID fails with NULL pointer\n") );
		TCHAR			pszTmp[256];
		camSnprintf( pszTmp, 256, _T("%s, %d"), lpszFileName, nLine );
		wxFAIL_MSG( pszTmp );
		return;     // quick escape
	}
	
#if defined(__WXMSW__)
	if( !IsValidAddress( pOb, sizeof(CCObject) ) )
	{
		TRACE0( _T("CC_ASSERT_VALID fails with illegal pointer\n") );
		TCHAR			pszTmp[256];
		camSnprintf( pszTmp, 256, _T("%s, %d"), lpszFileName, nLine );
		wxFAIL_MSG( pszTmp );
		return;     // quick escape
	}

#ifndef _M_I86SM
/*	// check to make sure the far VTable pointer is valid
	ENSURE(	sizeof(CCObject) == sizeof(void FAR*), 
			"VTable pointer is not valid");
	if( !IsValidAddress( *(void FAR**)pOb, sizeof(void FAR*), FALSE ) )
	{
		TRACE0( _T("CC_ASSERT_VALID fails with illegal vtable pointer\n"));
		char			pszTmp[256];
		sprintf( pszTmp, "%s, %d", lpszFileName, nLine );
		wxFAIL_MSG( pszTmp );
		return;     // quick escape
	} */
#endif

	if( !IsValidAddress( pOb, pOb->GetRuntimeClass()->GetSize() ) )
	{
		TRACE0( _T("CC_ASSERT_VALID fails with illegal pointer\n") );
		TCHAR			pszTmp[256];
		camSnprintf( pszTmp, 256, _T("%s, %d"), lpszFileName, nLine );
		wxFAIL_MSG( pszTmp );
		return;     // quick escape
	}
#endif
	
	pOb->AssertValid();
}
#endif //_DEBUG

/********************************************************************************************

>	void CCObject::AssertValid() const

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Ensures that a an instance of CCObject (or a derived class) is valid.
				In the base case, this means that 'this' is not a NULL pointer.
				You can override this function to provide whatever validation you want
				for your class (preferably using ENSURE/ASSERT).
				
				\i You should bracket this function with #ifdef _DEBUG ... #endif so that it
				is not called in retail builds.  You will get a link error if you do not
				do this, as the function has no definition in retail builds. \i0
	Errors:		-
	SeeAlso:	ENSURE

********************************************************************************************/

void CCObject::AssertValid() const
{
	ENSURE(this != NULL, "CCObject::AssertValid: This pointer is null");
}

/********************************************************************************************

>	void CCObject::Dump(CDumpContext& dc) const

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		dc - an MFC CDumpContext object that you should direct your output to.
	Outputs:	-
	Returns:	-
	Purpose:	Print out the contents of an object derived from CCObject.
				You may override this function to dump whatever information you deem useful
				or helpful about this object - it is used, amongst other things, in
				diagnostic memory dumps.
				The default action if you do not override this function is to print:
				
				"a ClassName at $address"
				
				where ClassName is the name of the class that the object is an instance of,
				and address is the value of the 'this' pointer - \i you should provide at
				least this level of functionality if you override the function. \i0
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

PORTNOTE("other","Removed MFC CDumpContext")
#ifndef EXCLUDE_FROM_XARALX
void CCObject::Dump(CDumpContext& dc) const
{
#ifdef _DEBUG
	char Msg[256];
	_stprintf(Msg, "%s at %p ", GetRuntimeClass()->m_lpszClassName, (void*)this);
	afxDump << Msg;
#else
	dc;
#endif //_DEBUG
}
#endif

/********************************************************************************************

>	CCObject::CCObject()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CCObject constructor - does nothing at present.  Ought to be inline.
	Errors:		-
	SeeAlso:	CCObject

********************************************************************************************/

CCObject::CCObject()
{
}

/********************************************************************************************

>	CCObject::~CCObject()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CCObject destructor - does nothing at present.  Ought to be inline.
	Errors:		-
	SeeAlso:	CCObject

********************************************************************************************/

CCObject::~CCObject()
{
}
