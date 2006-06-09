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

// C++ interface to GDraw in assembly-language (intel version) in DLL

/*
*/

#include "camtypes.h"
// camconfig.h must be included immediately after camtypes.h
#include "camconfig.h"

#include "GDrawIntf.h"
#include "XaDrwOld.h"
#include "GVersion.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "gradtbl.h"

#include "grndrgn.h"

#include "camprofile.h"

// Andy - tell us if there's some subtle reason for not doing this. I wanted the memory tracking - Alex
//#define	new	CAM_DEBUG_NEW

// statics
GDrawAsm * GDrawAsm::Current = NULL;
INT32 GDrawAsm::Instance = 0;
GDrawAsm * GDrawAsm::sm_pListHead = NULL;

//static GCONTEXT* s_pContext = NULL;

CRITICAL_SECTION CriticalSection::sm_Section;
BOOL CriticalSection::sm_bInit = FALSE;

// trivial constructor

GDrawAsm::GDrawAsm() : GDrawContext()
{
	m_pContext = NULL;
	Instance++;
	SaveArea = NULL;
	SaveAreaLength = 0;
	m_pPrevious = NULL;
	m_pNext = NULL;

	m_ThreadID = 0;
}


GDrawAsm::GDrawAsm(DWORD ThreadID) : GDrawContext()
{
	m_pContext = NULL;
	Instance++;
	SaveArea = NULL;
	SaveAreaLength = 0;
	m_pPrevious = NULL;
	m_pNext = NULL;

	if (ThreadID == 0)
		ThreadID = wxThread::GetCurrentId();

	// Insert this at the beginning of the list
	m_pNext = sm_pListHead;
	sm_pListHead = this;

	m_ThreadID = ThreadID;
}


/********************************************************************************************

>	static GDrawAsm* GetContextForCurrentThread(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/96
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the first context in the list whose ThreadID matches
				(NULL if no matches)
	Purpose:	Used by GRenderRegion to get a thread-safe context for rendering
	Errors:		-
	Scope:		Public static
	SeeAlso:	-

********************************************************************************************/

GDrawAsm* GDrawAsm::GetContextForCurrentThread(void)
{
	CriticalSection Ralph;

	GDrawAsm *pContext = sm_pListHead;

	DWORD ThreadID = wxThread::GetCurrentId();

	while (pContext && (ThreadID != pContext->m_ThreadID))
	{
		pContext = pContext->m_pNext;
	}

	TRACE( wxT("GetContextForCurrentThread = %p"), pContext );

	return(pContext);
}

// these are the allocation fns used by GDraw.dll
extern "C" LPBYTE GDraw_Alloc(UINT32 Size)
{
	LPBYTE Result = (LPBYTE)CCMalloc(Size);

	#ifdef _DEBUG
	if (Result==NULL)
		TRACEALL( _T("GDraw failed to get %ld bytes\n"), Size);
	#endif

	return Result;
}

extern "C" void GDraw_Free(LPBYTE ptr)
{
	CCFree(ptr);
}

/********************************************************************************************

>	BOOL GDrawAsm::Init()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the this pointer can be used to do some rendering, FALSE if cannot
				e.g. platform not supported e.g. Win16. A FALSE return means the member
				functions will return failure codes.
	Purpose:	Can the this pointer be used to render things.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::Init()
{
//	TRACEUSER( "Gerry", _T("Initialising GDrawAsm at 0x%08x\n"), this);
	DWORD GVer = XaDraw_GetVersion();

	// to be compatible, the major numbers must match and the minor number must be
	// at least the one we built this file with
	if ( ( HIWORD(GVer)  >  (WORD)GDRAW_VERSION_MAJOR ) ||
		(( HIWORD(GVer) ==  (WORD)GDRAW_VERSION_MAJOR ) 
#if GDRAW_VERSION_MINOR>0 // Suppress compiler warning
&& ( LOWORD(GVer) >= (WORD)GDRAW_VERSION_MINOR)
#endif
		)
	   )
	{
		if (m_pContext)
		{
			XaDraw_Terminate(m_pContext);
//			TRACEUSER( "Gerry", _T("Reusing context at 0x%08x\n"), m_pContext);
		}

		CamProfile cp(CAMPROFILE_GDRAW);

		// Create the context
		INT32 Len = XaDraw_ContextLength();
		if (Len > 0)
		{
			if (m_pContext == NULL)
				m_pContext = (GCONTEXT*)CCMalloc(Len);
			if (m_pContext)
			{
//				TRACEUSER( "Gerry", _T("Claimed context of %d bytes at 0x%08x\n"), Len, m_pContext);
//				memset(m_pContext, 0x01, Len);
				XaDraw_Initialise(m_pContext, NULL);
				XaDraw_SetMemoryHandlers(m_pContext, GDraw_Alloc, GDraw_Free);
				return TRUE;
			}
			else
			{
				TRACEUSER( "Gerry", _T("Failed to claim context of %d bytes\n"), Len);
			}
		}
	}

	return FALSE;
}


BOOL GDrawAsm::Init(GDrawAsm* pSource)
{
//	TRACEUSER( "Gerry", _T("Initialising GDrawAsm at 0x%08x from 0x%08x\n"), this, pSource);

	if (pSource->m_pContext == NULL)
		return(FALSE);

	DWORD GVer = XaDraw_GetVersion();

	// to be compatible, the major numbers must match and the minor number must be
	// at least the one we built this file with
	if ( ( HIWORD(GVer)  >  (WORD)GDRAW_VERSION_MAJOR ) ||
		(( HIWORD(GVer) ==  (WORD)GDRAW_VERSION_MAJOR )
#if GDRAW_VERSION_MINOR>0 // Suppress compiler warning
		&& ( LOWORD(GVer) >= (WORD)GDRAW_VERSION_MINOR)
#endif
		)
	   )
	{
		if (m_pContext)
		{
			XaDraw_Terminate(m_pContext);
//			TRACEUSER( "Gerry", _T("Reusing context at 0x%08x\n"), m_pContext);
		}

		// Create the context
		INT32 Len = XaDraw_ContextLength();
		if (Len > 0)
		{
			if (m_pContext == NULL)
				m_pContext = (GCONTEXT*)CCMalloc(Len);
			if (m_pContext)
			{
//				TRACEUSER( "Gerry", _T("Claimed context of %d bytes at 0x%08x\n"), Len, m_pContext);
				XaDraw_CopyContext(pSource->m_pContext, m_pContext);
				return TRUE;
			}
			else
			{
				TRACEUSER( "Gerry", _T("Failed to claim context of %d bytes\n"), Len);
			}
		}
	}

	return FALSE;
}


/********************************************************************************************

>	GDrawAsm::~GDrawAsm()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor - performs required tidy up.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

GDrawAsm::~GDrawAsm()
{
	CriticalSection Ralph;

//	TRACEUSER( "Gerry", _T("Deleting GDrawAsm at 0x%08x\n"), this);

	if (Instance > 0)
		Instance--;

//	if (Instance == 0)
//		XaDrawOld_Terminate(); // only terminate when the last one goes

//	if (Instance == 0)
//	{
//		XaDraw_Terminate(s_pContext); // only terminate when the last one goes
//		CCFree(s_pContext);
//		s_pContext = NULL;
//	}

	if (this == GRenderRegion::GetStaticDrawContext())
	{
		TRACEUSER("Gerry", _T("Deleting current GDraw context at 0x%08x\n"), this);
		GRenderRegion::SetTempDrawContext(NULL);
	}

	if (m_pContext)
	{
		CamProfile cp(CAMPROFILE_GDRAW);
		XaDraw_Terminate(m_pContext);
		CCFree(m_pContext);
//		TRACEUSER( "Gerry", _T("Released context at 0x%08x\n"), m_pContext);
		m_pContext = NULL;
	}

	// Make Current not point at this cos it's a bit dangerous
	if (Current == this)
		Current = NULL;

	if (SaveArea)
	{
//		TRACEUSER( "Gerry", _T("Destruct context of %d bytes at 0x%08x\n"), SaveAreaLength, SaveArea);
		CCFree((LPBYTE)SaveArea);	// Oh well, can't return errors
		SaveArea=NULL;		// For tidyness
		SaveAreaLength = 0;
	}

	// If we have a thread id
	if (m_ThreadID != 0)
	{
		// Remove this from the list

		if (m_pPrevious == NULL || sm_pListHead == this)
		{
			// We are the first item so make the head point to the next item
			sm_pListHead = m_pNext;
		}
		else
		{
			// Not the first so make the previous item point to the next item
			m_pPrevious->m_pNext = m_pNext;
		}

		// If we are not the last item
		if (m_pNext != NULL)
		{
			// Make the next item point to the previous item
			m_pNext->m_pPrevious = m_pPrevious;
		}
	}
}



/********************************************************************************************

>	DWORD GDrawAsm::GetMaxBitmapWidth(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/96
	Inputs:		-
	Outputs:	-
	Returns:	Max width of off screen bitmap supported
	Scope:		Public
	SeeAlso:	GDrawAsm::GetMaxBitmapDepth()

********************************************************************************************/

