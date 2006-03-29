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

// DLL-end of push tool DLL

#include "camtypes.h"
#include <afxdllx.h>
#include "module.h"
#include "errors.h"
#include "pushtool.h"
//#include "zoomtool.h"
//#include "freehand.h"
#include "pentool.h"
//#include "blndtool.h"
#include "beztool.h"
#include "filltool.h"
#include "gridtool.h"
#include "rectangl.h"
#include "regshape.h"
#include "moldtool.h"
#include "blnktool.h"

#include "ezmodule.h"

#if 0
#ifndef WIN32
#error 32-bit only - why are you compiling me?
#endif
static AFX_EXTENSION_MODULE extensionDLL;

extern "C" INT32 APIENTRY
DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID)
{

	if (dwReason==DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(extensionDLL, hInstance))
			return 0;
	}
	return 1;
}

extern "C" __declspec( dllexport ) UINT32 GetNumberOfModules()
{
	// this is required to link us into a load of MFC stuff
	new CDynLinkLibrary(extensionDLL);

	return 1;
}
#endif

static char Name[] 		= "Push Module";
static char Purpose[] 	= "To do push type stuff";
static char Author[] 	= "Mr DLL Pennell";

class PushModule : public Module_v1
{
public:
	// Initialise the module
	virtual BOOL Init();
	
	// What version of the Module interface does this module understand?
	virtual UINT32 GetInterfaceVersion() { return 1; } ;
	
	// What version of the Module info class does this module understand?
	virtual UINT32 GetInfoVersion() { return 1; };
	
	// Get info on the Module.
	virtual void Describe(void* Info);
	
	// Create an instance of a given tool.
	virtual void *CreateTool(UINT32 ToolID);

};

BOOL PushModule::Init()
{
	return TRUE;
}

void PushModule::Describe(void* Info)
{
	// Cast the pointer into one we understand...
	
	ModInfo_v1 *ModInfo = (ModInfo_v1 *) Info;
	
	// ...and fill in the fields
	
	ModInfo->InfoVersion = 1;
	ModInfo->InterfaceVersion = GetInterfaceVersion();
	ModInfo->Version = 1;
	ModInfo->ID = MODULEID_PUSH;
	ModInfo->NumTools = 13;
	ModInfo->Name = Name;
	ModInfo->Purpose = Purpose;
	ModInfo->Author = Author;
}
	


void *PushModule::CreateTool(UINT32 Type)
{
	if (Type==1)
		return NULL; //new ZoomTool;

	if (Type==2)
		return new PushTool;

	if (Type==3)
		return NULL; //new FreeHandTool;

	if (Type==4)
		return new PenTool;

	if (Type==5)
		return NULL; //new BlendTool;

	if (Type==6)
		return new BezierTool;

	if (Type==7)
		return new GradFillTool;

	if (Type==8)
		return new TranspTool;

	if (Type==9)
		return new GridTool;

	if (Type==10)
		return new RectangleTool;

	if (Type==11)
		return new QuickShapeTool;

	if (Type==12)
		return new MouldTool;

	if (Type==13)
		return new BlankTool;

	return NULL;
}			



#if 0
extern "C" __declspec( dllexport ) Module_v1 *CreateModule(UINT32 which)
{
	return new PushModule();
}

#ifdef _DEBUG
extern "C" __declspec( dllexport ) void IAmADebugTool()
{
}
#endif

#endif

IMPLEMENT_SIMPLE_MODULE_OIL( PushModule )
