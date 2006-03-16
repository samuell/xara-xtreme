// $Id: tooldlg.h 662 2006-03-14 21:31:49Z alex $
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
//
// Header file for ToolDlg.CPP
//
// This file implements the dialog box that allows the user to configure which
// toolbars are shown around the edges of the main frame window.

#ifndef INC_TOOLDLG
#define INC_TOOLDLG

//#include "dialogop.h"
//#include "bars.h"
//#include "resource.h"
//#include "mario.h"

/******************************************************************************************

>	class ToolbarDlg: public DialogOp

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Purpose:	The Toolbars... dialog allows a user to 

				View all defined toolbars
				Create a new toolbar
				Delete a toolbar
				Make docking bars show large or small buttons
				
				
******************************************************************************************/

#define OPTOKEN_TOOLBARDLG _T("ToolbarDlg")

class ToolbarDlg: public DialogOp
{
	CC_DECLARE_DYNCREATE( ToolbarDlg )

public:
	ToolbarDlg();  						// Constructor 
	~ToolbarDlg();
	void Do(OpDescriptor*);				// "Do" function
	static BOOL Init();         		// Setup function
	BOOL Create();						// Create this dialog
	static OpState GetState(String_256*, OpDescriptor*);
										// Return the state of this operation
	virtual MsgResult Message(Msg* Message);
										// Message handler
	void ShowToolbarList(); 			// Reflect the current state of bars in the dialog
	void ShowToolbarSizes(); 			// Reflect the current state of bars in the dialog

	static ToolbarDlg* GetToolbarDlg();		// Get pointer to current toolbar dialog

// Public data members...
	static const INT32 IDD;
	static const CDlgMode Mode;			// ?

private:
	BOOL InitDialog(); 					// Standard setup function
	void DeleteSelectedBar();
	void TakeToolbarDetails();			// Set the current state of bars from the dialog
	void NewToolbar();					// Create a new toolbar and set it up

	void GetSelectedBarName(String_32 *Result);	// Get Name of selected bar

private:
	static ToolbarDlg* CurrentToolbarDlg;	// Pointer to current ToolbarDlg
	static String_16 InfoBarName;		// Internationalised name of infobar
	static String_16 ControlBankName;		// Internationalised name of ControlBank
	BOOL CanDeleteSelection();
};




/******************************************************************************************

>	class ToolnameDlg: public DialogOp

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Purpose:	The "New toolbar" dialog allows a user to 
				Create a new, named toolbar
				
				
******************************************************************************************/

#define OPTOKEN_TOOLBARNAMEDLG _T("ToolbarNameDlg")

class ToolnameDlg: public DialogOp
{
	CC_DECLARE_DYNCREATE( ToolnameDlg )

public:
	ToolnameDlg();					// Constructor 

	void Do(OpDescriptor*);			// "Do" function
	static BOOL Init();         	// Setup function
	BOOL Create();					// Create this dialog
	static OpState GetState(String_256*, OpDescriptor*);
									// Return the state of this operation
	virtual MsgResult Message(Msg* Message);
									// Message handler

// Public data members...
	static const INT32 IDD;
	static const CDlgMode Mode;		// ?
	
private:
	BOOL InitDialog(); 				// Standard setup function

//	DialogOp*	pToolbarDlg;

};




/******************************************************************************************

>	class CustomizeBarDlg: public DialogOp

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Purpose:	The "Customize toolbar" dialog allows a user to 
				Edit the controls on a toolbar
				
				
******************************************************************************************/

#define OPTOKEN_CUSTOMIZEBARDLG _T("ToolbarNameDlg")

class CustomizeBarDlg: public DialogOp
{
	CC_DECLARE_DYNCREATE( CustomizeBarDlg )

public:
	CustomizeBarDlg();			  	// Constructor 

	void Do(OpDescriptor*);			// "Do" function
	static BOOL Init();         	// Setup function
	BOOL Create();					// Create this dialog
	static OpState GetState(String_256*, OpDescriptor*);
									// Return the state of this operation
	virtual MsgResult Message(Msg* Message);
									// Message handler

// Public data members...
	static const INT32 IDD;
	static const CDlgMode Mode;		// ?
	
private:
	BOOL InitDialog(); 				// Standard setup function

	void	ShowOpsList();

};




#endif // INC_TOOLDLG
