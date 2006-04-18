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

// This handles the machine-specific areas regarding modules & tools and how they link
// in to the kernel. The Kernel itself maintains a list of modules & tools, but we are
// responsible for physically finding modules (in .DLLs) and declaring their presence
// to everyone else.

// See winoil\killdlls.cpp for the _ADXDLL build vsn of Deinit() method of OILModule.

#include "camtypes.h"

#include "module.h"
#include "oilmods.h"
#include "tool.h"
#include "modlist.h"
//#include "dll_lib.h"
//#include "andy.h"
#include "errors.h"
#include "viewmod.h"
//#include "zoomres.h"

DECLARE_SOURCE("$Revision$");

// This list is always needed as RSN DLLs will hold resources even if the tool code
// itself is in the .EXE

HINSTANCE DLLs[MAX_DLLS];
UINT32 DLLCount = 0;

// This list is for other DLLs that MUST be closed upon termination
HINSTANCE ExtraDLLs[Extra_DLL_Count];

#define	TOOL_DLL_NAME	"TOOLS.DLL"


/********************************************************************************************

>	OILModule::Init()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Called to supply all modules to the Kernel. For each module that is found,
				calls Modules::DeclareModule(). Note that at this point only the constructors
				on each module are called; their Init functions are not. This allows one
				module to detect the presence of another module and fail its Init call.
	Errors:		None

********************************************************************************************/

// RALPH
#ifdef RALPH
	#define NOMODULE 
#else
	#ifndef _AFXDLL
		#define	NOMODULE
	#endif
#endif

//#ifndef _AFXDLL   

#ifdef NOMODULE
/********************************************************************************************

	Kludged built in to .exe file Tool code:

********************************************************************************************/

BOOL OILModule::Init()
{
// The Viewer, and Mac version always read from the exe
#ifdef STANDALONE
//	#define _READFROMEXE
#endif

#ifndef _READFROMEXE
	#ifdef _MAC
		#define _READFROMEXE
	#endif
#endif

	HINSTANCE hLibrary = (HINSTANCE)HINSTANCE_ERROR;

#pragma message( __LOCMSG__ "Removed SetErrorMode usage" )
//	SetErrorMode( SEM_NOOPENFILEERRORBOX );						// don't cause errors
	
	// Still need the DLLs for the module/tool resources.

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Have a look to see if the tools resources are in the normal ones...
	HINSTANCE hResInst = AfxGetResourceHandle();
	TCHAR Buf[200];
	INT32 LoadStringResult = ::LoadString( hResInst, _R(IDS_ZOOMOP_STATUSHELP), Buf, 200);
	if(LoadStringResult)
	{
		// if(ExtraDLLs[Resources_DLL] != 0)
		// We don't want to load the tools.dll if we're using an external resource
		// dll anyway, since it should already have these resources in it...
		hLibrary = (HINSTANCE)HINSTANCE_ERROR; //AfxGetInstanceHandle();
	}
	else
	{
#ifndef _READFROMEXE
		LPTCHAR LibName = wxT(TOOL_DLL_NAME);
		hLibrary = LoadLibrary( LibName );

		// for now the tools DLL is compulsory
		if (hLibrary < (HINSTANCE)HINSTANCE_ERROR)
		{
	/*		TCHAR loaded[256], buf[256];

			if (::LoadString( AfxGetResourceHandle(), _R(IDW_NOTOOLS), loaded, sizeof(loaded) ) )
			{
				wsprintf(buf, loaded, LibName );				// assumes %s in message
				Error::SetError(0, buf, 0);						// leave InformError for caller
			}
			return FALSE;		
	*/
			// Rewrite the above to use MakeMsg, so the context-senstive help will work.
			String_256 buf;
			buf.MakeMsg(_R(IDW_NOTOOLS), (LPCTSTR) LibName);
			Error::SetError(0, buf, 0);						// leave InformError to caller
			return FALSE;
		}
#else
		// Mac version cannot open DLLs
		hLibrary = AfxGetInstanceHandle();						// read resource from us
#endif
	}
//#else
//	hLibrary = AfxGetInstanceHandle();						// read resource from us
#endif

	// Bodge the DLL table
	DLLs[0] = hLibrary;
	DLLCount++;
	
	Module *NewModule = (Module *) new ViewModule;
	if (NewModule == NULL)
		goto AbnormalExit;
	
	// Construct an OILModule and make it point to this DLL for the module's resources.
	OILModule *pOILModule;
	pOILModule = new OILModule;
	if (pOILModule == NULL)
	{
		delete NewModule;
		goto AbnormalExit;
	}
	pOILModule->hInstance = hLibrary;
	
	// Declare this module.
	Module::Declare(NewModule, pOILModule);

	// Here you add your own built-in tools for in .exe testing - when in a DLL,
	// you can't do this.

	return TRUE;
	
AbnormalExit:
	// Out of memory - clean up and fail
	TRACEUSER( "Tim", _T("OILModule::Init(): memory exhausted") );
#pragma message( __LOCMSG__ "Removed SetErrorMode usage" )
//	FreeLibrary(hLibrary);
	DLLs[0] = (HINSTANCE)HINSTANCE_ERROR;
	DLLCount--;
	return FALSE;
}

