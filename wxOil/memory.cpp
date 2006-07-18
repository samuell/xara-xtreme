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

// This is the Camelot Dynamic Memory Manager
// This has been changed to use the normal Static heap now, as SmartHeap does a better job.
// If you want to get back to any of the source code that was here before, you will want to check
// out version 1.30 of this file. After that it is mostly missing.


#include "camtypes.h"
//#include "handles.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "memory.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "memblk.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#if defined(__WXGTK__)
#include <stdio.h>
#endif

#ifdef RALPH
#include "ralphcri.h"
#endif

DECLARE_SOURCE("$Revision$");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************


	  Technical Notes
	  ~~~~~~~~~~~~~~~

	  General
	  ~~~~~~~

	  At present, this module obtains a chunk of memory from Windows' Global heap,
	  and uses this to satisfy client memory requests.  The handle to this memory is
	  resolved to a physical address, and the block is locked for the duration of the
	  memory manager's use (i.e. until DeinitMemory() is called, when it is unlocked and
	  returned to the global heap).  The 'FreePoolPtr' variable points to the base of the
	  block.  Under 16bit Windows, the block is limited to 64k in size.

	  The handles used are managed by another module - the handle manager (see
	  winoil\handles.{h,cpp}).

	  In order to cope with 'heavy' blocks (see ClaimBlock), two heaps are maintained.
	  One is for heavy blocks, the other for normal blocks.  This is the ideal solution
	  under Windows, because we use a GlobalAlloc call for each, so when we need to add
	  a new heavy block, we don't need to move all the non-heavy objects to do so.  The OS
	  will cope with it by remapping the memory controller - i.e. we don't move any
	  memory - this is *good*.

	  Allocated blocks of memory are not always contiguous; there may be holes (unused 
	  blocks) in the heap.

	  The first word of a memory block contains its size in bytes, and a flag to indicate
	  whether or not it is a hole (i.e. not in use).  Therefore it is possible to step 
	  through the heap simply by reading the size of each block and using that to move on to
	  the next block.  A size of 0 indicates that the block containing the free space at the
	  end of the heap has been reached.  This block is also marked as _not_ being a 
	  hole - this (somewhat counter-intuitively) makes certain operations on the heap 
	  simpler.

	  The 'FreeOffset' variable is an offset to the start of any remaining memory in the 64k
	  block (hence it is 0 initially - all memory is available).  This memory does not 
	  include holes.  When a request for memory fails, the holes are examined to see if one 
	  of them can accomodate the request.  If no holes are big enough, the manager 
	  incrementally removes the holes until it can be satisfied, or until there are no more 
	  holes to free.  ("When Alexander saw the size of his allocation, he wept, for there 
	  were no more holes to free...")

	  The memory manager uses the principle of lazy evaluation - expensive operations are
	  defered until they are absolutely necessary, or when the CPU would otherwise be idle.
	  This minimises response times for the user.

	  Holes
	  ~~~~~

	  Holes are intended to be transient - the idea is that when releasing a block it is
	  simply marked as released.  Then, when the application receives idle events, it should
	  call TidyMemory(), which will move one block at a time to clean up the holes.

	  The upshot of all this is that when large allocations/deallocations are taking place, 
	  the user doesn't notice much (if any) degradation in response time, because the memory 
	  is not being moved.  However, when the user next stops to think for a few seconds, the
	  application will be 'covertly' tidying up its memory holes while the CPU is idle.  Only
	  one block is tidied at once so that control can be returned to the user quickly (when
	  they stop thinking and start clicking).

	  The function RemoveBlockGaps() is also provided to force all holes to be purged from
	  the heap (which may be desirable for events such as removing a document from memory,
	  and so on).

	  When a block is released by its owner, the associated handle is also freed. The list of
	  holes is maintained using the blocks themselves - as each block (either used or unused)
	  has its size contained in the first word of its memory, the blocks effectively form
	  a linked list which can be efficiently scanned for holes.  Holes are not joined 
	  together unless this is necessary (usually when the heap is being compacted).

	  When tidying the memory, it is guaranteed (by calling JoinHoles() before doing
	  anything else) that no two holes will be adjacent - any such holes will have been
	  joined into one.  Therefore when a hole is found, a block that is in use will be
	  sitting above it in memory.  These two blocks (the hole and the block in use) are
	  then swapped around, so that the hole is above the used block, and the used block now
	  starts where the hole used to.  Any handles into the used block are updated using the
	  handle manager.  There are, of course, no handles into the hole as it is not a valid
	  memory block anyway.  The hole's first word is then set to contain its size, as before.
	  (NB only the contents of the block in use are moved - the hole's contents are not moved
	  because they are undefined anyway, so it would be unnecessary).  Finally, if there is a
	  hole directly above this hole, the two are joined together.


	  For example:
	  ------------
		                    	  Dynamic Heap (X = Block In Use, . = Hole)

	                   +-------+------+------------+-------+------+-----------------+
	  Before tidying:  |XXXXXXX|......|XXXXXXXXXXXX|.......|XXXXXX|   Free Space    |
	                   +-------+------+------------+-------+------+-----------------+


	                   +-------+------------+--------------+------+-----------------+
	  After tidying:   |XXXXXXX|XXXXXXXXXXXX|..............|XXXXXX|   Free Space    |
	                   +-------+------------+--------------+------+-----------------+



	  No special algorithms for reclaiming/joining blocks are required (as they would be
	  in a normal malloc-style heap), because all the blocks in the heap can be moved at 
	  will.
	  Therefore optimising allocations for FIFO schemes, buddy schemes etc., is pointless.
	  Whenever the holes cause problems for allocation requests, they are gradually removed
	  to free up memory - this is not possible for a static heap manager!


	  Inter-block gaps
	  ~~~~~~~~~~~~~~~~

	  The 'EndOfBlockGap' is used to resolve the problem of not knowing whether an address
	  refers to the end of one block or the start of the next (according to Jim).  Blocks 
	  are simply separated by a 'dead zone' of 4 bytes.

	  Since the introduction of BlockHeader fields, I think this EndOfBlockGap can safely
	  be dropped...must try it sometime.


	  Resizing blocks
	  ~~~~~~~~~~~~~~~

	  Blocks can be either increased or decreased in size.  There are two functions which
	  perform simple resizing of blocks: IncreaseBlock() and DecreaseBlock(). They simply
	  alter the size of the block's allocation, and do not move any memory.
	  Two other functions, InsertMemory() and RemoveMemory(), are provided for inserting
	  memory into the middle of a block, or removing it from the middle of a block.

	  	Strategies used for resizing blocks
		~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		When making blocks smaller, the excess is simply made into a hole for removal later 
		on.

		When increasing the size of the block, if a large enough hole exists above the block
		being resized, then it is used to expand the block into, and the hole is decreased 
		in size accordingly.  Note that no other blocks are moved in this case.

		(Future optimisation: at this point, the manager should try to find any hole big 
		enough to hold the block, and reposition the block into that).
		
		If the hole isn't big enough to accommodate the request, then the manager compacts 
		the	heap and tries again.  If there is still not enough room, then the heap itself
		is resized - if not enough memory is available, the request fails.


	  SplitBlock()
	  ~~~~~~~~~~~~

	  NB. SplitBlock() has been replaced by the other function calls listed above, and 
	  should no longer be used. Please change any existing code that stil uses it.  The
	  function will be removed in the future.

	  SplitBlock() resizes blocks of memory. It can make blocks bigger or smaller.

	
	  Releasing memory
	  ~~~~~~~~~~~~~~~~
	  
	  At present, this module never returns memory to the OS until DeinitMemory() is called.
	  In future, TidyMemory should return memory once the free memory in the heap reaches
	  a certain threshold.


	  The Invaders - Epilogue
	  ~~~~~~~~~~~~~~~~~~~~~~~

	  The addition of holes complicates this module somewhat, but not unduly.  The worst
	  affected functions are SplitBlock() and ClaimBlock(), which run rings around holes in
	  order to do their job.  However, what they're trying to do is avoid redundant memory
	  transfers which can be VERY expensive, and hence the memory manager is faster at its
	  job, which is the point of the exercise (TANSTAAFL).


********************************************************************************************/

class DynamicHeap : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(DynamicHeap)

public:
	// Creation/initialisation/destruction
	DynamicHeap();
	BOOL Init();
	~DynamicHeap();

	// Claiming and releasing memory
	MHANDLE ClaimBlock(size_t Size);
	BOOL ReleaseBlock(MHANDLE Handle);

	// Resizing blocks
	BOOL IncreaseBlock(MHANDLE Handle, UINT32 NumBytes);
	BOOL DecreaseBlock(MHANDLE Handle, UINT32 NumBytes);
};

