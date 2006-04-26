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
// Tuned Memory manager

/*
*/

#include "camtypes.h"
#include "tunemem.h"
#include "errors.h"
#include "fixmem.h"

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_MEMDUMP(TunedMemory, CC_CLASS_MEMDUMP)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

//static BOOL ShowBandingRAM = FALSE;

// Defaults for smart banding
BOOL TunedMemory::RecommendInfinite = FALSE;
INT32 TunedMemory::RecommendRAM = 1024*768;

/********************************************************************************************

	Preference:	MaxRenderingRAM
	Section:	
	Range:		>128K
	Purpose:	This pref is used to set the maximum amount of ram that can be used to
				render with. If there is less than the screen needs here, the rendering
				will be banded. Useful on Low memory machines.

********************************************************************************************/

size_t TunedMemory::LimitRAM 		= 1024*768;
size_t TunedMemory::TotalRAM 		= 1024*768;
size_t TunedMemory::AvailableRAM 	= 1024*768;	// Make this the same!



/********************************************************************************************

	Preference:	IsRenderingRAMInfinte
	Section:	
	Range:		TRUE, FALSE
	Purpose:	If this pref is TRUE then the MaxRenderingRAM preference is ignored and all
				the ram we want is used. If it is FALSE the rendering ram will be limited
				to the figure in MaxRenderingRAM

********************************************************************************************/

BOOL TunedMemory::IsInfinite = TRUE;


/********************************************************************************************

	Preference:	SmartRenderingRAM
	Section:	
	Range:		TRUE, FALSE
	Purpose:	If this pref is TRUE then the MaxRenderingRAM preference is ignored and 
				the amount to use is calculated according to available memory. 
				If it is FALSE the rendering ram will be limited to the figure in 
				MaxRenderingRAM

********************************************************************************************/

BOOL TunedMemory::SmartBanding = TRUE;


/********************************************************************************************

>	TunedMemory::TunedMemory()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Purpose:	Constructor - sets the memory thing up with a default value of 1Mb of
				available ram

********************************************************************************************/

TunedMemory::TunedMemory()
{
	// make sure that the available ram is the same as the total
	AvailableRAM = TotalRAM = LimitRAM;
}





/********************************************************************************************

>	TunedMemory::~TunedMemory()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Purpose:	Destructor - TRACES an error out if there is still some ram left over

********************************************************************************************/

TunedMemory::~TunedMemory()
{
	if (AvailableRAM!=TotalRAM)
	{
		TRACEUSER( "Rik", wxT("At Exit the Available Ram did not equal the Total Ram in TunedMemory\n") );
		TRACEUSER( "Rik", wxT("TotalRAM = %ld, Available=%ld\n"), TotalRAM, AvailableRAM );
	}
}





/********************************************************************************************

>	LPVOID TunedMemory::LimitedCCMalloc(UINT32 Size)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Inputs:		Size - the ammount of ram needed
	Returns:	A pointer to the allocated ram or NULL if there is no more ram available
	Purpose:	Allocates ram out of the limited pool. The Available ram is decreased.
	SeeAlso:	LimitedCCFree

********************************************************************************************/

LPVOID TunedMemory::LimitedCCMalloc(size_t Size)
{
	// Get some ram
	LPVOID Memory = CCMalloc(Size);

	// if we got it, make a note of its allocation
	if ((Memory!=NULL) && (!IsInfinite))
	{
		AvailableRAM -= Size;
	}

	// return what we got
	return Memory;
}

/********************************************************************************************

>	LPVOID TunedMemory::LimitedCCRealloc(LPVOID buf, UINT32 NewSize)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/95
	Inputs:		Size - the new ammount of ram needed
	Returns:	A pointer to the allocated ram or NULL if there is no more ram available
	Purpose:	Re-Allocates ram out of the limited pool. The Available ram is decreased.
	SeeAlso:	LimitedCCFree

********************************************************************************************/

LPVOID TunedMemory::LimitedCCRealloc(LPVOID buf, size_t NewSize)
{
	// Find out how big the block is at the moment
	size_t OldSize = CCGetBlockSize(buf); 

	// Resize the block
	LPVOID Memory = CCRealloc( buf, NewSize );

	// If it got bigger, then decrease the available RAM,
	// otherwise we increase it
	if ((Memory!=NULL) && (!IsInfinite))
	{
		AvailableRAM -= (NewSize - OldSize);
	}

	// return what we got
	return Memory;
}