#else



/********************************************************************************************

	Proper DLL-reading code:

********************************************************************************************/


/********************************************************************************************

	void OILModule::EnumerateToolsFromDLL(const char *Filename)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/93
	Inputs:		Filename - filename of DLL to load and interrogate.
	Outputs:	-
	Returns:	-
	Purpose:	Loads a Windows DLL, finds out how many modules it contains, and tries
				to create (and declare) each one in turn.
				It detects debug vs non-debug build clashes and ignores modules that are
				not from the same build.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

typedef Module *(MODULE_FN *CreateProcType)(UINT32);
typedef	UINT32 ((MODULE_FN * CountProcType)());						// ptr to CountModules fn
typedef void *((MODULE_FN *DebugProcType)());						// ptr to Debug marker fn

void OILModule::EnumerateModulesFromDLL(const char *Filename)
{
	CreateProcType CreateProc;							// ptr to CreateModule fn
	CountProcType CountProc;							// ptr to CountModules fn
	DebugProcType DebugProc;							// ptr to Debug marker fn
	
	ModuleListItem *NewItem = NULL;						// ptr to new Module list item
	HINSTANCE hLibrary;									// handle of DLL


	if (DLLCount == MAX_DLLS)							// stop when limit reached
	{
		TRACE( _T("Too many DLLs\n"));
		return;
	}

	hLibrary = LoadLibrary(Filename);					// load .DLL


	if (hLibrary <= (HINSTANCE)HINSTANCE_ERROR)
	{
		ERROR3_PF( ("Tools DLL %s failed to load - may be problems starting up", Filename) );
		return;											// ignore if not found
	}

	// Read the function pointers from the DLL. (Lots of lovely casts required).

#if WIN32
	CountProc = (CountProcType)	 GetProcAddress(hLibrary, "GetNumberOfModules");
	CreateProc= (CreateProcType) GetProcAddress(hLibrary, "CreateModule");
#else
	CountProc = (CountProcType)	 GetProcAddress(hLibrary, "_GetNumberOfModules");
	CreateProc= (CreateProcType) GetProcAddress(hLibrary, "_CreateModule");
#endif
	DebugProc = (DebugProcType)	 GetProcAddress(hLibrary, "IAmADebugTool");

	// Construct an OILModule and make it point to this DLL for the module's resources.
	OILModule *pOILModule = new OILModule;
	if (pOILModule == NULL)
		// Out of memory
		TRACE( _T("OILModule::EnumerateModulesFromDLL(): memory exhausted"));
	else
		pOILModule->hInstance = hLibrary;

	if (pOILModule)
	{
		if (CreateProc && CountProc)
		{
			// ensure that debug status of DLL matches the .exe file
			#ifdef _DEBUG
			if (DebugProc)
			#else
			if (!DebugProc)
			#endif
			{
				// Fill in the instance handle so the Tool (or, more accurately, the
				// OILTool) can use it when initialising.
				DLLs[DLLCount++] = hLibrary;
				
				// Ask about Modules in this DLL

				UINT32 i;
				const UINT32 ModuleCount = CountProc();				// ask DLL how many

				for (i = 1; i <= ModuleCount; i++)					// tool numbers start at 1
				{
					Module *NewModule = CreateProc(i);				// ask about each Tool

					if (NewModule)
						NewItem = Module::Declare(NewModule, 
												  pOILModule);		// tell Kernel about it
				}
			}
		}
		else if (camStrcmp( Filename, TOOL_DLL_NAME )==0 )
		{
			// loaded resources from the DLL, now declare the built-in Tools
			Module *NewModule = (Module*) new ViewModule;

			if (NewModule)
				NewItem = Module::Declare(NewModule, pOILModule );
		}
	}

	// Did the DLL provide any modules? (or did we run out of memory?)
	if (NewItem == NULL)
	{
		// No - so forget about this DLL
		DLLs[DLLCount] = (HINSTANCE)HINSTANCE_ERROR;
		DLLCount--;											
		FreeLibrary(hLibrary);
		ERROR3_PF( ("Tools DLL %s failed to init - may be problems starting up", Filename) );
	}
	else
	{
		TRACEUSER( "Andy", _T("%08lx=%s\n"), hLibrary, Filename );
	}
}

/********************************************************************************************

>	BOOL OILModule::Init()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if OK, FALSE if serious fatal error.
	Purpose:	Under Windows, reads all Module DLLs and declares the modules found therein
				to the Kernel.
	Errors:		None.
	SeeAlso:	Tool::InitTools
	Scope:		Private

********************************************************************************************/