CC_IMPLEMENT_MEMDUMP(DynamicHeap, CCObject)

// Pointers to the heap objects.
static DynamicHeap *NormalHeap = NULL;



/********************************************************************************************

>	DynamicHeap::DynamicHeap()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/94
	Purpose:	Sets up the heap variables, ready for Init() to be called.
	SeeAlso:	DynamicHeap::Init; DynamicHeap::~DynamicHeap

********************************************************************************************/

DynamicHeap::DynamicHeap()
{
}

/********************************************************************************************

>	BOOL DynamicHeap::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/94
	Returns:	TRUE if the initialisation was successful, FALSE if not (e.g. couldn't
				get any memory).
	Purpose:	Initialise the heap object.  This gets a chunk of memory from the operating
				system, which it will use to satsify allocation requests.
	Errors:		If unable to claim memory.

********************************************************************************************/

BOOL DynamicHeap::Init()
{
	// Everything went ok
	return TRUE;
}



/********************************************************************************************

>	DynamicHeap::~DynamicHeap()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/94
	Purpose:	Deallocates any memory used by this heap object.
				In debug builds it checks the heap for objects which have not been
				deallocated, or for heap corruption, and complains if it finds any.
	SeeAlso:	DynamicHeap::Init; DynamicHeap::DynamicHeap; InitMemory; DeinitMemory

********************************************************************************************/

