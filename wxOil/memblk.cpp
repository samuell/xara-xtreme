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

// class to handle large memory blocks

/*
*/

#include "camtypes.h"

#include <errno.h>

#include "errors.h"
//#include "andy.h"
//#include "simon.h"				// for _R(IDS_CONTINUE)
#include "memblk.h"
#include "ensure.h"
#include "camelot.h"

#ifndef WIN32
#define	MAX_BLOCK	0xFFFCL		// 64k-4
#endif

#if USE_VM_BLOCKS
UINT32 MemoryBlock::PageSize;				// page size used in Virtual Mem API
#endif

/*********************************************************************************************

>	class MemoryBlock

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/94
	Purpose:	Some items start small and get large. The MemoryBlock class encapsulates
				memory management for objects that generally need to be fixed in memory
				but need to grow, and are generally large. Items that use the class
				include the handles table and the relocatable heap.
		
*********************************************************************************************/

/*********************************************************************************************

>	MemoryBlock::MemoryBlock()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/94
	Inputs:		-
	Returns:	-
	Purpose:	Constructor. Initialises some fields to zero. Does not do any allocations.
		
*********************************************************************************************/

MemoryBlock::MemoryBlock()
{
#if USE_STD_ALLOC
#elif USE_VM_BLOCKS
	MemBase = NULL;
#else
	MemHandle = 0;
#endif
}

/*********************************************************************************************

>	MemoryBlock::~MemoryBlock()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/94
	Inputs:		-
	Returns:	-
	Purpose:	Destructor. Doesn't do anything really, though might do a DeInit one day.
	SeeAlso:	MemoryBlock::DeInit
		
*********************************************************************************************/

MemoryBlock::~MemoryBlock()
{
}

/*********************************************************************************************

>	LPVOID MemoryBlock::Init( INT32 InitSize, BOOL bAutoZero, MemoryBlockType Type )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/94
	Inputs:		InitSize	initial size of block required
				bAutoZero	TRUE if new memory required to be zeroed
				Type		one of MEMORYBLOCK_xxx constants. Determines maximum size of
							block.
	Returns:	Pointer to memory if worked, NULL if failed to initialise.
	Purpose:	Sets up block for subsequent allocations.
		
*********************************************************************************************/