BOOL OILModule::Init()
{
	// At some point we will have a smart routine for finding all .DLLs. For now,
	// we keep life a little simpler...

	SetErrorMode( SEM_NOOPENFILEERRORBOX );						// don't cause errors


	// read built-in tools and their resources
	EnumerateModulesFromDLL( TOOL_DLL_NAME );

	// read from the external DLL(s)
	EnumerateModulesFromDLL( "Push.dll" );
	EnumerateModulesFromDLL( "ZoomTool.dll" );
	EnumerateModulesFromDLL( "FreeHand.dll" );
	EnumerateModulesFromDLL( "BlndTool.dll" );


	return TRUE;												// always OK
}



#endif



/********************************************************************************************

>	HINSTANCE OILModule::GetInstance(UINT32 ModuleID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		ModuleID - ID of the module to get the instance for.
	Outputs:	-
	Returns:	The HINSTANCE value for the required module, or HINSTANCE_ERROR if the ID
				was not found.
	Purpose:	Given a module ID, return the Windows HINSTANCE handle for the Windows
				'module' containing this Camelot module.
	Errors:		-

********************************************************************************************/

HINSTANCE OILModule::GetInstance(UINT32 ModuleID)
{
	// Find the module in question
	ModuleListItem *Item = Module::Find(ModuleID);
	
	// Extract the instance if the module was found
	if (Item == NULL)
	{
		TRACE( _T("OILModule::GetInstance() : ModuleID (%d) not found\n"), ModuleID);
		return (HINSTANCE)HINSTANCE_ERROR;
	}
	else	
		return (Item->m_pOILModule->GetInstance());
}

/********************************************************************************************

>	OILModule::OILModule(UINT32 ModuleID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Provides access to the OIL specific features of a module.
	Errors:		-
	SeeAlso:	GetInstance

********************************************************************************************/


OILModule::OILModule()
{
	// Initialise to a value that can be easily recognised as invalid.
	hInstance = (HINSTANCE)HINSTANCE_ERROR;
}

/********************************************************************************************

>	HINSTANCE OILModule::GetInstance()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Returns the HINSTANCE of the DLL that this module's resources are in.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


HINSTANCE OILModule::GetInstance()
{
	return hInstance;
}