DynamicHeap::~DynamicHeap()
{
}



/********************************************************************************************

>	MHANDLE DynamicHeap::ClaimBlock(UINT32 Size)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/93
	Inputs:		Size: The size in bytes of the block being requested 
	Returns:	The handle that refers to the newly allocated block.
	Purpose:	Allocates a block of 'size' bytes from the free pool, and obtains a handle
      			for it.  Blocks which are zero bytes long are permitted, but blocks must
      			be word-sized (i.e. a multiple of 4 bytes in size).

********************************************************************************************/

MHANDLE DynamicHeap::ClaimBlock(size_t Size)
{
#ifdef RALPH
	RalphCriticalSection RalphCS;
#endif

	// Get the memory
	ADDR pNewBlock = (ADDR) CCMalloc(Size);

	// Check for NULL
	if (pNewBlock==NULL)
		return BAD_MHANDLE;

	// Attempt to get a handle for a new block.
	MHANDLE NewHandle = ClaimHandle(pNewBlock);

	// if we did not get the handle, throw away the allocation
	if (NewHandle==BAD_MHANDLE)
	{
		TRACE( wxT("ClaimBlock - got the memory, but not a handle\n") );
		CCFree(pNewBlock);
		pNewBlock = NULL;
	}

	// return the handle
	return NewHandle;
}




/********************************************************************************************

>   BOOL DynamicHeap::ReleaseBlock(MHANDLE Handle)

    Author:     Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:    26/4/93
    Inputs:	    Handle: The handle of the block to be released.
    Returns:    TRUE if the handle was valid
    Purpose:    Given a handle to a memory block, releases this memory block from use, and
                returns the block to the free pool.  Subsequent accesses via this handle
                or via the block's address cause undefined behaviour.

********************************************************************************************/


BOOL DynamicHeap::ReleaseBlock(MHANDLE Handle)
{
	#ifdef RALPH
	RalphCriticalSection RalphCS;
	#endif

    // Get details for this handle
    ADDR Address = DescribeHandle(Handle);

	// See if it was bad
	if (Address == BAD_MHANDLE)
	{
		// Invalid handle - return error
		TRACE( wxT("ReleaseBlock: Handle is invalid\n") );
		return FALSE;
	}

	// It was OK, so free the memory
	CCFree(Address);

	// And release the handle or it never will be
	ReleaseHandle(Handle);

	return TRUE;
}





/********************************************************************************************

>	BOOL DynamicHeap::IncreaseBlock(MHANDLE Handle, UINT32 NumBytes)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/93
	Inputs:		Handle - the handle of the block to resize
				NumBytes - the number of bytes to add to this block's allocation (must
							be word-sized, and >=12 bytes).
	Returns:	TRUE if the resize worked, FALSE if it failed.
	Purpose:	See IncreaseBlock for a full description.
	SeeAlso:	IncreaseBlock; DecreaseBlock; InsertMemory; RemoveMemory; DescribeHandle

********************************************************************************************/