LPVOID MemoryBlock::Init( INT32 InitSize, BOOL bAutoZero, MemoryBlockType Type )
{
#if USE_STD_ALLOC

	m_RoundedSize = GetRoundedSize( InitSize );

	// now physically grab the starting size
	LPVOID				pNewPtr;
	if( bAutoZero )
	{
		pNewPtr = calloc( m_RoundedSize, 1 );
	}
	else
	{
		pNewPtr = malloc( m_RoundedSize );
	}
		
	if( pNewPtr == NULL )
	{
		TRACEUSER( "Andy", wxT("malloc() failed because %lx\n"), errno );
		return NULL;
	}

	TRACEUSER( "Andy", wxT("malloc OK at %p\n"), pNewPtr );
	m_pMemBlk = pNewPtr;
	return pNewPtr;

#elif USE_VM_BLOCKS

	// first time we are called get the page size
	if (PageSize==0L)
	{
		SYSTEM_INFO SysInfo;
		GetSystemInfo( &SysInfo );
		PageSize = SysInfo.dwPageSize;
	}

	// Note that the Virtual API always gives us zeroed memory whether we want it or not


	// calculate a suitable maximum size for the type of block
	INT32 LargestSize;

	switch (Type)
	{
		case MEMORYBLOCK_HANDLETABLE:
			LargestSize = 0x01000000;				// 16M of handles is enough
			break;
		case MEMORYBLOCK_RELOCHEAP:
			LargestSize = 0x10000000;				// 256M of reloc heap
			break;
		case MEMORYBLOCK_SCREENBUFFER:
			LargestSize = 0x01000000;				// 1024x768x32bits x4 for good measure
			break;
		default:
			ENSURE( FALSE, "Bad type in MemoryBlock::Init");
			return NULL;
	}

	// firstly reserve a gratuitously large chunk of address space

	MemBase = VirtualAlloc( NULL, LargestSize, MEM_RESERVE, PAGE_NOACCESS );
	if (MemBase==NULL)
	{
		TRACEUSER( "Andy", wxT("VirtualAlloc(reserve) failed because %lx\n"), GetLastError() );
		return NULL;
	}

	RoundedSize = GetRoundedSize( InitSize );

	// now physically grab the starting size
	LPVOID NewPtr = VirtualAlloc( MemBase, RoundedSize, MEM_COMMIT, PAGE_READWRITE );
	if (NewPtr==NULL)
	{
		TRACEUSER( "Andy", wxT("VirtualAlloc(commit) failed because %lx\n"), GetLastError() );
		VirtualFree(MemBase, 0L, MEM_RELEASE );
		MemBase = NULL;
		return NULL;
	}

	TRACEUSER( "Andy", wxT("VirtualAlloced OK at %lx from %lx\n"), NewPtr, MemBase );

	return NewPtr;
#else

	#ifndef WIN32
	if (InitSize > MAX_BLOCK)
		return NULL;
	#endif

	AutoZero = bAutoZero;											// remember flag in struct

	UINT32 Flags = GMEM_MOVEABLE;
	if (AutoZero)
		Flags |= GMEM_ZEROINIT;

	MemHandle = GlobalAlloc( Flags, InitSize );
TRACE( wxT("GlobalAlloc %u %lx returned %x\n"), Flags, InitSize, (INT32)MemHandle);
	if (MemHandle==NULL)
		return NULL;

	LPVOID Ptr = GlobalLock( MemHandle );
	if (Ptr == NULL)
	{
		GlobalFree( MemHandle );
		MemHandle = 0;
		return NULL;
	}

	return Ptr;
#endif
}

/*********************************************************************************************

>	void MemoryBlock::DeInit()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/94
	Inputs:		-
	Returns:	-
	Purpose:	Frees up any allocations on this memory block. Should always be called
				during exit for a tidy cleanup.
		
*********************************************************************************************/

void MemoryBlock::DeInit()
{
#if USE_STD_ALLOC
	if( NULL != m_pMemBlk )
	{
		free(  m_pMemBlk );
		m_pMemBlk = NULL;
	}
#elif USE_VM_BLOCKS
	if (MemBase)
	{
		BOOL Result = VirtualFree(MemBase, 0L, MEM_RELEASE);
		MemBase = NULL;
		ENSURE( Result, "VirtualFree failed");
	}
#else
	if (MemHandle)
	{
		GlobalUnlock( MemHandle );
		GlobalFree( MemHandle );
		MemHandle = 0;
	}
#endif
}

/*********************************************************************************************

	static BOOL DoGrowGlobalHandle( HGLOBAL *pHandle, UINT32 OldSize, UINT32 NewSize, UINT32 MoreFlags, LPVOID *Pointer)

		Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
		Created:	24/2/94
		Inputs:		pHandle		pointer to global handle whose block we want bigger, might be
								updated upon return
					OldSize		size it is at the moment
					NewSize		desired size of new block
					MoreFlags	extra flags to pass to GlobalReAlloc
					Pointer		where new pointer is to be stored
		Returns:	TRUE if worked, FALSE if failed
		Purpose:	Low-level private primitve for MemoryBlock::Grow. Actually does the
					work. Function does not exist if using VM_BLOCKs.
		SeeAlso:	MemoryBlock::Grow
		
*********************************************************************************************/

#if !USE_VM_BLOCKS && !USE_STD_ALLOC