DWORD GDrawAsm::GetMaxBitmapWidth(void)
{
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaDraw_GetMaxBitmapWidth());
}



/********************************************************************************************

>	DWORD GDrawAsm::GetMaxBitmapDepth(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/96
	Inputs:		-
	Outputs:	-
	Returns:	Max depth of off screen bitmap supported
	Scope:		Public
	SeeAlso:	GDrawAsm::GetMaxBitmapWidth()

********************************************************************************************/

DWORD GDrawAsm::GetMaxBitmapDepth(void)
{
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaDraw_GetMaxBitmapDepth());
}



#ifdef RALPH

/********************************************************************************************

>	BOOL GDrawAsm::InitContext(GCONTEXT *Context, INT32 Index = 0)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/96
	Inputs:		Context - ptr to GDraw context data to use
				Index - the number of the context (only for DEFCONS code)
	Outputs:	-
	Returns:	TRUE if no error
	Purpose:	Initialises the GDraw context
				The DEFCONS code is used to generate new default context tables
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::InitContext(GCONTEXT *Context, INT32 Index)
{
	CriticalSection Ralph;

	// Make sure this is current (so others are saved)
	if (!MakeCurrent())
		return(FALSE);

// DEFCONS: This is a marker for updating the default context tables
//			Change the following #if FALSE to TRUE
#if FALSE
	// Just use a static block
	// You will need to increase this if contexts get bigger
	INT32 Length = GetContextLength();
	LPBYTE pBlock = CCMalloc(Length);

	if (pBlock == NULL)
		return(FALSE);

	// Save the context into our block
	SaveContext((GCONTEXT*)pBlock);

	// Open the relevant file
	FILE *f = NULL;

	switch (Index)
	{
	case 0:
		f = fopen("c:\\context\\normal.con", "wb");
		break;

	case 1:
		f = fopen("c:\\context\\errord.con", "wb");
		break;
	}

	if (f != NULL)
	{
		// Write the data to the file
		fwrite(&pBlock, Length, 1, f);

		// Close the file
		fclose(f);
	}

	return(TRUE);

#else
	// Set this context to the passed in data
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaDrawOld_RestoreContext(Context)==0);
#endif
}

#endif


/********************************************************************************************

>	INT32 GDrawAsm::GetContextLength()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Inputs:		None
	Outputs:	-
	Returns:	Length of this GDraw context
	Purpose:	Gets the length of a GDraw context
	Errors:		-
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

INT32 GDrawAsm::GetContextLength()
{
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaDraw_ContextLength());
}

/********************************************************************************************

>	BOOL GDrawAsm::SaveContext(GCONTEXT *Context)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Inputs:		ptr to GDraw context to save
	Outputs:	-
	Returns:	TRUE if no error
	Purpose:	Saves the GDraw context
	Errors:		-
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::SaveContext(GCONTEXT *Context)
{
//	return(XaDrawOld_SaveContext(Context)==0);
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaDraw_CopyContext(m_pContext, Context)==0);
//	return(TRUE);
}

/********************************************************************************************

>	BOOL GDrawAsm::RestoreContext(CONST GCONTEXT *Context)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Inputs:		ptr to GDraw context to restore
	Outputs:	-
	Returns:	TRUE if no error
	Purpose:	Restores the GDraw context
	Errors:		-
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::RestoreContext(CONST GCONTEXT *Context)
{
//	return(XaDrawOld_RestoreContext(Context)==0);
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaDraw_CopyContext(Context, m_pContext)==0);
//	return(TRUE);
}

/********************************************************************************************

>	BOOL GDrawAsm::SwapContext(GCONTEXT *Context)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Inputs:		ptr to GDraw context to swap
	Outputs:	-
	Returns:	TRUE if no error
	Purpose:	Swaps the GDraw context
	Errors:		-
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

//BOOL GDrawAsm::SwapContext(GCONTEXT *Context)
//{
//	return(GDraw_SwapContext(Context)==0);
//}

/********************************************************************************************

>	BOOL GDrawAsm::MakeCurrent()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Inputs:		None
	Outputs:	-
	Returns:	TRUE if no error
	Purpose:	Makes the GDraw context passed the current one
	Errors:		-
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::MakeCurrent()
{
	// Check we're not already the current context
	if (this == Current)
		return(TRUE); // that was easy

#if FALSE
	if (Current != NULL) // then we need to do a save 1st
	{
		if (Current->SaveAreaLength != Current->GetContextLength())
		{
			if (Current->SaveArea != NULL) // free old one if it exists
			{
				TRACEUSER( "Gerry", _T("Freeing context of %d bytes at 0x%08x\n"), Current->SaveAreaLength, Current->SaveArea);
				CCFree((LPBYTE)(Current->SaveArea));
				Current->SaveArea = NULL;		// For tidyness
				Current->SaveAreaLength = 0;
			}

			Current->SaveAreaLength = Current->GetContextLength();
			Current->SaveArea = (GCONTEXT *) CCMalloc(Current->SaveAreaLength);
			TRACEUSER( "Gerry", _T("Claimed context of %d bytes at 0x%08x\n"), Current->SaveAreaLength, Current->SaveArea);
		}

		if (Current->SaveArea == NULL)
		{
			// Can't save the current context so return false
			Current->SaveAreaLength = 0;
			return(FALSE);
		}

		if (!(Current->SaveContext(Current->SaveArea)))
			return(FALSE);
	}

	// If this context has a saved state then restore it
	if (SaveArea != NULL)
		RestoreContext(SaveArea); // Ignore errors
#endif

	// Set current context
	Current = this;

	// It worked
	return (TRUE);
}


/********************************************************************************************

>	virtual eError GDrawAsm::GetLastError()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Inputs:		-
	Outputs:	-
	Returns:	The GDraw error code
	Errors:		See eError enum in gconsts.h
	Scope:		Public

********************************************************************************************/

eError GDrawAsm::GetLastError()
{
	CamProfile cp(CAMPROFILE_GDRAW);
	if (!MakeCurrent())
		return(GERROR_OUT_OF_MEMORY);
	else
		return(XaDraw_GetLastError(m_pContext));
}