BOOL DynamicHeap::IncreaseBlock(MHANDLE Handle, UINT32 NumBytes)
{
#ifdef RALPH
	RalphCriticalSection RalphCS;
#endif

    // Get details for this handle
    ADDR Address = DescribeHandle(Handle);

	// See if it is bad
	if (Address == BAD_MHANDLE)
	{
		// Invalid handle - return error
		TRACE( wxT("IncreaseBlock: Handle is invalid\n") );
		return FALSE;
	}

	// Find out how big the old block was
	size_t				Size = CCGetBlockSize(Address);
	Size += NumBytes;

	// Reallocate the block to the new size
	ADDR				pNewBlock = (ADDR)CCRealloc(Address, Size);
	if( pNewBlock==NULL )
	{
		TRACE( wxT("realloc failed in IncreaseBlock\n") );
		return FALSE;
	}

	// Update the handle
	AlterHandle( Handle, pNewBlock );
	return TRUE;
}



/********************************************************************************************

>	BOOL DynamicHeap::DecreaseBlock(MHANDLE Handle, UINT32 NumBytes)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/93
	Inputs:		Handle - the handle of the block to resize
				NumBytes - the number of bytes to remove from this block's allocation (must
							be word-sized, and >=12 bytes).
	Returns:	TRUE if the resize worked, FALSE if it failed.
	Purpose:	See DecreaseBlock for a full description.
	SeeAlso:	DecreaseBlock; IncreaseBlock; InsertMemory; RemoveMemory; DescribeHandle

********************************************************************************************/

BOOL DynamicHeap::DecreaseBlock(MHANDLE Handle, UINT32 NumBytes)
{
	#ifdef RALPH
	RalphCriticalSection RalphCS;
	#endif

    // Get details for this handle
    ADDR Address = DescribeHandle(Handle);

	// See if it is bad
	if (Address == BAD_MHANDLE)
	{
		// Invalid handle - return error
		TRACE( wxT("IncreaseBlock: Handle is invalid\n") );
		return FALSE;
	}

	// Find out how big the old block was
	size_t				Size = CCGetBlockSize(Address);
	Size -= NumBytes;

	// Reallocate the block to the new size
	ADDR pNewBlock = (ADDR) CCRealloc(Address, Size);
	if (pNewBlock==NULL)
	{
		TRACE( wxT("realloc failed in IncreaseBlock\n") );
		return FALSE;
	}

	// Update the handle
	AlterHandle(Handle, pNewBlock);
	return TRUE;
}




/********************************************************************************************

>	DynamicHeap *FindHeap(MHANDLE Handle)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/94
	Inputs:		Handle - the handle of the block to look for.
	Returns:	Pointer to the heap containing the block identified by 'Handle', or NULL
				if the block cannot be found.
	Purpose:	This function works out which heap the block associated with the handle
				is in. 

********************************************************************************************/

static inline DynamicHeap *FindHeap(MHANDLE Handle)
{
	ADDR Address = DescribeHandle(Handle);

	if (Address != BAD_MHANDLE)
	{
		// return the heap
		return NormalHeap;
	}

	// Couldn't find the correct heap
	return NULL;
}


/********************************************************************************************

>   BOOL InitMemory();

    Author:     Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:    26/4/93
    Returns:    TRUE if global memory was obtained and initialised successfully.
    Purpose:    Initialises the dynamic memory manager.  This involves obtaining
      			a block of memory from the OS.  This is then used as a pool to satisfy
      			allocation requests from clients of this module.

********************************************************************************************/

BOOL InitMemory()
{
	// Initialise the two heaps - one for normal objects, one for hevy objects (bitmaps etc)
	NormalHeap = new DynamicHeap;
	if (NormalHeap==NULL)
		return FALSE;

	if (!NormalHeap->Init())
		// report failure of heap initialisation
		return FALSE;

	// Everything went ok
	return TRUE;
}



/********************************************************************************************

>     void DeinitMemory();

      Author:     Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
      Created:    26/4/93
      Purpose:    Returns the memory pool to the OS.

********************************************************************************************/

void DeinitMemory()
{
	if (NormalHeap != NULL)
		delete NormalHeap;
}

