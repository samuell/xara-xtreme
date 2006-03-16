// $Id: plugop.h 662 2006-03-14 21:31:49Z alex $
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

// OpDescriptor, Operations and Context sensitive menu for plug-ins

#ifndef INC_PLUGINOP
#define INC_PLUGINOP

#include "contmenu.h"
#include "ops.h"

class KernelBitmap;
/********************************************************************************************

>	class PlugInOp : public Operation

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/96
	Purpose:	Operation so that a plug-ins can be invoked and used.  

********************************************************************************************/				

#define OPTOKEN_PLUGINS				_T("PlugIns")

class PlugInOp : public Operation
{
	CC_DECLARE_DYNCREATE( PlugInOp );

public:

	PlugInOp();

	virtual void Do(OpDescriptor*);

	static BOOL Init();
	static BOOL RegisterOpToken(TCHAR *OpToken, const String_64& NewMenuText,
								CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInOp),
								pfnGetState gs = GetState);
	static BOOL RegisterOpToken(TCHAR *OpToken, UINT32 MenuTextID,
								CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInOp),
								pfnGetState gs = GetState);
	static OpState GetState(String_256*, OpDescriptor*);	

private:
};


/********************************************************************************************

>	class PlugInsContextMenu : public ContextMenu

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/11/96
	Purpose:	Implements the plug-ins context sensitive menus

	SeeAlso:	ContextMenu

********************************************************************************************/

class PlugInsContextMenu : public ContextMenu
{
	CC_DECLARE_MEMDUMP(PlugInsContextMenu);

public:
	PlugInsContextMenu();
			// Default constructor - DO NOT CALL this constructor - use the one below
	PlugInsContextMenu(KernelBitmap* pBitmap, Document * pDocument);

	virtual BOOL Build(void);
			// Builds a pop-up menu over the gallery list

	static BOOL BuildMenu(KernelBitmap * pBitmap, Document * pDocument,
						  ContextMenu * pContextMenu, MenuItem* pMainRoot = NULL,
						  BOOL AddSeparator = FALSE, BOOL UseUndoable = FALSE);
protected:
	// As we are invoked from a menu item, we need to be told what bitmap we are
	// working on, so these need to be set up before the operation is invoked.
	KernelBitmap * m_pBitmap;
	Document * m_pDocument;
};

/********************************************************************************************

>	class PlugInOpDescriptor : public OpDescriptor

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Purpose:	New PlugInOpDescriptor class whose purpose in life is to allow an OpDescriptor
				to be created which instead of using a resource id to get the menu item/operation
				item text, uses a string resource. This then means we can use the name from the
				plug-in instead of having to resource all names!

********************************************************************************************/

class PlugInOpDescriptor : public OpDescriptor
{
	CC_DECLARE_DYNAMIC( PlugInOpDescriptor );

public:
	PlugInOpDescriptor(const TCHAR* pcszToken, const String_64& NewMenuText,
								CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInOp),
								pfnGetState gs = PlugInOp::GetState);

	// This will use the TextID to obtain a string resource text description of the 
	// operation. 
	virtual BOOL GetText(String_256* Description, OpTextFlags WhichText);

	static OpState GetState(String_256* psName, OpDescriptor* pOpDesc);

protected:
	// Replaces MenuID resource
	String_64 m_MenuText;
};

#endif // INC_PLUGINOP