/********************************************************************************************

>	virtual INT32 GDrawAsm::GetGraduationTableSize()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Inputs:		-
	Outputs:	-
	Returns:	The size in bytes of the graduation table
	Scope:		Public

********************************************************************************************/

INT32 GDrawAsm::GetGraduationTableSize()
{
	CriticalSection Ralph;

	if (!MakeCurrent())
		return(0);

	CamProfile cp(CAMPROFILE_GDRAW);

	return(XaColour_GetGraduationTableSize(m_pContext));
}


/********************************************************************************************

>	virtual INT32 GDrawAsm::GetGraduationTableLength()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Inputs:		-
	Outputs:	-
	Returns:	The number of entries in the graduation table
	Scope:		Public

********************************************************************************************/

INT32 GDrawAsm::GetGraduationTableLength()
{
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaColour_GetGraduationTableLength());
}


/********************************************************************************************

>	virtual INT32 GDrawAsm::GetLongGraduationTableSize()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Inputs:		-
	Outputs:	-
	Returns:	The size in bytes of the long graduation table
	Scope:		Public

********************************************************************************************/

INT32 GDrawAsm::GetLongGraduationTableSize()
{
	CriticalSection Ralph;

	if (!MakeCurrent())
		return(0);

	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaColour_GetLongGraduationTableSize(m_pContext));
}


/********************************************************************************************

>	virtual INT32 GDrawAsm::GetLongGraduationTableLength()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Inputs:		-
	Outputs:	-
	Returns:	The number of entries in the long graduation table
	Scope:		Public

********************************************************************************************/

INT32 GDrawAsm::GetLongGraduationTableLength()
{
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaColour_GetLongGraduationTableLength());
}


/********************************************************************************************

>	virtual INT32 GDrawAsm::GetTransparentGraduationTableSize()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Inputs:		-
	Outputs:	-
	Returns:	The size in bytes of the transparent graduation table
	Scope:		Public

********************************************************************************************/

INT32 GDrawAsm::GetTransparentGraduationTableSize()
{
	CriticalSection Ralph;

	if (!MakeCurrent())
		return(0);

	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaColour_GetTransparentGraduationTableSize());
}


/********************************************************************************************

>	virtual INT32 GDrawAsm::GetTransparentGraduationTableLength()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Inputs:		-
	Outputs:	-
	Returns:	The number of entries in the transparent graduation table
	Scope:		Public

********************************************************************************************/

INT32 GDrawAsm::GetTransparentGraduationTableLength()
{
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaColour_GetTransparentGraduationTableLength());
}


/********************************************************************************************

>	virtual INT32 GDrawAsm::GetLongTransparentGraduationTableSize()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Inputs:		-
	Outputs:	-
	Returns:	The size in bytes of the long transparent graduation table
	Scope:		Public

********************************************************************************************/

INT32 GDrawAsm::GetLongTransparentGraduationTableSize()
{
	CriticalSection Ralph;

	if (!MakeCurrent())
		return(0);

	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaColour_GetLongTransparentGraduationTableSize());
}


/********************************************************************************************

>	virtual INT32 GDrawAsm::GetLongTransparentGraduationTableLength()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Inputs:		-
	Outputs:	-
	Returns:	The number of entries in the long transparent graduation table
	Scope:		Public

********************************************************************************************/

INT32 GDrawAsm::GetLongTransparentGraduationTableLength()
{
	CamProfile cp(CAMPROFILE_GDRAW);
	return(XaColour_GetLongTransparentGraduationTableLength());
}


/********************************************************************************************

>	void GDrawAsm::SetStackLimit( LPVOID stack )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Pointer to minimum address the stack can be permitted to be used when
				recursively flattening paths.
	Outputs:	-
	Returns:	-
	Purpose:	Stops bad paths form using all stack space up.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

void GDrawAsm::SetStackLimit( LPVOID stack )
{
	CriticalSection Ralph;

	ERROR3("SetStackLimit() is not thread safe!!! Use SetStackSize() instead.");

	if (!(MakeCurrent())) return;
	CamProfile cp(CAMPROFILE_GDRAW);
	XaDraw_SetStackLimit(m_pContext, stack);
}


/********************************************************************************************

>	void GDrawAsm::SetStackSize(INT32 StackSize)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/94
	Inputs:		Maximum size of stack for path flattening
	Outputs:	-
	Returns:	-
	Purpose:	Stops bad paths form using all stack space up.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

void GDrawAsm::SetStackSize(DWORD StackSize)
{
	CriticalSection Ralph;

	if (!(MakeCurrent())) return;
	CamProfile cp(CAMPROFILE_GDRAW);
	XaDraw_SetStackSize(m_pContext, StackSize);
}

/********************************************************************************************

>	BOOL GDrawAsm::SetupBitmap( INT32 Width, INT32 Height, INT32 Depth, LPBYTE Bits, DWORD Format = 2 )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Bitmap size & depth and pointer to actual bitmap bytes.
	Outputs:	-
	Returns:	TRUE if OK, FALSE if didn't like bitmap for some reason.
	Purpose:	This is the bitmap that will be drawn into with rendering calls.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::SetupBitmap( INT32 Width, INT32 Height, INT32 Depth, LPBYTE Bits, DWORD Format, BOOL DoBiCompression )
{
	CriticalSection Ralph;

	if (!(MakeCurrent())) return(FALSE);
	// build a Windows-like structure because thats what Gavin wants.

	BITMAPINFOHEADER bmInfo;

	bmInfo.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.biWidth = Width;
	bmInfo.biHeight = Height;
	bmInfo.biPlanes = 1;
	bmInfo.biBitCount = (WORD)Depth;

	if(DoBiCompression == TRUE)
	{
		// In GDraw 2.008 Docs it says that 32 bit BMPs need this setting to this value
		// if they are to use the Transparency channel.
		bmInfo.biCompression = 0x80000001;
	}
	else
	{
		bmInfo.biCompression = BI_RGB;
	}

	bmInfo.biSizeImage = 0L;
	bmInfo.biXPelsPerMeter = 1000;
	bmInfo.biYPelsPerMeter = 1000;
	bmInfo.biClrUsed = 0;
	bmInfo.biClrImportant = 0;

	// Set up bitmap, but preserve the changed bounding box...
	RECT BBox;
	GetChangedBBox(&BBox);

	if(DoBiCompression == TRUE && Bits != NULL)
	{
		// Another point stated in the GDraw 2.008 Docs is that the 32bit BMP should be
		// Initialized to 00,00,00,FF for every pixel!

		DWORD* pSetBits = (DWORD*)Bits;
		DWORD BMPSIZE = (Width * Height);// >> 2;		// Ilan - why was there a ">> 2"??
		DWORD Index = 0;

		while(Index < BMPSIZE)
		{
			pSetBits[Index++] = 0xFF000000;
		}
	}

	if ( Depth == 8 )
	{
		//	If the bitmap is 8 bit then we must use a format of 0 for Gavin to avoid using his new greyscale bitmap stuff
		//	This is quite horrible as we are corrupting an entry parameter to this function
		//	but as nothing seems to be setting this correctly in 8 bit modes there is little easy alternative
		Format = 0;
	}

	CamProfile cp(CAMPROFILE_GDRAW);
#if defined(__WXMSW__) || !USE_wxBITMAP
	BOOL Result = XaDraw_SetDIBitmap(m_pContext, &bmInfo, Bits, Format) ? FALSE : TRUE;
#else
	BOOL Result = XaDraw_SetInvertedDIBitmap(m_pContext, &bmInfo, Bits, Format) ? FALSE : TRUE;
#endif	

	// Bodged out until this is implemented...
//	SetChangedBBox(&BBox);

	return Result;
}



/********************************************************************************************

>	BOOL GDrawAsm::SetBitmap( CONST LPBITMAPINFO BitmapInfo, CONST LPBYTE Bitmap, DWORD BitmapFormat = 2)

	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/00
	Inputs:		Bitmap info and bits
	Outputs:	-
	Returns:	TRUE if OK, FALSE if didn't like bitmap for some reason.

********************************************************************************************/