/********************************************************************************************

>   MHANDLE ClaimBlock(UINT32 Size, BOOL Heavy = FALSE)

    Author:   	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:  	26/4/93
    Inputs:		Size: The size in bytes of the block being requested
				Heavy: If TRUE, then the block is treated differently to other (non-heavy)
				blocks, in that heavy blocks are the last to be moved.  Large objects
				(e.g. bitmaps etc) should be declared heavy.
    Returns:  	The handle that refers to the newly allocated block.
    Purpose:  	Allocates a block of 'size' bytes from the free pool, and obtains a handle
      		  	or it.  Blocks which are zero bytes long are permitted, but blocks must
      		  	be word-sized (i.e. a multiple of 4 bytes in size).

********************************************************************************************/

MHANDLE ClaimBlock(size_t Size, BOOL Heavy)
{
	// Make sure we have a heap
	if (NormalHeap==NULL)
	{
		ERROR3("The Heap is not there!");
		return BAD_MHANDLE;
	}

	// claim some memory from it
	return NormalHeap->ClaimBlock(Size);
}

/********************************************************************************************

>   BOOL ReleaseBlock(MHANDLE Handle);

    Author:     Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:    26/4/93
    Inputs:	    Handle: The handle of the block to be released.
    Returns:    TRUE if the handle was valid
    Purpose:    Given a handle to a memory block, releases this memory block from use, and
                returns the block to the free pool.  Subsequent accesses via this handle
                or via the block's address cause undefined behaviour.

********************************************************************************************/

BOOL ReleaseBlock(MHANDLE Handle)
{
	// Make sure we have a heap
	if (NormalHeap==NULL)
	{
		ERROR3("The Heap is not there!");
		return FALSE;
	}

	// Release the memory
	return NormalHeap->ReleaseBlock(Handle);
}



/********************************************************************************************

>   BOOL DescribeBlock(MHANDLE Handle, ADDR *Address, UINT32 *Size);

    Author:     Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:    26/4/93
    Inputs:	    Handle: The handle of the block to describe
    Outputs:    Address: The real address of the block
      			   Size: The size in bytes of the block
    Returns:    TRUE if the handle was valid
    Purpose:    Given a handle, returns the size and location of the associated block of
                memory.

********************************************************************************************/

BOOL DescribeBlock(MHANDLE Handle, ADDR *Address, size_t *Size)
{
	// Get details associated with this handle
	*Address = DescribeHandle(Handle);

	if (*Address == NULL)
	{
		// Handle was invalid - return error
		TRACE( wxT("DescribeBlock: Handle is invalid\n") );
		*Address = NULL;
		*Size = 0;
		return FALSE;
	}

	// Extract the size from the block
	*Size = CCGetBlockSize(*Address);

	// Everything went ok
	return TRUE;
}

/********************************************************************************************

>     BOOL SplitBlock(MHANDLE Handle, INT32 SplitSize, UINT32 Offset);

      Author:	Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
      Created:	26/4/93
      Inputs:	Handle: The handle of the block to be shrunk/enlarged.
      			SplitSize: Amount of memory to insert/delete.
      			Offset: The position to insert/delete memory at.
      Returns:	FALSE
      Purpose:	NB. Do not use this function! Use on of the following instead:
				IncreaseBlock, DecreaseBlock
	  SeeAlso:	IncreaseBlock; DecreaseBlock

********************************************************************************************/


BOOL SplitBlock(MHANDLE Handle, INT32 SplitSize, UINT32 Offset)
{
	ERROR3("SplitBlock function is now dead. Use IncreaseBlock or DecreaseBlock instead\n");
	return FALSE;
}



/********************************************************************************************

>	BOOL IncreaseBlock(MHANDLE Handle, UINT32 NumBytes)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/93
	Inputs:		Handle - the handle of the block to resize
				NumBytes - the number of bytes to add to this block's allocation (must
							be word-sized, and >=12 bytes).
	Returns:	TRUE if the resize worked, FALSE if it failed.
	Purpose:	Increase the size of the specified block by the specified amount.  No memory
				is moved within the block - the block is simply extended.
				NB. This operation may cause the block to move, so the handle should be
				realised into an address again (via DescribeBlock()) after this function
				returns.
	SeeAlso:	DecreaseBlock; InsertMemory; RemoveMemory; DescribeHandle

********************************************************************************************/