/********************************************************************************************

>	void TunedMemory::LimitedCCFree(LPVOID buf)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Inputs:		buf - the previously allocated buffer
	Purpose:	Frees the memory allocated by LimitedCCMalloc

********************************************************************************************/

void TunedMemory::LimitedCCFree(LPVOID buf)
{
	// Find out how big the block was
	size_t BlockSize = CCGetBlockSize(buf); 

	// add it back into the pool
	if (!IsInfinite)
		AvailableRAM += BlockSize;

	// Free the memory
	CCFree(buf);
}





/********************************************************************************************

>	INT32 TunedMemory::GetTotalTunedMem()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Returns:	Total ram in the limited heap
	Purpose:	Finds out the total ram that can be allocated in the limited memory thing.
				This is how much ram the user has deemed enough for Camelot to do its
				rendering etc with. When this ammount has been allocated rendering will
				have to wait for other regions to finish before they can start.
	SeeAlso:	TunedMemory::GetAvailableTunedMem

********************************************************************************************/

size_t TunedMemory::GetTotalTunedMem()
{
	return TotalRAM;
}





/********************************************************************************************

>	INT32 TunedMemory::GetAvailableTunedMem()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Returns:	The amount of available ram in the limited pool
	Purpose:	This is the amount of the ram that could be allocated (GetTotal) that is
				still left unallocated. If this amount is not enough for your job, then
				you should wait until this function returns a bigger number. It would also
				be worth checking to see if the TotalTunedRam would be enough as well.

********************************************************************************************/

size_t TunedMemory::GetAvailableTunedMem()
{
	return AvailableRAM;
}



/********************************************************************************************

>	static INT32 TunedMemory::GetLimitedMemSize()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Returns:	The current size of the limited memory thing
	Purpose:	To allows the Options Dialog to find out what the current setting for the
				amount of RAM that can be used for rendering is at.

********************************************************************************************/

size_t TunedMemory::GetLimitedMemSize()
{
	return LimitRAM;
}



/********************************************************************************************

>	static void TunedMemory::SetLimitedMemSize(INT32 NewSize)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Inputs:		NewSize - The new size of the limited memory heap
	Returns:	NewSize if ok or if the value is too low, a new minimum size. 
	Purpose:	This function allows the options dialog to set the amount of ram to use
				for drawing to a new value that the user chose. If the value is less than
				a minimum useful value, at present 128k, then the value will be ignored and
				128k will be used.

********************************************************************************************/

size_t TunedMemory::SetLimitedMemSize(size_t NewSize)
{
	// make sure it is not too small for safty
	if (NewSize<1024*128)
		NewSize = 1024*128;

	// Find out what the difference is
	size_t Difference = NewSize - TotalRAM;

	// Set the total to the new size
	TotalRAM = NewSize;
	LimitRAM = NewSize;

	// And change the available ram as well
	AvailableRAM += Difference;
	
	return NewSize;
}



/********************************************************************************************

>	static BOOL TunedMemory::IsMemSizeInfinite()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Returns:	TRUE if the Limited heap is not currently limited, FALSE if it is
	Purpose:	Lets the options dialog have a look at the Tuned Memorys IsInfinite state.

********************************************************************************************/

BOOL TunedMemory::IsMemSizeInfinite()
{
	return IsInfinite;
}

/********************************************************************************************

>	static BOOL TunedMemory::IsAutomaticMemory()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/95
	Returns:	TRUE if the memory limit will be calculated automatically.
				FALSE if the user value is being used.
	Purpose:	Lets the options dialog have a look at the Smart banding state.

********************************************************************************************/

BOOL TunedMemory::IsAutomaticMemory()
{
	// This routine has been changed to determine whether to use the
	// recommended memory or not.
	return SmartBanding;
}


/********************************************************************************************

>	static void TunedMemory::SetMemSizeInfinte(BOOL NewIsInfinite)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Inputs:		NewIsInfinite - The new state of the Infinte flag
	Purpose:	Sets the infinte flag in the limited memory manager. TRUE means the limited
				heap should be made infinte, FALSE means use the value found by
				GetLimitedMemSize
				NOTE: This call now determines whether to use the recommended memory
				      or not.

********************************************************************************************/