BOOL GDrawAsm::SetBitmap( CONST LPBITMAPINFOHEADER BitmapInfo, CONST LPBYTE Bitmap, DWORD BitmapFormat )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

#if defined(__WXMSW__) || !USE_wxBITMAP
	return (MakeCurrent() && (XaDraw_SetDIBitmap(m_pContext, BitmapInfo,Bitmap, BitmapFormat) ? FALSE : TRUE));
#else
	return (MakeCurrent() && (XaDraw_SetInvertedDIBitmap(m_pContext, BitmapInfo,Bitmap, BitmapFormat) ? FALSE : TRUE));
#endif	
}

/********************************************************************************************

>	BOOL GDrawAsm::SetMatrix( GMATRIX* GMat)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		A GMatrix.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets the matrix to be used during rendering.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::SetMatrix( GMATRIX* GMat)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_SetMatrix(m_pContext, GMat) ? FALSE : TRUE));
}

/********************************************************************************************

>	void GDrawAsm::SetAntialiasFlag( BOOL Flag )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Flag determines whether output should be antialiased or not.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the matrix to be used during rendering.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

void GDrawAsm::SetAntialiasFlag( BOOL Flag )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (MakeCurrent()) XaDraw_SetAntialiasFlag(m_pContext, FALSE != Flag );
}

/********************************************************************************************

>	void GDrawAsm::SetAntialiasQualityFlag( BOOL Flag )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Flag determines whether antialiasing should be high quality or standard
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

void GDrawAsm::SetAntialiasQualityFlag( BOOL Flag )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (MakeCurrent()) XaDraw_SetAntialiasQualityFlag(m_pContext, FALSE != Flag );
}

/********************************************************************************************

>	void GDrawAsm::SetTileSmoothingFlag( BOOL Flag )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> H
	Created:	13/9/99
	Inputs:		Flag determines whether output should be Smoothed or not.
	Purpose:	Sets smoother rendering of Bitmap Plotting, but does not work
				with perspectivised!

********************************************************************************************/
void GDrawAsm::SetTileSmoothingFlag( BOOL Flag )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (MakeCurrent()) XaColour_SetTileSmoothingFlag(m_pContext, FALSE != Flag );
}

/********************************************************************************************

>	void GDrawAsm::SetTileFilteringFlag( BOOL Flag )

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/00
	Inputs:		Flag determines whether output should be rendered using maximum quality.
	Purpose:	Sets smoothest rendering of bitmaps - rotation & scaling.

********************************************************************************************/
void GDrawAsm::SetTileFilteringFlag( BOOL Flag )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (MakeCurrent()) XaColour_SetTileFilteringFlag(m_pContext, FALSE != Flag );
}

/********************************************************************************************/

void GDrawAsm::SetMaxFilterSize( UINT32 uSize )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if ( MakeCurrent() )
		XaBitmap_SetMaxFilterSize(m_pContext,uSize);
}

/********************************************************************************************/

BOOL GDrawAsm::Blur( INT32 nBlur )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_Blur(m_pContext,nBlur) ? FALSE : TRUE);
}

/********************************************************************************************/

BOOL GDrawAsm::Sharpen( INT32 nSharpen )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_Sharpen(m_pContext,nSharpen) ? FALSE : TRUE);
}

/********************************************************************************************

>	void GDrawAsm::SetHintingFlag( BOOL Flag )

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/01/95
	Inputs:		Flag determines whether output should be hinted or not.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the flag in GDraw which forces all exactly horizontal and vertical line
				segments to be hinted onto pixel centres.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

void GDrawAsm::SetHintingFlag( BOOL Flag )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (MakeCurrent()) XaDraw_SetHintingFlag(m_pContext, FALSE != Flag );
}

/********************************************************************************************

>	void GDrawAsm::SetFlatness( DWORD Flat )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		How much beziers should be flattened.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the flattening.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

void GDrawAsm::SetFlatness( DWORD Flat )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (MakeCurrent()) XaDraw_SetFlatness(m_pContext, Flat );
}

/********************************************************************************************

>	void GDrawAsm::TransformPath( LPPOINT IPoints, LPPOINT OPoints, DWORD Length, GMATRIX*GMat )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		IPoints is the path to be transformed. Length is how many points. GMatrix is the
				matrix which wants to be used.
	Outputs:	OPoints is where the converted path goes (can be the same as IPoints).
	Returns:	-
	Purpose:	Convert any number of points using a GMatrix.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

void GDrawAsm::TransformPath( LPPOINT IPoints, LPPOINT OPoints, DWORD Length, GMATRIX*GMat )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (MakeCurrent()) XaDraw_TransformPath(m_pContext, IPoints, OPoints, Length, GMat );
}

/********************************************************************************************

>	INT32 GDrawAsm::MaxScale( CONST GMATRIX *Matrix, CONST RECT *BBox )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		-
	Returns:	The scale factor that should be safe to use.

	Purpose:	This returns a scale factor that should be safe to use for a drawing
				of the size of the given bounding box. Currently only the translation
				values of the matrix are used. To convert this maximum scale factor
				to a zoom value, divide by the scale factor required for 100% zoom.

				It is assumed that the required matrix is of the form

														+-S,0,0,+-S,Tx,Ty

				where S is the value returned and Tx,Ty are the translation values
				of the given matrix.

	Errors:		returns -1, on failure.
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

INT32 GDrawAsm::MaxScale( CONST GMATRIX *Matrix, CONST RECT *BBox )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent()) return -1;
	return XaDraw_MaxScale(m_pContext, Matrix, BBox );
}

///////////////////////
// colour
///////////////////////

/********************************************************************************************

>	void GDrawAsm::SetHalftoneOrigin( DWORD x, DWORD y )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		X and Y offsets.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the origin of the dither patterns.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

void GDrawAsm::SetHalftoneOrigin( DWORD x, DWORD y )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (MakeCurrent()) XaColour_SetHalftoneOrigin(m_pContext, x, y );
}

/********************************************************************************************

>	BOOL GDrawAsm::SetConversionPalette( PLOGPALETTE pPalette )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/96
	Inputs:		The Palette to use for ConvertBitmap.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets the Palette that is to be used for ConvertBitmap.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::SetConversionPalette( PLOGPALETTE pPalette )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetConversionPalette(m_pContext, pPalette ) ? FALSE : TRUE));
}

/********************************************************************************************

>	virtual BOOL GDrawAsm::SetDitherStyle(DitherStyle Style = DITHER_GREY_ORDERED)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96

	Inputs:		DitherStyle - Indicates the new DitherStyle to be used, from:
						DITHER_ORDERED,
						DITHER_GREY_ORDERED,
						DITHER_NONE
	Returns:	TRUE if it worked, else FALSE

	Purpose:	Sets the style of the dither patterns.

	Scope:		Public

********************************************************************************************/

BOOL GDrawAsm::SetDitherStyle(DitherStyle Style)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetDitherStyle(m_pContext,Style) ? FALSE : TRUE));
}