static BOOL DoGrowGlobalHandle( HGLOBAL *pHandle, UINT32 OldSize, UINT32 NewSize, UINT32 MoreFlags, LPVOID *Pointer)
{
	if (GlobalUnlock( *pHandle ))
	{
		// if didn't unlock then we won't be able to realloc, so better stop now
		TRACE( wxT("GlobalUnlock failed in GrowGlobalHandle\n") );
		return FALSE;
	}

	// get Windows to grab us a new bigger block. Important to use GMEM_MOVEABLE as we want
	// our block to move if possible to satisfy the request
	HGLOBAL hNewHandle = GlobalReAlloc( *pHandle, NewSize, GMEM_MOVEABLE | MoreFlags );

	if (hNewHandle)
	{
		ADDR NewPtr = (ADDR)GlobalLock( hNewHandle );
		if (NewPtr == NULL)
		{
			// careful here as we really do want to get our old data back - at the moment
			// all we have is a handle to an unlocked block of memory. To try and recover the
			// situation we'll try setting it back to its old size
			hNewHandle = GlobalReAlloc( hNewHandle, OldSize, GMEM_MOVEABLE );
			if (hNewHandle)
			{
				NewPtr = (ADDR)GlobalLock( hNewHandle );
				if (NewPtr)
				{
					// OK, got old data back but might be in different place so tidy up
					// then exit gracefully
					*Pointer = NewPtr;
					*pHandle = hNewHandle;
					TRACE( wxT("WARNING: Made hasty exit from GrowGlobalHandle\n") );
					return FALSE;									// indicate failure
				}
			}
			// this is very catastrophic - we cannot get our old data back, so
			// we will communicate this to the user. The program is likely to
			// crash and burn after this
			TRACE( wxT("DANGER: Catastrophic failure in GrowGlobalHandle\n") );
			if (InformSeriousError( _R(IDE_BAD_GROWHEAP), _R(IDS_ABORT), _R(IDS_CONTINUE) ) == 1)
				AfxAbort();

			return FALSE;									// although system is v. unstable
		}

		TRACEUSER( "Andy", wxT("Handle %x changed to %x (%lx-%lx) (%ld-%ld) %x\n"),
			(INT32)*pHandle, (INT32)hNewHandle,
			(INT32)*Pointer, (INT32)NewPtr,
			(INT32)OldSize, (INT32)NewSize,
			(INT32)MoreFlags
		);

		// we've got our new block, update the callers variables
		*Pointer = NewPtr;
		*pHandle = hNewHandle;
		return TRUE;
	}

	// couldn't reallocate it, the old one will still be there though
	TRACE( wxT("GlobalReAlloc failed in GrowGlobalHandle\n") );
	TRACEUSER( "Andy", wxT("Failed to grow handle %x from %ld.%ld at %lx\n"),
				(INT32)*pHandle,
				(INT32)OldSize,(INT32)NewSize,
				(INT32)*Pointer
			);

	return FALSE;

}
#endif


/*********************************************************************************************

>	BOOL MemoryBlock::Grow( UINT32 OldSize, UINT32 NewSize, LPVOID *Pointer )

		Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
		Created:	24/2/94
		Inputs:		OldSize		size it is at the moment
					NewSize		desired size of new block
					Pointer		where new pointer is to be stored
		Returns:	TRUE if worked, FALSE if failed
		Purpose:	Like GlobalReAlloc in a way, but a safer version that does all it can
					to ensure existing data doesn't get thrown away. Handles unlocking,
					reallocing and relocking the result. Even if the return value is
					FALSE, the data might still have been moved - see if *Pointer has changed
					if this is important to you.
		SeeAlso:	DoGrowGlobalHeap;MemoryBlock::Shrink
		
*********************************************************************************************/