BOOL IncreaseBlock(MHANDLE Handle, UINT32 NumBytes)
{
	// Make sure we have a heap
	if (NormalHeap==NULL)
	{
		ERROR3("The Heap is not there!");
		return FALSE;
	}

	// Increase the block
	return NormalHeap->IncreaseBlock(Handle, NumBytes);
}

/********************************************************************************************

>	BOOL DecreaseBlock(MHANDLE Handle, UINT32 NumBytes)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/93
	Inputs:		Handle - the handle of the block to resize
				NumBytes - the number of bytes to remove from this block's allocation (must
							be word-sized, and >=12 bytes).
	Returns:	TRUE if the resize worked, FALSE if it failed.
	Purpose:	Decrease the size of the specified block by the specified amount.  No memory
				is moved within the block - the block is simply reduced in size, and the
				memory is chopped off the end. 
				NB. This operation may cause the block to move, so the handle should be 
				realised into an address again (via DescribeBlock()) after this function 
				returns.
	SeeAlso:	IncreaseBlock; InsertMemory; RemoveMemory; DescribeHandle

********************************************************************************************/

BOOL DecreaseBlock(MHANDLE Handle, UINT32 NumBytes)
{
	// Make sure we have a heap
	if (NormalHeap==NULL)
	{
		ERROR3("The Heap is not there!");
		return FALSE;
	}

	// Decrease the block
	return NormalHeap->DecreaseBlock(Handle, NumBytes);
}

/********************************************************************************************

>	BOOL InsertMemory(MHANDLE Handle, UINT32 Offset, UINT32 NumBytes)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/93
	Inputs:		Handle - the handle of the block to resize.
				Offset - the position within the block to insert the memory at (must be
						 word-aligned).
				NumBytes - the number of bytes to add to this block's allocation (must
							be word-sized, and >=12 bytes).
	Returns:	TRUE, if the insertion was successful, FALSE if not.
	Purpose:	Insert memory into the middle of a block at the position specified by
				the 'Offset' parameter.  The memory above the offset is shuffled up (i.e.
				it is not overwritten).
				i.e.   the 'B' block is moved up in order to insert the new memory.
				The new memory (the '?' block) is uninitialised, so you must initialise
				it yourself.

	SeeAlso:	IncreaseBlock; DecreaseBlock; RemoveMemory; DescribeHandle;
				DynamicHeap::InsertMemory

********************************************************************************************/

BOOL InsertMemory(MHANDLE Handle, UINT32 Offset, UINT32 NumBytes)
{
	ERROR3("InsertMemory function is now dead. Use IncreaseBlock or DecreaseBlock instead\n");
	return FALSE;
}

/********************************************************************************************

>	BOOL RemoveMemory(MHANDLE Handle, UINT32 Offset, UINT32 NumBytes)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/93
	Inputs:		Handle - the handle of the block to resize.
				Offset - the position within the block to remove the memory at (must be
						 word-aligned).
				NumBytes - the number of bytes to remove to this block's allocation (must
						   be word-sized, and >=12 bytes).
	Returns:	TRUE, if the removal was successful, FALSE if not.
	Purpose:	Remove memory from the middle of a block at the position specified by
				the 'Offset' parameter.  The memory contents above the offset is shuffled 
				down (i.e. it is not lost).
				i.e.   the 'B' block is moved down in order to preserve its contents.
				The 'A' block is truncated, and the last 'NumBytes' of it are lost.
				
	SeeAlso:	IncreaseBlock; DecreaseBlock; InsertMemory; DescribeHandle;
				DynamicHeap::RemoveMemory

********************************************************************************************/

BOOL RemoveMemory(MHANDLE Handle, UINT32 Offset, UINT32 NumBytes)
{
	ERROR3("RemoveMemory function is now dead. Use IncreaseBlock or DecreaseBlock instead\n");
	return FALSE;
}

/********************************************************************************************

>	BOOL TidyMemory()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/94
    Returns:    TRUE if a hole was removed, FALSE if there were no holes left to remove.
    Purpose:    Removes one hole from the heap (if any exist).  It always removes the hole
    			which is lowest in memory in an attempt to avoid redundant memory moves.
      			Intended to be called on idle events to sanitise the heap when nothing
      			much else is going on.

********************************************************************************************/

BOOL TidyMemory()
{
	return FALSE;
}