/********************************************************************************************

>	BOOL GDrawAsm::AddToGraduationTable( COLORREF Colour, BOOL HSV, GraduationTable* Table, DWORD Index)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Inputs:		A colour to add. If HSV is TRUE then the colour is coverted to RGB
				in HSV space, else in RGB space.
	Outputs:	GraduationTable is updated to include the new colour.
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Adds a colour to a grad table at the specified point.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::AddToGraduationTable( COLORREF Colour, BOOL HSV, GraduationTable* Table, DWORD Index)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_AddToGraduationTable(m_pContext, Colour, HSV, Table, Index ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::BuildGraduationTable( COLORREF Start, COLORREF End, BOOL HSV, GraduationTable* Table)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		A start and end colour. If HSV is TRUE then intermediate colours generated
				in HSV space, else in RGB space.
	Outputs:	GraduationTable is filled as required (defined in gconsts.h).
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Gets table ready for grad fills.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::BuildGraduationTable( COLORREF Start, COLORREF End, DWORD HSV, GraduationTable* Table)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_BuildGraduationTable(m_pContext, Start, End, HSV, Table ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::BuildTransparencyTable( DWORD Start, DWORD End, TransparentGradTable* Table)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		A start and end transparency.
	Outputs:	TransparentGradTable is filled as required (defined in gconsts.h).
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Gets table ready for transparent grad fills.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::BuildTransparencyTable( DWORD Start, DWORD End, TransparentGradTable* Table)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_BuildTransparencyTable(m_pContext, Start, End, Table ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::SetColour( COLORREF Colour )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		A colour.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets drawing colour to be used in subsequent rendering.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::SetColour( COLORREF Colour )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetColour(m_pContext, Colour ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::SetSolidColour( COLORREF Colour, DWORD BPP = 0, DWORD Format = 2 )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		A colour.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets drawing colour to be used in subsequent rendering.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::SetSolidColour( COLORREF Colour, DWORD BPP, DWORD Format )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetSolidColour(m_pContext, Colour, BPP, Format ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::SetWordColour( DWORD Colour )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		A colour.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets drawing colour to be used in subsequent rendering.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::SetWordColour( DWORD Colour )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetWordColour(m_pContext, Colour ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::SetTransparency( COLORREF Colour, TransparencyEnum Trans)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		A colour and a transparency method.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets drawing colour to be used in subsequent rendering with transparency.
				Only has effect if output device can do transparency, else falls into
				SetColour.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::SetTransparency( COLORREF Colour, TransparencyEnum Trans)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetTransparency(m_pContext, Colour, Trans ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::ConvertBitmap( LPBITMAPINFO SInfo, LPBYTE SBytes, LPBITMAPINFO DInfo,
									LPBYTE DBytes, INT32 Dither )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		SInfo and SBytes are the source bitmap. Dither is the type of dithering
				required (currently 0=ordered dither, 1=error diffused).
	Outputs:	DInfo and DBytes are the destination bitmap.
	Returns:	TRUE if worked, FALSE if failed (e.g. unsupported bitmap depth(s))
	Purpose:	Converts bitmaps from one format to another. Currently supported modes are:
				16->8 and 32->8.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::ConvertBitmap( LPBITMAPINFOHEADER SInfo, LPBYTE SBytes, LPBITMAPINFOHEADER DInfo,
									LPBYTE DBytes, INT32 Dither )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_ConvertBitmap(m_pContext, SInfo, SBytes, DInfo, DBytes, Dither ) ? FALSE : TRUE));
}

/********************************************************************************************

>	LPLOGPALETTE GDrawAsm::SelectPalette( INT32 WhichPal )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		A palette number. 0 is greyscale, 1=normal, for other numbers see Gavins
				documentation.
	Outputs:	-
	Returns:	Pointer to a recommended logical palette, or NULL if cannot.
	Purpose:	Returns a 256 colour logical palette that should be used in 8-bit modes.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

const LOGPALETTE* GDrawAsm::SelectPalette( INT32 WhichPal )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent()) return NULL;
	return XaColour_SelectPalette(m_pContext, FALSE != WhichPal );
}

/********************************************************************************************

>	BOOL GDrawAsm::SetGraduation(
		DWORD Style,
		CONST GraduationTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Sets the current fill style to a colour graduation. The low bytes of Style
				should be 0 for linear, 1 for elliptical and 2 for conical. Style & 0xFF
				defines the transparency type (see SetTransparency). Table should have been
				set up either manually or with BuildGraduationTable. The points are used
				to define where the fill is (see Gavins docs).
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Reads the currently selected palette and builds internal tables based upon
				the results. Should be called after a palette change. Note that this may take
				a few seconds to execute and should be called sparingly.
	Errors:		-
	Scope:		Public
	SeeAlso:	SetTransparency;BuildGraduationTable

********************************************************************************************/

BOOL GDrawAsm::SetGraduation(
		DWORD Style,
		CONST GraduationTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetGraduation(m_pContext, Style, Table, PointA, PointB, PointC ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::SetTransparentGraduation(
		DWORD Style,
		CONST TranspGradTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Sets the current fill style to a transparent graduation. The low bytes of Style
				should be 0 for linear, 1 for elliptical and 2 for conical. Style & 0xFF
				defines the transparency type (see SetTransparency). Table should have been
				set up either manually or with BuildTransparencyTable. The points are used
				to define where the fill is (see Gavins docs).
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Reads the currently selected palette and builds internal tables based upon
				the results. Should be called after a palette change. Note that this may take
				a few seconds to execute and should be called sparingly.
	Errors:		-
	Scope:		Public
	SeeAlso:	SetTransparency;BuildTransparencyTable

********************************************************************************************/

BOOL GDrawAsm::SetTransparentGraduation(
		DWORD Style,
		CONST TranspGradTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	Style = Style & 0xFFFF00FF;
	if (Table->IsLarge())
		Style |= 0x0100;

	return (MakeCurrent() && (XaColour_SetTransparentGraduation(m_pContext, Style, Table->GetTable(), PointA, PointB, PointC ) ? FALSE : TRUE));
}


/********************************************************************************************

>	BOOL GDrawAsm::SetGraduation(
		DWORD Style,
		CONST GraduationTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Sets the current fill style to a colour graduation. The low bytes of Style
				should be 0 for linear, 1 for elliptical and 2 for conical. Style & 0xFF
				defines the transparency type (see SetTransparency). Table should have been
				set up either manually or with BuildGraduationTable. The points are used
				to define where the fill is (see Gavins docs).
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Reads the currently selected palette and builds internal tables based upon
				the results. Should be called after a palette change. Note that this may take
				a few seconds to execute and should be called sparingly.
	Errors:		-
	Scope:		Public
	SeeAlso:	SetTransparency;BuildGraduationTable

********************************************************************************************/

BOOL GDrawAsm::SetGraduation4(
		DWORD Style,
		CONST GraduationTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC,
		CONST POINT *PointD
	)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetGraduation4(m_pContext, Style, Table, PointA, PointB, PointC, PointD ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::SetTransparentGraduation(
		DWORD Style,
		CONST TranspGradTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC
	)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Sets the current fill style to a transparent graduation. The low bytes of Style
				should be 0 for linear, 1 for elliptical and 2 for conical. Style & 0xFF
				defines the transparency type (see SetTransparency). Table should have been
				set up either manually or with BuildTransparencyTable. The points are used
				to define where the fill is (see Gavins docs).
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Reads the currently selected palette and builds internal tables based upon
				the results. Should be called after a palette change. Note that this may take
				a few seconds to execute and should be called sparingly.
	Errors:		-
	Scope:		Public
	SeeAlso:	SetTransparency;BuildTransparencyTable

********************************************************************************************/

BOOL GDrawAsm::SetTransparentGraduation4(
		DWORD Style,
		CONST TranspGradTable *Table,
		CONST POINT *PointA,
		CONST POINT *PointB,
		CONST POINT *PointC,
		CONST POINT *PointD
	)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	Style = Style & 0xFFFF00FF;
	if (Table->IsLarge())
		Style |= 0x0100;

	return (MakeCurrent() && (XaColour_SetTransparentGraduation4(m_pContext, Style, Table->GetTable(), PointA, PointB, PointC, PointD ) ? FALSE : TRUE));
}

/********************************************************************************************/

BOOL GDrawAsm::SetBias( UINT32 uChannel,double fBias )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetBias(m_pContext,uChannel,fBias) ? FALSE : TRUE);
}

/********************************************************************************************/

BOOL GDrawAsm::SetGain( UINT32 uChannel,double fGain )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetGain(m_pContext,uChannel,fGain) ? FALSE : TRUE);
}

