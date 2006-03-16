// $Id: viewmod.cpp 673 2006-03-15 15:34:39Z alex $
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
#include "nodecont.h"
#include "tool.h"
#include "viewmod.h"
//#ifndef _AFXDLL
// tools that can go in DLLs go here
#include "pushtool.h"
#include "zoomtool.h"
#include "freehand.h"
//#include "pentool.h"
#include "blndtool.h"
#include "beztool.h"
#include "filltool.h"
//#include "gridtool.h"
#include "regshape.h"
#include "rectangl.h"
#include "moldtool.h"
//#include "texttool.h"
#include "shadtool.h"	// shadow tool header, Olivier 29/10/96
//#include "blnktool.h"
#include "cntrtool.h"  // contour tool, DavidM 24/8/99
//#endif
// tools that cannot go in DLLs go here
#include "selector.h"
#include "eliptool.h"
#include "bevtool.h"
//#include "slicetool.h"
//#include "liveeffectstool.h"


static char Name[] 		= "Kernel Module";
static char Purpose[] 	= "Tools that need to be in the kernel";
static char Author[] 	= "The Nasty People";

DECLARE_SOURCE("$Revision: 673 $");



// this macro makes it easier to move tools from the .exe to the .dll
// in AFXDLL builds, the macro returns NULL, otherwise it does a 'new classtype'

//#if _AFXDLL
//#define	IN_DLL(classname)	NULL
//#else
#define	IN_DLL(classname)	new classname
//#endif

#define	NOT_IN_DLL(classname)	new classname

/********************************************************************************************

>	virtual BOOL ViewModule::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialise the view module (which contains the Push and Zoom tools).
				Does nothing at the moment.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ViewModule::Init()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
#ifdef STANDALONE
	TextTool* pTextTool = new TextTool();
	if (pTextTool)
		pTextTool->Init();
#endif
#endif

	return TRUE;
}
	
/********************************************************************************************

>	virtual void ViewModule::Describe(void* Info)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Get info on the Module.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ViewModule::Describe(void* Info)
{
	// Cast the pointer into one we understand...
	
	ModInfo_v1 *ModInfo = (ModInfo_v1 *) Info;
	
	// ...and fill in the fields
	
	ModInfo->InfoVersion = 1;
	ModInfo->InterfaceVersion = GetInterfaceVersion();
	ModInfo->Version = 1;
	ModInfo->ID = MODULEID_KERNEL;
#ifdef STANDALONE
	ModInfo->NumTools = 2;
#else
#ifdef BUILDSHADOWS
	ModInfo->NumTools = 21; // Shadows + Bevels + Contours + Slicing
#else
	ModInfo->NumTools = 18; // Shadows + Bevels + Contours + Slicing
#endif
#endif
	ModInfo->Name = Name;
	ModInfo->Purpose = Purpose;
	ModInfo->Author = Author;
}
	

/********************************************************************************************

>	virtual void *ViewModule::CreateTool(UINT32 Type)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Create an instance of a given tool.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void *ViewModule::CreateTool(UINT32 Type)
{
	switch (Type)
	{
		case 1:
			return IN_DLL(ZoomTool);
			
		case 2:
			return IN_DLL(PushTool);

#ifndef STANDALONE
		case 3:
			return IN_DLL(RectangleTool);

#ifndef WEBSTER	//Martin-06/11/96-begin
		case 4:
			return IN_DLL(FreeHandTool);
#endif	//WEBSTER

		case 5:
			return NOT_IN_DLL(SelectorTool);

PORTNOTE("other","Removed grid tool usage")
#ifndef EXCLUDE_FROM_XARALX
#ifndef WEBSTER	//Martin
		case 6:
			return IN_DLL(GridTool);

#endif	//WEBSTER
#endif
		case 7:
			return IN_DLL(BezierTool);

		case 8:
			return NOT_IN_DLL(EllipseTool);

		case 9:
			return IN_DLL(GradFillTool);

#ifndef WEBSTER	//Martin
PORTNOTE("other","Removed pen tool usage")
#ifndef EXCLUDE_FROM_XARALX
		case 10:
			return IN_DLL(PenTool);
#endif

		case 11:
			return IN_DLL(BlendTool);
#endif	//WEBSTER

		case 12:
			return IN_DLL(TranspTool);

		case 13:
			return IN_DLL(QuickShapeTool);

#ifndef WEBSTER	//Martin
		case 14:
			return IN_DLL(MouldTool);

PORTNOTE("other","Removed blank tool usage")
#ifndef EXCLUDE_FROM_XARALX
		case 15:
			return IN_DLL(BlankTool);
#endif
#endif	//WEBSTER-Martin-06/11/96-end

PORTNOTE("other","Removed text tool usage")
#ifndef EXCLUDE_FROM_XARALX
		case 16:
			return IN_DLL(TextTool);
#endif

#ifndef NO_ADVANCED_TOOLS
PORTNOTE("other","Removed effects tool usage")
#ifndef EXCLUDE_FROM_XARALX
		case 17:
			return IN_DLL(LiveEffectsTool);
#endif

		case 18:
			return IN_DLL(SoftShadowTool);

		case 19:
			return IN_DLL(BevelTool);

		case 20:
			return IN_DLL(ContourTool);
#endif

PORTNOTE("other","Removed slice tool usage")
#ifndef EXCLUDE_FROM_XARALX
		case 21:
			return IN_DLL(SliceTool);
#endif

		// Add more view tools here...

#endif
	}
	
	return NULL;
}