void TunedMemory::SetMemSizeInfinte(BOOL NewSmartBanding)
{
	// Set the member version
	SmartBanding = NewSmartBanding;

	if (SmartBanding)
	{
		// If 'Smart' banding enabled, then use the 
		// recommended memory (calculated on startup)
		IsInfinite 	= RecommendInfinite;

		if (!IsInfinite)
		{
			// Set the recommended memory limit

			// Find out what the difference is
			size_t		Difference = RecommendRAM - TotalRAM;

			// Set the total to the new size
			TotalRAM = RecommendRAM;

			// And change the available ram as well
			AvailableRAM += Difference;
		}
	}
	else
	{
		// Use the user specified limited RAM
	 	IsInfinite = FALSE;
		TotalRAM = LimitRAM;
		AvailableRAM = LimitRAM;
	}
}




/********************************************************************************************

>	static INT32 TunedMemory::GetScreenMemSize()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Returns:	The mem required to do a full screen redraw with transparancy on
	Purpose:	This function gives a figure for the amount of ram needed to redraw
				the full screen with transparency on. for 1024 x 768 this is about 3.5Meg

********************************************************************************************/

INT32 TunedMemory::GetScreenMemSize()
{
	INT32					Width  = wxSystemSettings::GetMetric( wxSYS_SCREEN_X );
	INT32					Height = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );
	return Width * Height * 4;
}




/********************************************************************************************

>	static BOOL Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Returns:	TRUE if it worked, FALSE if not
	Purpose:	Declares the preferences for this class in the ini file.

********************************************************************************************/

BOOL TunedMemory::Init()
{
	if( Camelot.DeclareSection( wxT("Screen"), 10 ) )
	{ 
		Camelot.DeclarePref( NULL, wxT("MaxRenderingRAM"), (INT32*)&LimitRAM );
		Camelot.DeclarePref( NULL, wxT("IsRenderingRAMInfinte"), &IsInfinite, FALSE, TRUE );
		Camelot.DeclarePref( NULL, wxT("SmartRenderingRAM"), &SmartBanding, FALSE, TRUE );

//		Camelot.DeclarePref(NULL, "ShowBandingRAM", &ShowBandingRAM, FALSE, TRUE);
	}

	// Find the available Physical memory,
	// and work out a sensible band size to use
	CalculateRecommendedMemory();

	if (SmartBanding)
	{
		// Use the recommended memory
		TotalRAM 	= RecommendRAM;
		IsInfinite 	= RecommendInfinite;
	}
	else
	{
		// Use the user specified limited RAM
		TotalRAM 	= LimitRAM;
	}

	// Set available ram equal to the total ram
	AvailableRAM = TotalRAM;
	return TRUE;
}

/********************************************************************************************

>	void TunedMemory::CalculateRecommendedMemory()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/95
	Purpose:	Calculates a sensible default Banding size given the Physical memory
				available.

********************************************************************************************/

void TunedMemory::CalculateRecommendedMemory()
{
	TRACE( _T("Warning - TunedMemory::CalculateRecommendedMemory called") );

#if defined(__WXMSW)
	MEMORYSTATUS MemStat;
	MemStat.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&MemStat);

	INT32 PhysicalMemory = MemStat.dwTotalPhys;
	TRACEALL( wxT("Total Physical Memory is %d\n"), PhysicalMemory);

//	INT32 ScreenMem = GetScreenMemSize();
//	TRACEALL( _T("Screen Memory is %d\n"), ScreenMem);

	if (PhysicalMemory <= 8*1024*1024)			// 8 Megs or less
	{
		RecommendInfinite = FALSE;				// Use limited memory
		RecommendRAM = 512*1024;				// Limit to max of 512 K
	}
	else if (PhysicalMemory <= 12*1024*1024)	// 8 to 12 Megs
	{
		RecommendInfinite = FALSE;				// Use limited memory
		RecommendRAM = 1*1024*1024;				// Limit to max of 1024 K
	}
	else if (PhysicalMemory <= 16*1024*1024)	// 12 to 16 Megs
	{
		RecommendInfinite = FALSE;				// Use limited memory
		RecommendRAM = 2*1024*1024;				// Limit to max of 2048 K
	}
	else 										// More than 16 Megs
	{
		RecommendInfinite = TRUE;				// Unlimited memory
	}

	if (ShowBandingRAM)
	{
		ERROR3_PF( ("Total Physical Memory is %d", PhysicalMemory) );

		if (IsInfinite)
		{
			ERROR3("Using unlimited banding RAM");
			TRACEALL( wxT("Using unlimited Rendering RAM\n") );
		}
		else
		{
			ERROR3_PF( ("Rendering RAM set to %d",TotalRAM) );
			TRACEALL( wxT("Rendering RAM set to %d\n"), TotalRAM );
		}
	}
#else
	RecommendInfinite = TRUE;
#endif
}