/********************************************************************************************/

BOOL GDrawAsm::SetBrightness( double fBrightness )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetBrightness(m_pContext,fBrightness) ? FALSE : TRUE);
}

/********************************************************************************************/

BOOL GDrawAsm::SetContrast( double fContrast )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetContrast(m_pContext,fContrast) ? FALSE : TRUE);
}

/********************************************************************************************/

BOOL GDrawAsm::SetGamma( double fGamma )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetGamma(m_pContext,fGamma) ? FALSE : TRUE);
}

/********************************************************************************************/

BOOL GDrawAsm::SetPostGamma( double fPostGamma )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetPostGamma(m_pContext,fPostGamma) ? FALSE : TRUE);
}

/********************************************************************************************/

BOOL GDrawAsm::SetSaturation( double fSaturation )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetSaturation(m_pContext,fSaturation) ? FALSE : TRUE);
}

/********************************************************************************************/

BOOL GDrawAsm::SetContone( UINT32 uContoneStyle, COLORREF rgbStart, COLORREF rgbEnd )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetContone(m_pContext,uContoneStyle,rgbStart,rgbEnd) ? FALSE : TRUE);
}

/*******************************************************************************************/

BOOL GDrawAsm::SetInputRange( UINT32 uChannel, BYTE uStart, BYTE uEnd )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetInputRange(m_pContext,uChannel,uStart,uEnd) ? FALSE : TRUE);
}
/*******************************************************************************************/

BOOL GDrawAsm::SetOutputRange( UINT32 uChannel, BYTE uStart, BYTE uEnd )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() && (XaBitmap_SetOutputRange(m_pContext,uChannel,uStart,uEnd) ? FALSE : TRUE);
}

/*******************************************************************************************/

BOOL GDrawAsm::SetBitmapFill( LPBITMAPINFOHEADER bmInfo, LPBYTE lpBytes, DWORD Style, LPPOINT Points,
									COLORREF DefaultColour, LPRGBQUAD ColourTable,
									LPBYTE Red, LPBYTE Green, LPBYTE Blue, LPBYTE Trans, DWORD Offset)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetTilePattern(m_pContext, bmInfo, lpBytes, Style, Points, Points+1, Points+2,
									DefaultColour, (CONST BGRT*)ColourTable, Red, Green, Blue, NULL, Offset)
				? FALSE : TRUE));
}

BOOL GDrawAsm::SetTransparentBitmapFill( LPBITMAPINFOHEADER bmInfo, LPBYTE lpBytes, DWORD Style, LPPOINT Points,
											COLORREF DefaultColour,  BYTE* pTable, DWORD Offset)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetTransparentTilePattern(m_pContext, bmInfo, lpBytes, Style, Points, Points+1, Points+2,
												DefaultColour, pTable, Offset)
				? FALSE : TRUE));
}

BOOL GDrawAsm::SetPerspectiveBitmapFill( LPBITMAPINFOHEADER bmInfo, LPBYTE lpBytes, DWORD Style, LPPOINT Points,
										COLORREF DefaultColour, LPRGBQUAD ColourTable,
										LPBYTE Red, LPBYTE Green, LPBYTE Blue, LPBYTE Trans, DWORD Offset)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetTilePattern4(m_pContext, bmInfo, lpBytes, Style, Points, Points+1, Points+2, Points+3,
									DefaultColour, (CONST BGRT*)ColourTable, Red, Green, Blue, Trans, Offset)
				? FALSE : TRUE));
}

BOOL GDrawAsm::SetPerspectiveTransparentBitmapFill( LPBITMAPINFOHEADER bmInfo, LPBYTE lpBytes, DWORD Style, LPPOINT Points,
													COLORREF DefaultColour, BYTE* pTable, DWORD Offset)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetTransparentTilePattern4(m_pContext, bmInfo, lpBytes, Style, Points, Points+1, Points+2, Points+3,
												DefaultColour, pTable, Offset)
				? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::SetTransparencyLookupTable(CONST BYTE * Table)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/11/94
	Inputs:		Pointer to a table
	Outputs:	GDraw set up
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets the table for type 12 transparency
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/
BOOL GDrawAsm::SetTransparencyLookupTable(CONST BYTE * Table)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetTransparencyLookupTable(m_pContext,Table)
				? FALSE : TRUE));
}


///////////////////////////
// rendering
///////////////////////////

/********************************************************************************************

>	BOOL GDrawAsm::StrokePath( LPPOINT Points, const BYTE *Verbs, DWORD Len, BOOL Close,
								DWORD LineWidth, CapStyles LineCaps, JoinStyles LineJoin,
								CONST DashType *Dash )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Points is the list of co-ords of the path. Verbs is the PC_xx verb list.
				Len is how many points there are. Close controls whether open paths are
				closed. Dash can be NULL for solid lines.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Strokes the path using the current fill style. Zero width lines are plotted
				as single pixel lnies and never anti-aliased.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::StrokePath( LPPOINT Points, const BYTE *Verbs, UINT32 Len, BOOL Close,
								DWORD LineWidth, CapStyles LineCaps, JoinStyles LineJoin,
								CONST DashType *Dash )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_StrokePath(m_pContext, Points, Verbs, Len, FALSE != Close, LineWidth, 
		LineCaps, LineJoin, Dash ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::FillPath( LPPOINT Points, const BYTE *Verbs, size_t Length, DWORD Winding )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Points is the list of co-ords of the path. Verbs is the PC_xx verb list.
				Length is how many points there are.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Fills the path using the current fill style.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::FillPath( LPPOINT Points, const BYTE *Verbs, UINT32 Length, DWORD Winding )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_FillPath(m_pContext, Points, Verbs, Length, Winding ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::GetStatistics( LPPOINT Points, const BYTE *Verbs, size_t Length, DWORD Winding,
								  STATISTICS * Stats )

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		Points is the list of co-ords of the path. Verbs is the PC_xx verb list.
				Length is how many points there are.
	Outputs:	Stats filled in with statistics
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Fills the path counting various things about it
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::GetStatistics( LPPOINT Points, const BYTE *Verbs, UINT32 Length, DWORD Winding,
							  STATISTICS * Stats )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && ((XaDraw_GetStatistics(m_pContext, Points, Verbs, Length, Winding, Stats )) ? (FALSE || 1 /** BODGE - GDRAW BROKEN **/) : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::FillRectangle( LPRECT Rect )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Rect is the rectangle.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Fills the rectangle with the current fill style.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::FillRectangle( LPRECT Rect )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_FillRectangle(m_pContext, Rect ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::FillPoint( LPPOINT Point )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		Point is a coord.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets one device pixel to the curent fill colour.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::FillPoint( LPPOINT Point )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_FillPoint(m_pContext, Point ) ? FALSE : TRUE));
}