#ifdef _DEBUG
/********************************************************************************************

>   void DumpMemory();

    Author:     Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:    11/5/93
    Purpose:    Displays size of blocks in the heap (including holes and the free pool
      			at the end).  Uses TRACE macro for output - for debugging purposes.

********************************************************************************************/

void DumpMemory()
{
	// The memory is now allocated by smartHeap, so it will report leaks
}



/********************************************************************************************

>   void SetMemoryFailFlag(BOOL DenyRequests)

    Author:     Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
    Created:    17/11/93
    Inputs:	  	DenyRequests - if TRUE, the dynamic heap pretends that it has no more free
      						   memory, and all future requests are denied.
      						   if FALSE, the heap will start to grant requests for memory
      						   again.
    Purpose:    Used in debugging, to check how various subsystems cope with the dynamic
      			heap becoming exhausted.

********************************************************************************************/

void SetMemoryFailFlag(BOOL DenyRequests)
{
	// This function no longer has any effect.
}

#endif // _DEBUG




/********************************************************************************************

>   void GetMemoryStatus(UINT64* pPhysRam = NULL, UINT32* pLoadPercent = NULL)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    09/March/2006
    Inputs:		-
	Ouputs:		PhysRam - Amount of physical RAM (in bytes)
				CurrentLoadPercent - Percentage of physical RAM in use at the time of this call
	Returns:	-
    Purpose:	Read the current state of the memory environment for Camelot

********************************************************************************************/

#if defined(__WXMSW__)
typedef BOOL (PASCAL *GETMEMORYSTATUSEX_FUNC)(LPMEMORYSTATUSEX lpBuffer);
#endif

void GetMemoryStatus(UINT64* pPhysRam, UINT32* pLoadPercent)
{
#if defined(__WXMSW__)
	// Windows:
	// It's important to call GlobalMemoryStatusEX if it's available
	// because GlobalMemoryStatus has issues in older OSs that can cause it to return
	// duff information.
	GETMEMORYSTATUSEX_FUNC pExFunc;
	pExFunc = (GETMEMORYSTATUSEX_FUNC) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GlobalMemoryStatusEx");

	if (pExFunc)
	{
		MEMORYSTATUSEX memStatus;
		memStatus.dwLength=sizeof(MEMORYSTATUSEX);
		pExFunc(&memStatus);

		if (pLoadPercent) *pLoadPercent = memStatus.dwMemoryLoad;
		if (pPhysRam) *pPhysRam = memStatus.ullTotalPhys;
	}
	else
	{
		MEMORYSTATUS memStatus;
		memStatus.dwLength=sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&memStatus);

		if (pLoadPercent) *pLoadPercent = memStatus.dwMemoryLoad;
		if (pPhysRam) *pPhysRam = memStatus.dwTotalPhys;
	}
#elsif defined(__WXGTK__)
	// Assume Linux - there should really be some FreeBSD code here

	/* Linux: read memory information from the kernel's /proc/meminfo interface */
	FILE *fp;
	unsigned long /*TYPENOTE: Correct*/ memTotalKb, memFreeKb;
	int /*TYPENOTE: Correct*/ haveMemTotal = 0, haveMemFree = 0;
	char lineBuf[256];

	fp = fopen("/proc/meminfo", "r");

	if (fp != NULL)
	{
		while (!haveMemTotal || !haveMemFree)
		{
			if (fgets(lineBuf, 256, fp) == NULL)
				break;

			if (sscanf(lineBuf, "MemTotal: %lu", &memTotalKb) == 1)
				haveMemTotal = 1;
			if (sscanf(lineBuf, "MemFree: %lu", &memFreeKb) == 1)
				haveMemFree = 1;
		}
		fclose(fp);
	}

	if (!haveMemTotal)
		memTotalKb = 512UL * 1024;      /* guess 512MB */
	if (!haveMemFree)
		memFreeKb = memTotalKb / 2;     /* guess 50% free */

	if (pPhysRam != NULL)
		*pPhysRam = (UINT64)memTotalKb * 1024;
	if (pLoadPercent != NULL)
		*pLoadPercent = (UINT32)(100UL - ((memFreeKb * 100UL) / memTotalKb));
#else
	PORTNOTETRACE("other", "GetMemoryStatus is not implemented on this architecture");
	if (pPhysRam) *pPhysRam = 512L * 1024 * 1024;	// Guess 512M
	if (pLoadPercent) *pLoadPercent = 50;			// Guess 50% free
#endif
}