BOOL MemoryBlock::Grow( UINT32 OldSize, UINT32 NewSize, LPVOID *Pointer )
{
#if USE_STD_ALLOC
	NewSize = GetRoundedSize( NewSize );				// round new size up
	if( NewSize == m_RoundedSize )
		return TRUE;									// if same physical size

	LPVOID				NewPtr = realloc( *Pointer, NewSize );
	if (NewPtr==NULL)
	{
		TRACEUSER( "Andy", wxT("VirtualAlloc growing from %ld to %ld failed\n"), OldSize, NewSize);
		return FALSE;
	}
	*Pointer = m_pMemBlk = NewPtr;
	m_RoundedSize = NewSize;
	return TRUE;
	
#elif USE_VM_BLOCKS
	NewSize = GetRoundedSize( NewSize );				// round new size up
	if (NewSize == RoundedSize)
		return TRUE;									// if same physical size

	LPVOID NewPtr = VirtualAlloc( MemBase, NewSize, MEM_COMMIT, PAGE_READWRITE );
	if (NewPtr==NULL)
	{
		TRACEUSER( "Andy", wxT("VirtualAlloc growing from %ld to %ld failed\n"), OldSize, NewSize);
		return FALSE;
	}
	*Pointer = NewPtr;
	RoundedSize = NewSize;
	return TRUE;

#else
	const UINT32 Flags = AutoZero ? GMEM_ZEROINIT : 0;

	#ifndef WIN32
	if (NewSize > MAX_BLOCK)
		return FALSE;
	#endif

	return DoGrowGlobalHandle( &MemHandle, OldSize, NewSize, Flags, Pointer );
#endif
}

/*********************************************************************************************

>	BOOL MemoryBlock::Shrink( UINT32 OldSize, UINT32 NewSize, LPVOID *Pointer )

		Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
		Created:	3/3/94
		Inputs:		OldSize		size it is at the moment
					NewSize		desired size of new block, smaller than OldSize
					Pointer		where new pointer is to be stored
		Returns:	TRUE if worked, FALSE if failed
		Purpose:   	Reduces the current allocation, without throwing away data. Normally
					this should always work, as giving memory back shouldn't ever fail,
					but a FALSE return means we couldn't. If a FALSE return is combined
					with a NULL Pointer return, it means an extremely bad thing occurred
					and the original data has been lost.
		SeeAlso:	MemoryBlock::Grow
		
*********************************************************************************************/

BOOL MemoryBlock::Shrink( UINT32 OldSize, UINT32 NewSize, LPVOID *Pointer )
{
#if USE_STD_ALLOC
	
	OldSize = GetRoundedSize( OldSize );
	NewSize = GetRoundedSize( NewSize );
	
	if (OldSize == NewSize)
		return TRUE;										// no change required

	if (OldSize < NewSize)
	{
		ENSURE(FALSE, "Cannot Shrink backwards");
		return FALSE;
	}									// if same physical size

	LPVOID				NewPtr = realloc( *Pointer, NewSize );
	if (NewPtr==NULL)
	{
		TRACEUSER( "Andy", wxT("VirtualAlloc growing from %ld to %ld failed\n"), OldSize, NewSize);
		return FALSE;
	}
	*Pointer = m_pMemBlk = NewPtr;
	m_RoundedSize = NewSize;
	return TRUE;
	
#elif USE_VM_BLOCKS

	// we do a shrink by freeing up spare blocks

	OldSize = GetRoundedSize( OldSize );
	NewSize = GetRoundedSize( NewSize );

	if (OldSize == NewSize)
		return TRUE;										// no change required

	if (OldSize < NewSize)
	{
		ENSURE(FALSE, "Cannot Shrink backwards");
		return FALSE;
	}

	// lets free up the unwanted bits now
	BOOL Result = VirtualFree( ((LPBYTE)MemBase)+NewSize, OldSize-NewSize, MEM_DECOMMIT );

	if (Result==FALSE)
	{
		TRACEUSER( "Andy", wxT("VirtualFree Shrink from %ld to %ld failed\n"), OldSize, NewSize);
		return FALSE;
	}

	RoundedSize = NewSize;
	return TRUE;

#else
	// the data will be left alone
	ENSURE( FALSE, "Cannot shrink memory blocks");
	return FALSE;

#endif
}