/********************************************************************************************

>	INT32 GDrawAsm::StrokePathToPath(
		CONST POINT *IPoints,
		CONST BYTE  *ITypes,
		DWORD ILength,
		POINT *OPoints,
		BYTE  *OTypes,
		DWORD OLength,
		BOOL Close,
		DWORD LineWidth,
		CapStyle LineCaps,
		JoinStyle LineJoin,
		CONST DashType *Dash)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		A lot. See Gavins docs.
	Outputs:	-
	Returns:	Length of the resulting path, or -1L if errored (or maybe zero?).
	Purpose:	Converts a path into another path by sort-of stroking in toto another data
				structure.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

INT32 GDrawAsm::StrokePathToPath(
		CONST POINT *IPoints,
		CONST BYTE  *ITypes,
		DWORD ILength,
		POINT *OPoints,
		BYTE  *OTypes,
		DWORD OLength,
		BOOL Close,
		DWORD LineWidth,
		CapStyles LineCaps,
		JoinStyles LineJoin,
		CONST DashType *Dash)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent()) return -1;
	return XaDraw_StrokePathToPath(m_pContext, IPoints, ITypes, ILength, OPoints, OTypes, OLength,
									FALSE != Close, LineWidth, LineCaps, LineJoin, Dash );
}

/********************************************************************************************

>	BOOL GDrawAsm::ClipRectangle( LPRECT Rect )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/94
	Inputs:		A rectangle (in logical coordinates).
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets clip rectangle of subsequent rendering.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::ClipRectangle( LPRECT Rect )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_ClipRectangle(m_pContext, Rect ) ? FALSE : TRUE));
}

/********************************************************************************************

>	void GDrawAsm::ClearChangedBBox()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Resets 'changed' rectangle. Should be called before anything is drawn,
				then drawn areas will be accumulated into the BBox.
	Errors:		-
	Scope:		Public
	SeeAlso:	GetChangedBBox

********************************************************************************************/

void GDrawAsm::ClearChangedBBox()
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (MakeCurrent()) XaDraw_ClearChangedBBox(m_pContext);
}

/********************************************************************************************

>	BOOL GDrawAsm::GetChangedBBox( LPRECT Rect )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/94
	Inputs:		-
	Outputs:	A rectangle in pixel co-ordinates.
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Gets size of 'changed' rectangle where pixels have been plotted.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::GetChangedBBox( LPRECT Rect )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_GetChangedBBox(m_pContext, Rect ) ? FALSE : TRUE));
}

/********************************************************************************************

>	BOOL GDrawAsm::SetChangedBBox( LPRECT Rect )

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/94
	Inputs:		A rectangle in pixel co-ordinates.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets size of 'changed' rectangle where pixels have been plotted.
	Errors:		-
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL GDrawAsm::SetChangedBBox( LPRECT Rect )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_SetChangedBBox(m_pContext, Rect ) ? FALSE : TRUE));
}

COLORREF GDrawAsm::ConvertHSVtoRGB(COLORREF hsv)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	MakeCurrent(); // Oooerr, should really not call GDraw if this fails
	return XaColour_ConvertHSVtoRGB(hsv );
}

COLORREF GDrawAsm::ConvertRGBtoHSV(COLORREF rgb)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	MakeCurrent(); // Oooerr, should really not call GDraw if this fails
	return XaColour_ConvertRGBtoHSV( rgb );
}






BOOL GDrawAsm::SetMiterLimit( DWORD MiterLimit)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_SetMiterLimit(m_pContext,MiterLimit) ? FALSE : TRUE));
}

BOOL GDrawAsm::SetMatrixIntFlag( BOOL Flag)
{
	return FALSE;
}

BOOL GDrawAsm::SetDashAdjustmentFlag( BOOL Flag )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_SetDashAdjustmentFlag(m_pContext, FALSE != Flag) ? FALSE : TRUE));
}

BOOL GDrawAsm::SetInvert( DWORD Colour )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_SetInvert(m_pContext,Colour) ? FALSE : TRUE));
}

BOOL GDrawAsm::InitialiseWithPalette( const LOGPALETTE* Palette )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaColour_InitialiseWithPalette(m_pContext,Palette) ? FALSE : TRUE));
}

const BYTE* GDrawAsm::ReturnBrush()
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent()) return NULL;
	return XaColour_ReturnBrush(m_pContext);
}

const BYTE* GDrawAsm::ReturnBrushRGB()
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent()) return NULL;
	return XaColour_ReturnBrushRGB(m_pContext);
}


/********************************************************************************************

>	virtual BOOL GDrawAsm::SetSeparationTables(	const BGR *CyanSepTable = NULL,
												const BGR *MagentaSepTable = NULL,
												const BGR *YellowSepTable = NULL,
												const BGR *BlackSepTable = NULL,
												const BYTE *UnderColourRemovalTable = NULL,
												const BYTE *BlackGenerationTable = NULL)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96

	Inputs:		CyanSepTable, MagentaSepTable, YellowSepTable, BlackSepTable
					These point to separation tables for C,M,Y, and K
					Each table is an array of 256 BGR triples
					Pass in a NULL pointer to use the default (linear) tables

				UnderColourRemovalTable, BlackGenerationTable
					These point to tables specifying UCR/BG
					Each table is an array of 256 BYTEs
					Pass in a NULL pointer to use the default (linear) tables

	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.

	Purpose:	Sets process colour separation tables to be used in the future by
				various GDraw calls. See the GDraw documentation for details.

	Notes:		IMPORTANT: GDraw does not copy the tables - it retains the pointers
				that you pass in, so it is up to you to deallocate the memory (after
				restoring the separation tables to the defaults of course).

				Call with all parameters == NULL to restore the defaults.

********************************************************************************************/

BOOL GDrawAsm::SetSeparationTables(const BGR *CyanSepTable,
									const BGR *MagentaSepTable,
									const BGR *YellowSepTable,
									const BGR *BlackSepTable,
									const BYTE *UnderColourRemovalTable,
									const BYTE *BlackGenerationTable)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return(MakeCurrent() &&
			(XaColour_SetSeparationTables(m_pContext,CyanSepTable, MagentaSepTable,
										 YellowSepTable, BlackSepTable,
										 UnderColourRemovalTable, BlackGenerationTable)
			? FALSE : TRUE));
}

/********************************************************************************************

>	virtual BOOL GDrawAsm::SetBitmapConversionTable( const BGR* BitmapConversionTable )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96

	Inputs:		Ptr to the conversion table to use.

	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed.

	Purpose:	This function will set up the bitmap conversion table for use by the
				following plotting functions:

					XaColourOld_Set3WayGraduation
					XaColourOld_Set3WayGraduation4
					XaColourOld_Set4WayGraduation
					XaColourOld_Set4WayGraduation4
					XaColourOld_SetGourand
					XaColourOld_SetMultiRadial
					XaColourOld_SetTilePattern
					XaColourOld_SetTilePattern4
					XaSpriteOld_PlotTile
					XaSpriteOld_PlotTile4.

				This table is used to convert BGR values. It should be a 128K dword table
				indexed by a 17 bit index :
											bbbbbgggggrrrrr00.

				Bits must be set in the separation style of the above functions in order to
				make use of this table.

********************************************************************************************/

BOOL GDrawAsm::SetBitmapConversionTable( const BGR* BitmapConversionTable )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return(MakeCurrent() &&
			(XaColour_SetBitmapConversionTable(m_pContext,BitmapConversionTable)
			? FALSE : TRUE));
}


BOOL GDrawAsm::PlotBitmap( LPBITMAPINFOHEADER bmInfo, LPBYTE lpBytes, DWORD Style, LPPOINT Points,
									LPRGBQUAD ColourTable,
									LPBYTE Red, LPBYTE Green, LPBYTE Blue, LPBYTE Trans)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaBitmap_PlotTile(m_pContext, bmInfo, lpBytes, Style, Points, Points+1, Points+2,
									(CONST BGRT*)ColourTable, Red, Green, Blue, Trans)
				? FALSE : TRUE));
}

BOOL GDrawAsm::PlotPerspectiveBitmap( LPBITMAPINFOHEADER bmInfo, LPBYTE lpBytes, DWORD Style, LPPOINT Points,
									LPRGBQUAD ColourTable,
									LPBYTE Red, LPBYTE Green, LPBYTE Blue, LPBYTE Trans)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaBitmap_PlotTile4(m_pContext, bmInfo, lpBytes, Style, Points, Points+1, Points+2, Points+3,
									(CONST BGRT*)ColourTable, Red, Green, Blue, Trans)
				? FALSE : TRUE));
}

INT32 GDrawAsm::CalcStrokeBBox(
		CONST POINT *IPoints,
		CONST BYTE  *ITypes,
		DWORD ILength,
		LPRECT Rect,
		BOOL Close,
		DWORD LineWidth,
		CapStyles LineCaps,
		JoinStyles LineJoin,
		CONST DashType *Dash)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent()) return -1;
	return XaDraw_CalcStrokeBBox(m_pContext, IPoints, ITypes, ILength, Rect,
									FALSE != Close, LineWidth, LineCaps, LineJoin, Dash );
}

BOOL GDrawAsm::IsOverlap( LPPOINT Points, const BYTE *Verbs, UINT32 Length, DWORD Winding )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_IsOverlap(m_pContext, Points, Verbs, Length, Winding ) ? FALSE : TRUE));
}

BOOL GDrawAsm::CalcBBox( LPPOINT Points, const BYTE *Verbs, DWORD Length, LPRECT Rect, BOOL Flatten)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_CalcBBox(m_pContext, Points, Verbs, Length, Rect, FALSE != Flatten ) ? FALSE: TRUE));
}

INT32 GDrawAsm::MakeRegion( LPPOINT Points, const BYTE *Verbs, UINT32 Length, DWORD Winding, REGION *ORegion, UINT32 OLength)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent()) return -1;
	return XaDraw_MakeRegion(m_pContext, Points, Verbs, Length, Winding, ORegion, OLength );
}

BOOL GDrawAsm::DeviceClipRectangle( LPRECT Rect)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_DeviceClipRectangle(m_pContext, Rect ) ? FALSE: TRUE));
}

BOOL GDrawAsm::GetDeviceClipRectangle( LPRECT  Rect)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_GetDeviceClipRectangle(m_pContext, Rect ) ? FALSE: TRUE));
}

BOOL GDrawAsm::ClipRegion( const REGION* Region)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_ClipRegion(m_pContext, Region ) ? FALSE: TRUE));
}

CONST REGION* GDrawAsm::GetClipRegion()
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return MakeCurrent() ? XaDraw_GetClipRegion(m_pContext) : NULL;
}

BOOL GDrawAsm::HintPath(
		CONST POINT *Points,
		CONST BYTE  *Types,
		UINT32 Length,
		BOOL Close,
		DWORD LineWidth)
{
//	return (MakeCurrent() && (XaDraw_HintPath(m_pContext, Points, Types, Length, Close, LineWidth ) ? FALSE: TRUE));
	return FALSE;
}

INT32 GDrawAsm::MakeUnclippedRegion(
		CONST POINT *Points,
		CONST BYTE  *Types,
		UINT32 Length,
		DWORD Winding,
		REGION *ORegion,
		UINT32 OLength)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent()) return -1;
	return XaDraw_MakeUnclippedRegion(m_pContext, Points, Types, Length, Winding, ORegion, OLength );
}


BOOL GDrawAsm::FillRegion( CONST REGION *Region, CONST POINT *Offset )
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (XaDraw_FillRegion(m_pContext, Region, Offset ) ? FALSE: TRUE));
}

INT32 GDrawAsm::SetBevelContrast(UINT32 nContrast)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent())
		return -1;

	return XaDraw_SetBevelContrast(m_pContext,nContrast) ? FALSE : TRUE;
}

INT32 GDrawAsm::SetBevelLightness(UINT32 nContrast)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent())
		return -1;

	return XaDraw_SetBevelLightness(m_pContext,nContrast);
}

INT32 GDrawAsm::SetBevelDarkness(UINT32 nContrast)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent())
		return -1;

	return XaDraw_SetBevelDarkness(m_pContext,nContrast);
}

INT32 GDrawAsm::TranslateBevelValue(BYTE nIndex, BYTE Colour)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	if (!MakeCurrent())
		return -1;

	return XaDraw_TranslateBevelValue(m_pContext,nIndex, Colour);
}

BOOL GDrawAsm::Set3WayGraduation(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (
			XaColour_Set3WayGraduation(m_pContext,Style, ColourA, ColourB, ColourD, PointA, PointB, PointD)
			? FALSE : TRUE) );
}


BOOL GDrawAsm::Set3WayGraduation4(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (
			XaColour_Set3WayGraduation4(m_pContext,Style, ColourA, ColourB, ColourD, PointA, PointB, PointC, PointD)
			? FALSE : TRUE) );
}


BOOL GDrawAsm::Set4WayGraduation(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (
			XaColour_Set4WayGraduation(m_pContext,Style, ColourA, ColourB, ColourC, ColourD, PointA, PointB, PointD)
			? FALSE : TRUE) );
}


BOOL GDrawAsm::Set4WayGraduation4(DWORD Style,
		COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (
			XaColour_Set4WayGraduation4(m_pContext,Style, ColourA, ColourB, ColourC, ColourD, PointA, PointB, PointC, PointD)
			? FALSE : TRUE) );
}


BOOL GDrawAsm::SetTransparent3WayGraduation(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (
			XaColour_SetTransparent3WayGraduation(m_pContext,Style, ValueA, ValueB, ValueD, PointA, PointB, PointD)
			? FALSE : TRUE) );
}


BOOL GDrawAsm::SetTransparent3WayGraduation4(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (
			XaColour_SetTransparent3WayGraduation4(m_pContext,Style, ValueA, ValueB, ValueD, PointA, PointB, PointC, PointD)
			? FALSE : TRUE) );
}


BOOL GDrawAsm::SetTransparent4WayGraduation(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueC, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointD)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (
			XaColour_SetTransparent4WayGraduation(m_pContext,Style, ValueA, ValueB, ValueC, ValueD, PointA, PointB, PointD)
			? FALSE : TRUE) );
}


BOOL GDrawAsm::SetTransparent4WayGraduation4(DWORD Style,
		BYTE ValueA, BYTE ValueB, BYTE ValueC, BYTE ValueD,
		CONST POINT *PointA, CONST POINT *PointB, CONST POINT *PointC, CONST POINT *PointD)
{
	CriticalSection Ralph;
	CamProfile cp(CAMPROFILE_GDRAW);

	return (MakeCurrent() && (
			XaColour_SetTransparent4WayGraduation4(m_pContext,Style, ValueA, ValueB, ValueC, ValueD, PointA, PointB, PointC, PointD)
			? FALSE : TRUE) );
}
