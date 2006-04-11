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

// Code to modify the behaviour of controls on Camelot bars so that they respond
// to help messages and can be dragged about etc.


#include "camtypes.h"

#include "ctrlhelp.h"

#include "app.h"
#include "fixmem.h"
#include "errors.h"
#include "ensure.h"
#include "opdesc.h"
#include "camframe.h"
#include "bblwnd.h"
#include "ops.h"
#include "tool.h"
#include "dlgmgr.h"
//#include "fonts.h"
#include "keypress.h"
//#include "childbar.h"
#include "statline.h"

//#include "textres.h"	// required so we know what an _R(IDC_FONT_COMBO) is.
#include "textinfo.h"	// required so we know what a TextInfoBarOp is.

//#include "bitbutn.h"

#define STATE_TRACE 0


wxWindow* EMPTY_SLOT = NULL;
extern wxWindow* LastPointerInButton; // for new flat look - last button plinthed
								 // refers to the global variable defined in bitbutn.cpp

/********************************************************************************************

>	class BarTable : public SimpleCCObject

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/94
	Purpose:	Provide an array of window handles for all the existing Camelot bars.
				This enables us to get bubble help to behave correctly, i.e. when the 
				cursor is over the bar, the bubble help goes away, but the delay timer
				for bubble help is not restarted.
	SeeAlso:	ControlHelper

********************************************************************************************/

class BarTable : public SimpleCCObject
{
public:
	BarTable();
	~BarTable();
	BOOL Init();

	BOOL AddBar(wxWindow*);
	BOOL DeleteBar(wxWindow*);
	BOOL ChangeBar(wxWindow*, wxWindow*);

	BOOL IsABar(wxWindow*);

private:
	INT32 FindBarIndex(wxWindow*);

	enum
	{
		NotFound = -1,
		EmptySlot = 0,
		Granularity = 5,
		InitialSize = 10
	};

	wxWindow** Table;
	INT32 TableSize;
};

/********************************************************************************************

>	BarTable::BarTable()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Purpose:	Construct the bar table object.
	SeeAlso:	BarTable::Init

********************************************************************************************/

BarTable::BarTable()
{
	// No table data yet.
	Table = NULL;
	TableSize = 0;
}

/********************************************************************************************

>	BarTable::~BarTable()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Purpose:	Destroy a BarTable object - frees up all memory used by the table data.
	SeeAlso:	BarTable::Init

********************************************************************************************/

BarTable::~BarTable()
{
	// Free up the table data.
	CCFree((void*)Table);
}

/********************************************************************************************

>	BOOL BarTable::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Returns:	TRUE if successful;
				FALSE if not.
	Purpose:	Allocate and initialise the table of bar entries.
	Errors:		Out of memory.
	SeeAlso:	BarTable

********************************************************************************************/

BOOL BarTable::Init()
{
	// Try to get some space for our table.
	Table = (wxWindow**) CCMalloc(BarTable::InitialSize * sizeof(wxWindow*));
	if (Table == NULL)
		return FALSE;

	// Initialise the table
	for (INT32 i = 0; i < BarTable::InitialSize; i++)
	{
		// No control registered in this entry yet
		Table[i] = EMPTY_SLOT;
	}

	// Update the table size
	TableSize = BarTable::InitialSize;

	// Success!
	return TRUE;
}

/********************************************************************************************

>	BOOL BarTable::AddBar(wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		Window - the handle of the bar.
	Returns:	TRUE if the bar was added to the table successully;
				FALSE if not.
	Purpose:	Add a barhandle to the table, using the data passed in.  The table
				will expand to accomodate the new bar if necessary and if possible.
	Errors:		Out of memory, Bar already exists in the table (debug builds only).
	SeeAlso:	BarTable::DeleteBar; BarTable::FindBar

********************************************************************************************/

BOOL BarTable::AddBar(wxWindow* Window)
{
	// Basic sanity checks.
	ENSURE(Window != NULL, "NULL window handle in BarTable::AddBar");
	if (Window == NULL )
		return FALSE;
	ENSURE(FindBarIndex(Window) == BarTable::NotFound, "Bar already exists in BarTable::AddBar");

	INT32 i = 0;
	while ((i < TableSize) && (Table[i] != EMPTY_SLOT))
		i++;

	if (i >= TableSize)
	{
		// No free slots - extend the table.
		INT32 NewTableSize = TableSize + BarTable::Granularity;
		wxWindow* *NewTable = (wxWindow* *) CCRealloc((void*)Table, NewTableSize * sizeof(wxWindow*));
		if (NewTable == NULL)
			return FALSE;

		// Table extended ok - point 'i' at the first free slot, and update table variables.
		i = TableSize;
		Table = NewTable;
		TableSize = NewTableSize;

		for (INT32 j=i+1;j<TableSize;j++)
			Table[j] = EMPTY_SLOT;
	}

	// If we've got this far, 'i' points at a valid free slot in the table.
	Table[i] = Window;

	// Everything worked
	return TRUE;
}

/********************************************************************************************

>	BOOL BarTable::DeleteBar(wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		Window - the handle of the control to remove.
	Returns:	TRUE if the control was removed ok;
				FALSE if the control could not be found in the table.
	Purpose:	Remove the control from the table of subclassed controls.
	Errors:		Control's window handle was not found in the table (ENSUREs in debug builds).
	SeeAlso:	BarTable::AddControl; BarTable::FindControl; ControlHelper;
				ControlEntry

********************************************************************************************/

BOOL BarTable::DeleteBar(wxWindow* Window)
{
	// Basic sanity checks
	ENSURE(Window != 0, "NULL Window handle in BarTable::DeleteBar!");
	if (Window == 0)
		return FALSE;

	// Search for the specified bar.
	INT32 i = FindBarIndex(Window);

	ENSURE(i != BarTable::NotFound, "Control not found in BarTable::DeleteBar");

	if (i == BarTable::NotFound)
		// Unable to find any record of the bar
		return FALSE;

	// Found the bar - delete it
	Table[i] = EMPTY_SLOT;

	// Return success
	return TRUE;
}

/********************************************************************************************

>	BOOL BarTable::ChangeBar(wxWindow* Old, wxWindow* New)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/94
	Inputs:		Old - the old window handle of the bar.
				New - the new window handle of the bar.
	Returns:	TRUE if the bar was changed successfully;
				FALSE if not.
	Purpose:	Inform the table that a bar has been deleted and recreated with a new window
				handle.
	Errors:		The bar could not be found.
	SeeAlso:	BarTable::AddBar; BarTable::DeleteBar

********************************************************************************************/

BOOL BarTable::ChangeBar(wxWindow* Old, wxWindow* New)
{
	// Basic sanity checks
	ENSURE((Old != NULL) && (New != NULL), "NULL Window handle in BarTable::ChangeBar!");
	if ((Old == NULL) || (New == NULL))
		return FALSE;

	// Search for the specified bar.
	INT32 i = FindBarIndex(Old);

	ENSURE(i != BarTable::NotFound, "Control not found in BarTable::ChangeBar");

	if (i == BarTable::NotFound)
		// Unable to find any record of the bar
		return FALSE;

	// Found the bar - update the window handle.
	Table[i] = New;

	// Return success
	return TRUE;
}


/********************************************************************************************

>	BOOL BarTable::IsABar(wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/94
	Inputs:		Window - the window handle to check.
	Returns:	TRUE if this window handle is registered as a Camelot bar;
				FALSE if not.
	Purpose:	Find out if a given window handle is the handle of a Camelot bar, as
				recorded in this BarTable object.
	SeeAlso:	ControlHelper

********************************************************************************************/

BOOL BarTable::IsABar(wxWindow* Window)
{
	ENSURE(Window != NULL, "NULL Window in BarTable::IsABar");
	if (Window == NULL)
		return FALSE;

	return (FindBarIndex(Window) != BarTable::NotFound);
}

/********************************************************************************************

>	INT32 BarTable::FindBarIndex(wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		Window - the handle of the control to search for.
	Returns:	The index into the Table array of the control's record, or
				BarTable::NotFound if the control is not registered in the table.
	Purpose:	Given the window handle of a control, return the offset into the table
				array of its record.
	Errors:		Not found.
	SeeAlso:	BarTable::FindControl; BarTable::DeleteControl

********************************************************************************************/

INT32 BarTable::FindBarIndex(wxWindow* Window)
{
	// Scane the table for this entry
	INT32 i = 0;
	while ((i < TableSize) && (Table[i] != Window))
		i++;

	// Did we find it?
	if (Table[i] != Window)
		// No - inform caller of failure.
		return BarTable::NotFound;

	// Yes, success
	return i;
}



/*****************************************************************************
>	BOOL ControlEntry::ControlStatusLineText(String_256* ptext)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/94
	Returns:	TRUE if a valid string was written to the text buffer
	Purpose:	return the status-line text for a given ControlEntry object
*****************************************************************************/

BOOL ControlEntry::ControlStatusLineText(String_256* ptext)
{
	// if valid OpDesc, load status-line text from OpDescriptor
	if (pOpDesc != NULL)
		return pOpDesc->GetText(ptext,OP_DESC_TEXT);

	// If there's no status-line string associated with the control then make the
	// string blank.
	if (StatusID == 0)
	{
		TRACEUSER( "JustinF", _T("ControlEntry at 0x%lX has zero status-line string ID\n"),
								(UINT32) this);
		ptext->Empty();
		return TRUE;
	}

	// else just load using string ID provided directly by caller.
	return ptext->Load(StatusID,ModuleID);
}


/********************************************************************************************

>	ControlTable::ControlTable()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Purpose:	Construct the control table object.
	SeeAlso:	ControlTable::Init

********************************************************************************************/

ControlTable::ControlTable()
{
	// No table data yet.
	Table = NULL;
	TableSize = 0;

	// Initial state
	LastWindow = NULL;
	LastIndex = 0;
}

/********************************************************************************************

>	ControlTable::~ControlTable()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Purpose:	Destroy a ControlTable object - frees up all memory used by the table data.
	SeeAlso:	ControlTable::Init

********************************************************************************************/

ControlTable::~ControlTable()
{
	// Free up the table data.
	CCFree(Table);
}

/********************************************************************************************

>	BOOL ControlTable::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Returns:	TRUE if successful;
				FALSE if not.
	Purpose:	Allocate and initialise the table of control entries.
	Errors:		Out of memory.
	SeeAlso:	ControlTable

********************************************************************************************/

BOOL ControlTable::Init()
{
	// Try to get some space for our table.
	Table = (ControlEntry *) CCMalloc(ControlTable::InitialSize * sizeof(ControlEntry));
	if (Table == NULL)
		return FALSE;

	// Initialise the table
	for (INT32 i = 0; i < ControlTable::InitialSize; i++)
	{
		// No control registered in this entry yet
		Table[i].Window = EMPTY_SLOT;
	}

	// Update the table size
	TableSize = ControlTable::InitialSize;

	// Success!
	return TRUE;
}

/********************************************************************************************

>	BOOL ControlTable::AddControl(wxWindow* Window, ControlHelpInfo *pInfo, WNDPROC WndProc)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		Window - the handle of the control (this is used as a minimal key for the
						 table entries).
				pInfo - information about the control to add.
				WndProc - the WndProc that the control normally uses.
	Returns:	TRUE if the control was added to the table successully;
				FALSE if not.
	Purpose:	Add a control record to the table, using the data passed in.  The table
				will expand to accomodate the new control if necessary and if possible.
	Errors:		Out of memory, Control already exists in the table (debug builds only).
	SeeAlso:	ControlTable::DeleteControl; ControlTable::FindControl

********************************************************************************************/

PORTNOTE( "dialog", "Remove function that needs WNDPROC" )
#if !defined(EXCLUDE_FROM_XARALX)
BOOL ControlTable::AddControl(wxWindow* Window, ControlHelpInfo* pInfo, WNDPROC WndProc)
{
	// Basic sanity checks.
	ERROR3IF(pInfo == NULL, "Null ControlHelpInfo* in ControlTable::AddControl");
	ENSURE(Window != NULL, "NULL window handle in ControlTable::AddControl");
	ENSURE(WndProc != NULL, "NULL WndProc in ControlTable::AddControl");
//	ERROR3IF(pInfo->StatusID == 0, "Zero string ID in ControlTable::AddControl");
	
	if ((Window == NULL ) || (WndProc == NULL))
		return FALSE;

	// Start from the end of array and search backwards (it's more efficient for large
	// arrays).
	INT32 i = TableSize - 1;

	while ((i >= 0) && (Table[i].Window != EMPTY_SLOT))
		i--;

	if (i < 0)
	{
		// No free slots - extend the table.
		INT32 NewTableSize = TableSize + ControlTable::Granularity;
		ControlEntry *NewTable = 
			(ControlEntry *) CCRealloc(Table, NewTableSize * sizeof(ControlEntry));
		if (NewTable == NULL)
			return FALSE;

		// Table extended ok - point 'i' at the first free slot, and update table variables.
		i = TableSize;
		Table = NewTable;
		TableSize = NewTableSize;

		for (INT32 j=i+1;j<TableSize;j++)
			Table[j].Window = EMPTY_SLOT;
	}

	// If we've got this far, 'i' points at a valid free slot in the table.
	Table[i].Window   = Window;
	Table[i].WndProc  = WndProc;
	Table[i].Parent	  = pInfo->Parent;
	Table[i].pOpDesc  = pInfo->pOpDesc;
	Table[i].BubbleID = pInfo->BubbleID;
	Table[i].StatusID = pInfo->StatusID;
	Table[i].ModuleID = pInfo->ModuleID;  
	
	// If the Control being subclassed is a combo or an edit then we need to add
	// commit handling.

	String_256 ClassNameStr;  
	GetClassName(Window, (TCHAR*)ClassNameStr, 255);
	Table[i].AddCommitHandling = ( (ClassNameStr == String_8(TEXT("Edit"))) ||
		 						   (ClassNameStr == String_8(TEXT("ComboBox"))) ||
//								   (ClassNameStr == String_64(TEXT("cc_CustomEdit"))) ||									
								   (ClassNameStr == String_64(TEXT("cc_1dBitmapComboBoxEdit"))) ||
								   (ClassNameStr == String_64(TEXT("cc_2dBitmapComboBoxEdit"))) );

	// Set cache back to default
	LastWindow = NULL;
	LastIndex = 0;

	// Everything worked
	return TRUE;
}
#endif

/********************************************************************************************

>	WNDPROC ControlTable::DeleteControl(wxWindow* Window, wxWindow* *RealWindow)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		Window - the handle of the control to remove (including its child windows).
	Outputs:	The window handle of the entry just deleted, which for child windows will
				not be the same as the 'Window' input parameter. Pass in NULL if you don't
				want this window handle.
	Returns:	The original WNDPROC of the control that was deleted, or
				NULL if the control could not be found in the table.
	Purpose:	Remove the control or a child of it from the table of subclassed controls, 
				and return the original WndProc for that window.
				This function should be called repetitively for a window handle until NULL
				is returned, to ensure that all child window entries of the control are
				deleted too.
	Errors:		ENSUREs if NULL window handle passed in.
	SeeAlso:	ControlTable::AddControl; ControlTable::FindControl; ControlHelper;
				ControlEntry

********************************************************************************************/

WNDPROC ControlTable::DeleteControl(wxWindow* Window, wxWindow* *RealWindow)
{
	// Basic sanity checks
	ENSURE(Window != 0, "NULL Window handle in ControlTable::DeleteControl!");
	if (Window == 0)
		return FALSE;

	// Search for the specified control and its children.
	INT32 i = FindControlIndex(Window, FALSE);

	if (i == ControlTable::NotFound)
		// No more controls associated with this window ID...
		return NULL;

	// Found the control - delete it
	if (RealWindow != NULL)
		*RealWindow = Table[i].Window;
	Table[i].Window = EMPTY_SLOT;
	Table[i].Parent = NULL;

	// Return success
	return Table[i].WndProc;
}

BOOL ControlTable::ChangeControl(wxWindow* Window, ControlHelpInfo *pInfo)
{
	INT32 i = FindControlIndex(Window);

	if (i == ControlTable::NotFound)
		return FALSE;

	Table[i].BubbleID = pInfo->BubbleID;
	Table[i].StatusID = pInfo->StatusID;
	Table[i].ModuleID = pInfo->ModuleID;  

	// Everything worked
	return TRUE;
}

/********************************************************************************************

>	ControlEntry *ControlTable::FindControl(wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		Window - the handle of the control to look for.
	Returns:	Pointer to the data currently registered for the control in question,
				or NULL if the control could not be found.
	Purpose:	Given a control's window handle, return the information currently registered
				for that control.  Do not free the data pointed to by the return value.
	SeeAlso:	ControlEntry

********************************************************************************************/

ControlEntry *ControlTable::FindControl(wxWindow* Window)
{
	INT32 i=FindControlIndex(Window);

	if (i==ControlTable::NotFound)
		return NULL;

	return Table + i;
}


/********************************************************************************************

>	INT32 ControlTable::FindControlIndex(wxWindow* Window, BOOL IgnoreChildren = TRUE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		Window         - the handle of the control to search for.
				IgnoreChildren - if TRUE, then only return the entry for the control
								 itself; if FALSE then return entries for the control or
								 any children of it.
	Returns:	The index into the Table array of the control's record, or
				ControlTable::NotFound if the control is not registered in the table.
	Purpose:	Given the window handle of a control, return the offset into the table
				array of its record, or optionally of the control's child windows.
	Errors:		Not found.
	SeeAlso:	ControlTable::FindControl; ControlTable::DeleteControl

********************************************************************************************/

INT32 ControlTable::FindControlIndex(wxWindow* Window, BOOL IgnoreChildren)
{
	// Scan the table for this entry...
	INT32 i;

	// Is this the same as the last window handle we looked for?
	// If so, start from the last slot we looked at.
	if (LastWindow == Window)
		i = LastIndex;
	else 
		i = 0;

	while ((i < TableSize) && (Table[i].Window != Window) &&
		   (IgnoreChildren || (Table[i].Parent != Window)))
		i++;

	// Did we find it?
	if (i >= TableSize)
		// No - inform caller of failure.
		return ControlTable::NotFound;

	// Yes, success
	LastIndex = i;
	LastWindow = Window;

	return i;
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//						ControlHelper Member Functions + Data
//
/////////////////////////////////////////////////////////////////////////////////////////////

// Pointer to the table of sublassed controls we're managing at present.
ControlTable *ControlHelper::Controls = NULL;

// Pointer to the list of bars we currently know about.
BarTable *ControlHelper::Bars = NULL;

// Window handle of the last control that the cursor was over.
wxWindow* ControlHelper::LastControl = NULL;

// The last position of the cursor (used to defer bubble help updates).
POINT ControlHelper::LastPos = {-1, -1};

// Pointer to the bubble help window.
BubbleHelpWnd *ControlHelper::BubbleWnd = NULL;

// Timer and flag to control small delay between consecutive bubble help display.
MonotonicTime ControlHelper::PendingTimer;

// State of the bubble help system
ControlHelper::BubbleState ControlHelper::BubbleHelpState = STATE_DEAD;

// TRUE if one of our controls has focus, in which case bubble help should not appear.
BOOL ControlHelper::ControlHasFocus = FALSE;

// Variables for ad-hoc bubble help
wxWindow* ControlHelper::AdHocWindow = NULL;
UINT32 ControlHelper::AdHocControl = 0;
BOOL ControlHelper::AdHocControlIsDifferent = TRUE;
void *ControlHelper::AdHocReference = NULL;
ControlHelper::BubbleHelpCallback ControlHelper::AdHocCallback = NULL;


// Number of non-kernel modal dialogs open.
INT32 ControlHelper::ModalDialogs = 0;


/********************************************************************************************

>	BOOL ControlHelper::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Returns:	TRUE if initialised ok;
				FALSE if not.
	Purpose:	Called to initialise the control helper system.

********************************************************************************************/

BOOL ControlHelper::Init()
{
	// Allocate and initialise a BarTable object.
	Bars = new BarTable;
	if ((Bars == NULL) || !Bars->Init())
		return FALSE;

	// Allocate and intialise ourselves a ControlTable object
	Controls = new ControlTable;
	if ((Controls == NULL) || !Controls->Init())
		return FALSE;

	// Initialise the bubble help window system
	return BubbleHelpWnd::Init();
}

/********************************************************************************************

>	void ControlHelper::DeInit()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Purpose:	Deinitialise the control helper system.

********************************************************************************************/

void ControlHelper::DeInit()
{
	// Deallocate our control table and bar table objects.
	delete Bars;
	Bars = NULL;
	delete Controls;
	Controls = NULL;
	BubbleHelpWnd::DeInit();
}

/********************************************************************************************

>	BOOL ControlHelper::NotifyBarCreated(wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/94
	Inputs:		Window - the handle of the bar that has just been created.
	Returns:	TRUE if the bar was added to ControlHelper's records successfully;
				FALSE if not.
	Purpose:	Inform the ControlHelper class that a bar has been created with the
				specified window handle.
	Errors:		Out of memory.
	SeeAlso:	ControlHelper::NotifyBarDeleted; ControlHelper::NotifyBarChanged

********************************************************************************************/

BOOL ControlHelper::NotifyBarCreated(wxWindow* Window)
{
	// Sanity checks
	ENSURE(Bars != NULL, "ControlHelper has not been initialised successfully");
	ENSURE(Window != NULL, "NULL Window handle in ControlHelper::NotifyBarCreated");

	// Just add the bar to our list.
	return Bars->AddBar(Window);
}

/********************************************************************************************

>	BOOL ControlHelper::NotifyBarDeleted(wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/94
	Inputs:		Window - handle of the bar that has just been deleted.
	Returns:	TRUE if the bar handle was found;
				FALSE if not.
	Purpose:	Notify the ControlHelper class that a bar has been deleted.
	SeeAlso:	ControlHelper::NotifyBarChanged; ControlHelper::NotifyBarCreated

********************************************************************************************/

BOOL ControlHelper::NotifyBarDeleted(wxWindow* Window)
{
	// Sanity checks
	ENSURE(Bars != NULL, "ControlHelper has not been initialised successfully");
	ENSURE(Window != NULL, "NULL Window handle in ControlHelper::NotifyBarDeleted");

	// Just remove the bar from our list.
	return Bars->DeleteBar(Window);
}

/********************************************************************************************

>	BOOL ControlHelper::NotifyBarChanged(wxWindow* Old, wxWindow* New)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/94
	Inputs:		Old - the old window handle of the bar.
				New - the new window handle of the bar.
	Returns:	TRUE if the bar's window handle was updated successfully;
				FALSE if not.
	Purpose:	Notify the ControlHelper class that a bar's window handle has been changed.
	Errors:		Bar not found.
	SeeAlso:	ControlHelper::NotifyBarCreated; ControlHelper::NotifyBarDeleted

********************************************************************************************/

BOOL ControlHelper::NotifyBarChanged(wxWindow* Old, wxWindow* New)
{
	// Sanity checks
	ENSURE(Bars != NULL, "ControlHelper has not been initialised successfully");
	ENSURE((Old != NULL) && (New != NULL), 
		   "NULL Window handle in ControlHelper::NotifyBarChanged");

	// Just update the bar in our list.
	return Bars->ChangeBar(Old, New);
}


/********************************************************************************************

>	BOOL ControlHelper::AddControl(wxWindow* Window, ControlHelpInfo *pInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/94
	Inputs:		Window - The window to add an entries for.
				pInfo - information about the control being added.
	Returns:	TRUE if the window and its descendants were processed ok;
				FALSE if not.
	Purpose:	Add entries for a window and all its descendant windows to our list of
				subclassed controls.
				This is a 'helper' function for ControlHelper::NotifyControlCreated.
	SeeAlso:	ControlTable::AddControl; ControlHelper::NotifyControlCreated

********************************************************************************************/

BOOL ControlHelper::AddControl(wxWindow* Window, ControlHelpInfo *pInfo)
{
	// Firstly, try to add this control
	WNDPROC WndProc = (WNDPROC) ::GetWindowLong(Window, GWL_WNDPROC);
	ENSURE(WndProc != 0, "NULL WndProc in ControlHelper::AddControl");

	if (!(Controls->AddControl(Window, pInfo, WndProc)))
		goto Failure;

	// Subclass the control.
	::SetWindowLong(Window, GWL_WNDPROC, (INT32) MyWndProc);

	// Now try to add/subclass all the children...
	wxWindow* Child;
	Child = ::GetWindow(Window, GW_CHILD);

	while (Child != NULL)
	{
		// Recurse for this child window
		if (!(AddControl(Child, pInfo)))
			goto Failure;

		// Get the next child window, if any.
		Child = ::GetWindow(Child, GW_wxWindow*NEXT);
	}

	// Success!
	return TRUE;

Failure:
	// Could not add all the controls for some reason - remove any we might have added
	// so far, but only if this call is for the parent control (cos we recurse in this
	// function, and we only want to do this once).
	if (Window == pInfo->Parent)
	{
		wxWindow* RealWindow;

		while ((WndProc = Controls->DeleteControl(Window, &RealWindow)) != NULL)
		{
			// De-subclass the control.
			WNDPROC TheWndProc = (WNDPROC) ::GetWindowLong(Window, GWL_WNDPROC);
			if (TheWndProc = MyWndProc)
				// It really is one we've subclassed, so 'de-subclass' it!
				::SetWindowLong(Window, GWL_WNDPROC, (INT32) WndProc);
		}
	}

	// Failed
	return FALSE;
}

/********************************************************************************************

>	BOOL ControlHelper::NotifyControlCreated(wxWindow* Window, ControlHelpInfo *pInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		Window - the handle of the control that has been created.
				pInfo - information about the control neing created
	Returns:	TRUE if the control was added to the ControlHelper's table ok;
				FALSE if not.
	Purpose:	Inform the Control helper system that a control has just been created.
	Errors:		Out of memory; NULL window handle
	SeeAlso:	ControlHelper::NotifyControlDeleted; ControlHelper::NotifyControlChanged;
				ControlHelper::AddControl; ControInfo

********************************************************************************************/

BOOL ControlHelper::NotifyControlCreated(wxWindow* Window, ControlHelpInfo *pInfo)
{
	// Sanity checks
	ENSURE(Controls != NULL, "ControlHelper has not been initialised successfully");
	ENSURE(Window != 0, "NULL Window handle in ControlHelper::NotifyControlCreated");
	ENSURE(pInfo != 0, "NULL ControlHelpInfo pointer in ControlHelper::NotifyControlCreated");
	if ((Controls == NULL) || (Window == 0) || (pInfo == NULL))
		return FALSE;

	// Fill in the parent field
	pInfo->Parent = Window;

	// Add the control to our list of sublassed controls.
	if (!AddControl(Window, pInfo))
		// Give up!
		return FALSE;

	// Success!
	return TRUE;
}

BOOL ControlHelper::NotifyControlCreated(wxWindow* Window, OpDescriptor *pOpDesc)
{
	ControlHelpInfo Info;
	Info.pOpDesc = pOpDesc;
	Info.BubbleID = 0;
	Info.StatusID = 0;
	Info.ModuleID = 0;

	return NotifyControlCreated(Window, &Info);
}

/********************************************************************************************

>	BOOL ControlHelper::NotifyControlChanged(wxWindow* Window, ControlHelpInfo* pInfo)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/04/95
	Inputs:		Window - the handle of the control that is to be changed.
				pInfo - information about the control
	Returns:	TRUE if the control was changed in the table ok;
				FALSE if not.
	Purpose:	Allows the Bubble and Status ID's of a control to be changed.
	Errors:		Out of memory; NULL window handle
	SeeAlso:	ControlHelper::NotifyControlDeleted; ControlHelper::NotifyControlChanged;
				ControlHelper::AddControl; ControInfo

********************************************************************************************/

BOOL ControlHelper::NotifyControlChanged(wxWindow* Window, ControlHelpInfo* pInfo)
{
	if (Controls == NULL)
		return FALSE;
	
	if (!Controls->ChangeControl(Window, pInfo))
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL ControlHelper::NotifyControlDeleted(wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		Window - handle of the window that has been deleted.
	Returns:	TRUE if the control was deleted from the ControlHelper's table ok;
				FALSE if not.
	Purpose:	Notify the control helper system that a control has just been deleted.
	Errors:		ControlHelper has not been initialised; Window handle is 0.
	SeeAlso:	ControlHelper::NotifyControlCreated; ControlHelper::NotifyControlChanged

********************************************************************************************/

BOOL ControlHelper::NotifyControlDeleted(wxWindow* Window)
{
	ENSURE(Controls != NULL, "ControlHelper has not been initialised successfully");
	ENSURE(Window != NULL, "NULL Window handle in ControlHelper::NotifyControlDeleted");
	if ((Controls == NULL) || (Window == NULL))
		return FALSE;

	// If deleting the currently active bubble help control, then kill bubble help.
	if ((Window == LastControl) && (BubbleHelpState != STATE_DEAD))
		SetState(STATE_DEAD);

	wxWindow* RealWindow;
	WNDPROC OldWndProc;

	while ((OldWndProc = Controls->DeleteControl(Window, &RealWindow)) != NULL)
	{
		// De-subclass the control.
		WNDPROC CurrentWndProc = (WNDPROC) ::GetWindowLong(RealWindow, GWL_WNDPROC);

		if (CurrentWndProc == MyWndProc)
			// It really is one we've subclassed, so 'de-subclass' it!
			::SetWindowLong(RealWindow, GWL_WNDPROC, (INT32) OldWndProc);
	}

	// If we got this far it's successful
	return TRUE;
}


/*****************************************************************************
>	static BOOL ControlHelper::GetStatusLineText(String_256* ptext, wxWindow* window)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/94
	Returns:	TRUE if returning valid text
	Purpose:	Get status-line text for a control with given WHND
*****************************************************************************/

BOOL ControlHelper::GetStatusLineText(String_256* ptext, wxWindow* window)
{
	if (window==NULL)
		return FALSE;

	ControlEntry* pEntry=Controls->FindControl(window);
	if (pEntry==NULL)
		return FALSE;
	String_256 strCaption;
	GetWindowText(window, strCaption, 255);
	if (strCaption == String_256("Download"))
	{
		*ptext = "Download selected resource to the local hard disk";
		return TRUE;
	}
	else return pEntry->ControlStatusLineText(ptext);
}


/********************************************************************************************

>	LRESULT ControlHelper::MyWndProc(wxWindow* Window, UINT32 Msg, WPARAM wParam, LPARAM lParam)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		The usual - see SDK documentation for "WindowProc".
	Returns:	As for SDK WindowProc.
	Purpose:	Callback function for special handling of controls on Camelot bars.
				At present it does nothing - just chains to the controls usual WndProc.
	Errors:		
	SeeAlso:	ControlHelper

********************************************************************************************/

LRESULT ControlHelper::MyWndProc(wxWindow* Window, UINT32 Msg, WPARAM wParam, LPARAM lParam)
{
	ENSURE(Window != 0, "NULL Window handle in ControlHelper::MyWndProc");

	// Find the WindowProc for this control.

	ControlEntry *pEntry = Controls->FindControl(Window);

	ENSURE(pEntry != NULL, "Could not find subclassed control in ControlHelper::MyWndProc");

	if (pEntry != NULL)
	{
		switch (Msg)
		{
			case WM_MOUSEMOVE:
			{
		
				// start control drag
				if(KeyPress::IsAlternativePressed())
				{
					return TRUE;
				}
				// Update the status bar on MouseMove messages
				String_256 Desc;
				if (pEntry->ControlStatusLineText(&Desc))
				{
					StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
					if (pStatusLine->AllowControlHelp ())
					{
						if (pStatusLine)
							pStatusLine->UpdateText(&Desc,STATUSLINE_SELDESC_BUTTONS);
						else
							ERROR3("ControlHelper::MyWndProc() - GetpStatusLine() failed");
					}
				}
			}
			break;
  
			case WM_SETFOCUS:
				// This control has focus - disable the bubble help.
				//ControlHasFocus = TRUE;
				if (pEntry->AddCommitHandling)
				{
					SendMessage(GetParent(GetParent(Window)),WM_CTL_SETFOCUS,TRUE,(LPARAM)Window);	
					SendMessage(GetParent(Window),WM_CTL_SETFOCUS,TRUE,(LPARAM)Window);
					
					// Send the control an EM_SETSEL message so the text within it is highlighted.
					::SendMessage(Window, EM_SETSEL, 0, (LPARAM) (INT32) -1);
				}

				// Fall through...
			
			case WM_LBUTTONDOWN:
				if(KeyPress::IsAlternativePressed())
				{
					String_256 ClassNameStr;  // The control type
					GetClassName(GetParent(Window), (TCHAR*)ClassNameStr, 255);
					if (ClassNameStr == String_8(TEXT("ComboBox")))
					{
						SendMessage(GetParent(GetParent(Window)),WM_START_CTLDRAG,
										KeyPress::IsConstrainPressed(),(LPARAM)GetParent(Window));	
				 	}
					else
					{
				 		SendMessage(GetParent(Window),WM_START_CTLDRAG,
				 						KeyPress::IsConstrainPressed(),(LPARAM)Window);
					}
					return TRUE;
				}
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
		
				// start control drag
			
				// Mouse activity - disable bubble help.
				BubbleHelpDisable();
				break;

			case WM_KILLFOCUS:
				// This control has lost focus - allow the bubble help to start working again.
				ControlHasFocus = FALSE;
				if (pEntry->AddCommitHandling)
				{
					SendMessage(GetParent(GetParent(Window)),WM_CTL_SETFOCUS,FALSE,(LPARAM)Window);	
					SendMessage(GetParent(Window),WM_CTL_SETFOCUS,FALSE,(LPARAM)Window);	
				}
				break;
			default:

				if (pEntry->AddCommitHandling)
				{
					// Is commit processing required, this should be the case if the control 
					// is an edit or ComboBox 

					// Determine the control type
					switch (Msg)
					{
						case WM_CHAR:
							// Process this message to avoid message beeps.
							if ((wParam == CAMKEY(RETURN)) || (wParam == CAMKEY(TAB)))
							{
								ControlHasFocus = FALSE;
								return FALSE;
							}
							break;

						case WM_GETDLGCODE:
							return DLGC_WANTALLKEYS;

						case WM_KEYDOWN:
							// escape should return the focus back to the main frame
							if (wParam == CAMKEY(ESCAPE))
							{
								ControlHasFocus = FALSE;
								DialogManager::DefaultKeyboardFocus();
								return FALSE;
							}
							else if (wParam == CAMKEY(RETURN) || wParam == CAMKEY(TAB))
							{
								wxWindow* BarWnd;
								wxWindow* CtlWnd; 
					
								String_256 ClassNameStr;  // The control type

								BOOL camelotCustomComboBox = FALSE;

								//////////////////////////////////////
								
								// need to do things slightly differently if the control is a 
								// cc_1dBitmapComboBoxEdit or cc_2dBitmapComboBoxEdit

								GetClassName (Window, (TCHAR*)ClassNameStr, 255);

								if ((ClassNameStr == String_64(TEXT("cc_1dBitmapComboBoxEdit"))) ||
								    (ClassNameStr == String_64(TEXT("cc_2dBitmapComboBoxEdit"))))
								{
									camelotCustomComboBox = TRUE;
								}

								/////////////////////////////////////

								if (camelotCustomComboBox == FALSE)
								{
									GetClassName(GetParent(Window), (TCHAR*)ClassNameStr, 255);

									if (ClassNameStr == String_8(TEXT("ComboBox")) || ClassNameStr == String_16(TEXT("cc_CustomEdit")))
									{
										// edit controls of these classes are children of the controls.
										BarWnd = GetParent(GetParent(Window));	// Because combo edit control
																			// is a child of the combo 
																			// control itself
										CtlWnd = GetParent(Window);
									}
									else
									{
										// It's an Edit control
										BarWnd = GetParent(Window);	 // Bar is the edit's parent
										CtlWnd = Window;
									}
								}
								else
								{
									BarWnd = GetParent (Window);
									CtlWnd = Window;

									// the following sendmessage is actually so that when CAMKEY(RETURN) is hit,
									// (which actually goes through the WM_KEYDOWN in ctrlhelp.cpp); that we can actually
									// set the selected item BEFORE dialog manager continues !

									if (wParam == CAMKEY(RETURN))
									{
										SendMessage (CtlWnd, CB_SETTOPINDEX, 1, 0);
									}
									else
									{
										SendMessage (CtlWnd, CB_SETTOPINDEX, 0, 0);
									}
								}

								if (wParam == CAMKEY(RETURN) )
								{
									ControlHasFocus = FALSE;
									SendMessage(BarWnd, WM_CTL_SETFOCUS, COMMIT, (LPARAM) Window);	
								}												

								// Send commit message to parent, passing the key code (TAB or ENTER) which
								// it might be useful to know.
								SendMessage(BarWnd, WM_CTL_COMMIT, GetWindowID(CtlWnd), (LPARAM) wParam);
								
								// To fix various bugs this code is a little bodged.  The problems
								// that this code tries to address are:
								//	1)	Clicking in one of the size edit controls in the selector info bar,
								//		entering some text, pressing enter and then clicking in the same edit
								//		control a second time (this was refusing to take the focus a second
								//		time)
								//	2)	Typing a font name in the font drop down box box and then pressing
								//		enter (this was returning to the font the user had selected before
								//		they started typeing)
								if (::GetDlgCtrlID(Window) != _R(IDC_FONT_COMBO))
								{
									if (wParam == CAMKEY(RETURN))
									{
										GetMainFrame()->SetFocus();
									}
									else
									{
										// Move focus to the next (previous) control, just like Windows.
										wxWindow* wxWindow*NewFocus = ::GetNextDlgTabItem(BarWnd, Window, ::GetKeyState(CAMKEY(SHIFT)) < 0);
										if (wxWindow*NewFocus != NULL) ::SetFocus(wxWindow*NewFocus);
									}
									return FALSE;
								}
							}
							// added by Karim 30/9/99, to allow a more intelligent font-combo
							else if (wParam >= 'A' && wParam <= 'Z' && ::GetDlgCtrlID(Window) == _R(IDC_FONT_COMBO))
							{
								// pass the message on to the combo-box's parent, to be
								// processed in DialogManager::SendDialogMessage.
								::SendMessage(GetParent(Window), WM_KEYDOWN, wParam, lParam);
							}
							break;
					}

				}
		}
	}

	// For any other messages, kill bubble help before passing them on.
//	if (BubbleHelpActive)
//		BubbleHelpKill();

	// Default to chaining to the normal WndProc
	return ::CallWindowProc(pEntry->WndProc, Window, Msg, wParam, lParam);
}


/********************************************************************************************

>	void ControlHelper::ServiceBubbleHelp()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Purpose:	Updates the bubble help for our controls.  Should be called on idle or
				timer events.

********************************************************************************************/

void ControlHelper::ServiceBubbleHelp()
{
	// Find out which window the cursor is over.
	POINT Pos;
	::GetCursorPos(&Pos);
	wxWindow* ThisControl = ::WindowFromPoint(Pos);

	if ((ThisControl != NULL) /*&& (IsWindowVisible (ThisControl))*/)
	{
		// This code copes with disabled controls...

		// To look at the child windows of wxWindow*, screen coordinates
		// need to be converted to client coordinates.
		POINT ClientPos = Pos;
		ScreenToClient(ThisControl, &ClientPos);

		// Search through all child windows at this point. This
		// will continue until no child windows remain.
		while (TRUE)
		{
			wxWindow* Child = ChildWindowFromPoint(ThisControl, ClientPos);

			if ((Child != NULL) && (Child != ThisControl))
				ThisControl = Child;
			else
				break;
		}
	}


	WNDPROC WndProc;

	if (ThisControl == AdHocWindow)
		// Special ad-hoc bubble help - bodge the WndProc to fool the state machine.
		WndProc = ControlHelper::MyWndProc;
	else
	{
		// *** BODGE FIX by JustinF.
		// For some reason GetWindowLong GP-faults , so check the params to the call below.
		// IsWindow clears the error state so get it now just in case...
		DWORD LastError = ::GetLastError();
		if (!::IsWindow(ThisControl))
		{
			TRACE( _T("BubbleHelp wxWindow* bug - ThisControl is invalid (0x%p), GetLastError is 0x%lX\n"),
					(LPVOID) ThisControl, LastError);
			return;
		}
		
		// Find out the WndProc for this window.
		WndProc = (WNDPROC) ::GetWindowLong(ThisControl, GWL_WNDPROC);

		// If it's one of ours, find out if this is a child window of a control.
		// Only bother if this is different to the last control we looked at (because
		// we will have done this same check for the last control, so it must be a proper 
		// parent control if it's the same as the last control).
		if ((ThisControl != LastControl) && (WndProc == MyWndProc))
		{
			ControlEntry *pEntry = Controls->FindControl(ThisControl);
			ENSURE(pEntry != NULL, "Catastrophic failure in bubble help");

			// Just use the main parent handle of this control (may actually be the
			// same as the value ThisControl currently holds anyway).
			ThisControl = pEntry->Parent;
		}
	}

	//
	// Enter state machine event processor...
	//
	BubbleHelpStateMachine(Pos, ThisControl, WndProc);
}

/********************************************************************************************

>	void ControlHelper::BubbleHelpStateMachine(POINT Pos, wxWindow* ThisControl, WNDPROC WndProc)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Inputs:		Pos - the current cursor position; used to detect how long the mouse has
					  been stationary for. Currently this is not used as Charles doesn't
					  like it if bubble doesn't come up if you continuously jaggle the
					  pointer about.
				ThisControl - window handle of the control that the pointer is over.
				WndProc - WindowProc of the control specified by ThisControl (this enables
						  this routine to work out if this control has been subclassed by
						  the ControlHelper system).
	Purpose:	Service the bubble help state machine.  Given the input parameters it
				works out whether it needs to change state, and which state to change to.
				State changing also depends on timers elapsing and so on.
	SeeAlso:	-

********************************************************************************************/

void ControlHelper::BubbleHelpStateMachine(POINT Pos, wxWindow* ThisControl, WNDPROC WndProc)
{
	// can sometimes be called with a NULL handle if pointer has found its way offscreen
	if(ThisControl== NULL)
		return;

	switch (BubbleHelpState)
	{
		case STATE_DEAD:
			if (WndProc == ControlHelper::MyWndProc)
			{
				// It's one of our controls - set update pending
				SetState(STATE_INITIAL_PENDING);
			}
			else if (Bars->IsABar(ThisControl))
			{
				// It's a Camelot bar - set to initial disabled.
				SetState(STATE_INITIAL_DISABLED);
			}
			else
			{
				// this means that the cursor has left all the bars alone
				// so now is a good time to remove that button plinth if we were
				// using the flat style buttons with active plinth/bevelling (sjk 24/2/00)
				if (LastPointerInButton)
				{
					INT32 value = GetWindowLong(LastPointerInButton, GWL_STYLE);
					if (value & BS_POINTEROVER)
					{
						// extra tests on position since the export dlg appears to use the
						// help system slightly differently!!!
						RECT r;
						GetWindowRect(LastPointerInButton, &r);
						POINT p;
						GetCursorPos(&p);

						if (p.x < r.left || p.x > r.right || p.y < r.top || p.y > r.bottom)
						{
							SetWindowLong(LastPointerInButton, GWL_STYLE, value & ~BS_POINTEROVER);
							::InvalidateRect(LastPointerInButton, NULL, FALSE);
							::UpdateWindow(LastPointerInButton);
							LastPointerInButton = NULL;
						}
					}
				}
			}
			break;

		case STATE_INITIAL_PENDING:
			if (WndProc == ControlHelper::MyWndProc)
			{
				// User is over our control - if delay has passed and none of our controls
				// have focus, set to active
				if (PendingTimer.Elapsed(800) && !ControlHasFocus)
				{
					SetState(STATE_ACTIVE, ThisControl);
				}
			}
			else if (Bars->IsABar(ThisControl))
			{
				// User is on a control bar - disable help.
				SetState(STATE_INITIAL_DISABLED);
			}
			else
			{
				// User is not on a control or a bar - kill bubble help.
				SetState(STATE_DEAD);
			}
			break;

		case STATE_INITIAL_DISABLED:
			if (WndProc == ControlHelper::MyWndProc)
			{
				// User is over our control - is it different to the last control
				// we looked at?
				if (ThisControl != LastControl)
				{
					// Yes - start the delay timer for displaying bubble help
					SetState(STATE_INITIAL_PENDING);
				}
				else
				{
					// No - bubble help must be disabled for this control so do nothing.
				}
			}
			else if (!Bars->IsABar(ThisControl))
			{
				// User is not on a control or a bar - kill bubble help.
				SetState(STATE_DEAD);
			}
			break;

		case STATE_UPDATE_PENDING:
			if (WndProc == ControlHelper::MyWndProc)
			{
				// User is over our control - if delay has passed, set to active (but only 
				// if none of our controls have focus).
				if (PendingTimer.Elapsed(100) && !ControlHasFocus)
				{
					SetState(STATE_ACTIVE, ThisControl);
				}
			}
			else if (Bars->IsABar(ThisControl))
			{
				// User is on a control bar - disable help.
				SetState(STATE_DISABLED);
			}
			else
			{
				// User is not on a control or a bar - kill bubble help.
				SetState(STATE_DEAD);
			}
			break;

		case STATE_ACTIVE:
			if (BubbleWnd->GetSafewxWindow*() == ThisControl)
			{
				// We're over our own bubble window - disable it for a bit.
				SetState(STATE_UPDATE_PENDING);

				// We don't want to update LastControl or LastPos.
				return;
			}
			else if (WndProc == ControlHelper::MyWndProc)
			{
				// User is over our control - if a different one, set delayed update,
				// otherwise no change.
				// NB. if ad-hoc is active, we check different variables
				if (ThisControl == AdHocWindow)
				{
					// Ad-hoc window - check the flag set by DoBubbleHelpOn().
					if (AdHocControlIsDifferent)
						SetState(STATE_UPDATE_PENDING);
				}
				else if (LastControl != ThisControl)
				{
					SetState(STATE_UPDATE_PENDING);
				}
			}
			else if (Bars->IsABar(ThisControl))
			{
				// User is on a control bar - disable help.
				SetState(STATE_DISABLED);
			}
			else
			{
				// User is not on a control or a bar - kill bubble help.
				SetState(STATE_DEAD);
			}
			break;

		case STATE_DISABLED:
			if (WndProc == ControlHelper::MyWndProc)
			{
				// User is over our control - is it different to the last control
				// we looked at?
				// NB. if ad-hoc is active, we check different variables
				if (ThisControl == AdHocWindow)
				{
					// Ad-hoc window - check the flag set by DoBubbleHelpOn().
					if (AdHocControlIsDifferent)
						SetState(STATE_UPDATE_PENDING);
				}
				else if (LastControl != ThisControl)
				{
					// Yes - start the delay timer for displaying bubble help
					SetState(STATE_UPDATE_PENDING);
				}
				else
				{
					// No - bubble help must be disabled for this control so do nothing.
				}
			}
			else if (!Bars->IsABar(ThisControl))
			{
				// User is not on a control or a bar - kill bubble help.
				SetState(STATE_DEAD);
			}
			break;

		default:
			ENSURE(FALSE, "Bad current state in bubble help");
			break;
	}

	// Update the cursor position for pending updates, and the last control position
	// for display updates.
	LastPos = Pos;
	LastControl = ThisControl;
}



/********************************************************************************************

>	void ControlHelper::SetState(BubbleState NewState, wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Inputs:		NewState - the state to change the state machine to.
				Window - window handle of the control the pointer is currently over (only
						 used when setting the state to STATE_ACTIVE, when it is used to
						 get the bubble help text).
	Purpose:	Change the state of the bubble help state machine to the specified state.
				Calls the appropriate state change handler to process the event.
	SeeAlso:	ControlHelper::DeadHandler; ControlHelper::ActiveHandler; 
				ControlHelper::InitialPendingHandler; ControlHelper::InitialDisabledHandler;
				ControlHelper::DisabledHandler; ControlHelper::UpdatePendingHandler

********************************************************************************************/

void ControlHelper::SetState(BubbleState NewState, wxWindow* Window)
{
	// Find out which state to change to.
	switch (NewState)
	{
		case STATE_DEAD:				DeadHandler();				break;
		case STATE_ACTIVE:				ActiveHandler(Window);		break;
		case STATE_INITIAL_PENDING:		InitialPendingHandler();	break;
		case STATE_INITIAL_DISABLED:	InitialDisabledHandler();	break;
		case STATE_UPDATE_PENDING:		UpdatePendingHandler();		break;
		case STATE_DISABLED:			DisabledHandler();			break;
	}
}


/********************************************************************************************

>	void ControlHelper::DeadHandler()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Purpose:	Kill off the bubble help by setting the bubble help state machine to the
				STATE_DEAD state.
	SeeAlso:	ControlHelper::ActiveHandler; ControlHelper::InitialPendingHandler; 
				ControlHelper::InitialDisabledHandler; ControlHelper::DisabledHandler;
				ControlHelper::UpdatePendingHandler

********************************************************************************************/

void ControlHelper::DeadHandler()
{
#if STATE_TRACE
	if (IsUserName("Tim"))
		TRACE( _T("STATE_DEAD\n"));
#endif

	switch (BubbleHelpState)
	{
		case STATE_ACTIVE:
			// Kill the bubble help window and fall through.
			ENSURE(BubbleWnd != NULL, "No bubble help window!");
			BubbleHelpKill();

		case STATE_INITIAL_PENDING:
		case STATE_INITIAL_DISABLED:
		case STATE_DISABLED:
		case STATE_UPDATE_PENDING:
			BubbleHelpState = STATE_DEAD;
			break;

		default:
			ENSURE(FALSE, "Wrong state in bubble help");
			break;
	}
}

/********************************************************************************************

>	void ControlHelper::ActiveHandler(wxWindow* Window)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Inputs:		Window - handle of the control the cursor is currently over.
	Purpose:	Activate the bubble help system by moving to the STATE_ACTIVE state.
				This causes a bubble help window to appear under the cursor, but not is
				there are any keys down, or if there is a drag going on, or if our app
				is not active.  At some time in the future we should also refuse to put
				up bubble help if the user has the menu up.
	SeeAlso:	ControlHelper::DeadHandler; ControlHelper::InitialPendingHandler; 
				ControlHelper::InitialDisabledHandler; ControlHelper::DisabledHandler;
				ControlHelper::UpdatePendingHandler

********************************************************************************************/

void ControlHelper::ActiveHandler(wxWindow* Window)
{
	// Sanity checks
	ENSURE(Window != NULL, "Bad handle in control helper ActiveHandler()");
	ENSURE(BubbleWnd == NULL, "Bubble window already exists in BubbleHelpStart()");

	// No bubble help during drags, or if any mouse buttons are down.

	// Get mouse keys - top bit is set if the button is down.
	INT8 MouseStatus = (::GetKeyState(CAMKEY(LBUTTON)) | 
						 ::GetKeyState(CAMKEY(MBUTTON)) | 
						 ::GetKeyState(CAMKEY(RBUTTON)));

	if ((MouseStatus & 0x8000) ||
	    (Operation::GetCurrentDragOp() != NULL))
	{
		// Revert to pending
		BubbleHelpState = STATE_ACTIVE; // Very important else state machine gets upset!
		SetState(STATE_INITIAL_PENDING);
		return;
	}

	// Don't do bubble help if we're iconised.
	CWnd *pMainWnd = AfxGetApp()->m_pMainWnd;
	if (pMainWnd->IsIconic())
	{
		// Revert to pending
		BubbleHelpState = STATE_ACTIVE; // Very important else state machine gets upset!
		SetState(STATE_INITIAL_PENDING);
		return;
	}

	// Only do bubble help if our application is active.
	wxWindow* Active  = ::GetActiveWindow();
	wxWindow* MainWnd = pMainWnd->m_wxWindow*;

	if ((Active != MainWnd) && (::GetParent(Active) != MainWnd))
	{
		// Revert to pending
		BubbleHelpState = STATE_ACTIVE; // Very important else state machine gets upset!
		SetState(STATE_INITIAL_PENDING);
		return;
	}

	// Don't do bubble help if modal dialog is open.
	if (DialogManager::ModalDialogOpen() || (ModalDialogs > 0))
	{
		// Revert to pending
		BubbleHelpState = STATE_ACTIVE; // Very important else state machine gets upset!
		SetState(STATE_INITIAL_PENDING);
		return;
	}

	// Don't do bubble help if a menu is up	- Windows captures the mouse during a menu
	// processing loop.
	if (::GetCapture() != NULL)
	{
		// Revert to pending
		BubbleHelpState = STATE_ACTIVE; // Very important else state machine gets upset!
		SetState(STATE_INITIAL_PENDING);
		return;
	}

	// also only do bubble help if the control were over is actually visible
	if (::IsWindowVisible (Window) == FALSE)
	{
		// Revert to pending
		BubbleHelpState = STATE_ACTIVE; // Very important else state machine gets upset!
		SetState(STATE_INITIAL_PENDING);
		return;
	}

	// Ok - we're allowed to display bubble help - work out what to do:

#if STATE_TRACE
	if (IsUserName("Tim"))
		TRACE( _T("STATE_ACTIVE\n"));
#endif

	switch (BubbleHelpState)
	{
		case STATE_INITIAL_PENDING:
		case STATE_UPDATE_PENDING:
			BubbleHelpState = STATE_ACTIVE;

			// Create and show a bubble help window
			ENSURE(BubbleWnd == NULL, "Bubble window already exists!");
	
			// Make a new bubble help window
			TRY
			{
				BubbleWnd = new BubbleHelpWnd;
			}
			CATCH(CMemoryException, e)
			{
				TRACEALL( _T("Unable to create bubble help window!\n"));
				return;
			}
			END_CATCH

			// Create the actual window
			if (!BubbleWnd->Create())
			{
				TRACEALL( _T("Could not Init bubble help window\n"));
				return;
			}

			// If ad-hoc bubble help, call provider to get required text, otherwise
			// get resource IDs out of the OpDescriptor in our table of controls.
			if ((AdHocCallback != NULL) && (Window == AdHocWindow))
			{
				char *BubbleText = AdHocCallback(AdHocWindow, AdHocControl, AdHocReference);
				if (BubbleWnd->SetText(BubbleText))
					BubbleWnd->Show();
			}
			else
			{
				// Find the record for this control.
				ControlEntry *pEntry;
				pEntry = Controls->FindControl(Window);

				ENSURE(pEntry != NULL, "Bad Entry in ActiveHandler");

				// If we have an OpDescriptor, then get the bubble help/module IDs from that,
				// otherwise get them directly out of the ControlEntry.
				UINT32 BubbleID;
				UINT32 ModuleID;

				if (pEntry->pOpDesc != NULL)
				{
					BubbleID = pEntry->pOpDesc->GetBubbleId();
					ModuleID = Tool::GetModuleID(pEntry->pOpDesc->GetToolID());
				}
				else
				{
					BubbleID = pEntry->BubbleID;
					ModuleID = pEntry->ModuleID;
				}

				// Load the bubble help text.
				// Ugly hack, but who cares?
				String_256 strCaption;
				GetWindowText(Window, strCaption, 255);
				if (strCaption == String_256("Download"))
				{
					if (BubbleWnd->SetText("Download this resource"))
					// If the string loaded ok, put the window in the correct place and show it.
					BubbleWnd->Show();
				}
				else
				{
					if (BubbleWnd->SetText(BubbleID, ModuleID))
					// If the string loaded ok, put the window in the correct place and show it.
					BubbleWnd->Show();
				}
			}

			break;

		default:
			ENSURE(FALSE, "Wrong state in bubble help");
			break;
	}
}

/********************************************************************************************

>	void ControlHelper::InitialPendingHandler()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Purpose:	Sets the state to STATE_INITIAL_PENDING, which means that the bubble help
				window is about to come up, when a timer has elapsed, providing the user
				doesn't move the cursor away or press a key etc.
	SeeAlso:	ControlHelper::DeadHandler; ControlHelper::ActiveHandler; 
				ControlHelper::InitialDisabledHandler; ControlHelper::DisabledHandler;
				ControlHelper::UpdatePendingHandler

********************************************************************************************/

void ControlHelper::InitialPendingHandler()
{
#if STATE_TRACE
	if (IsUserName("Tim"))
		TRACE( _T("STATE_INITIAL_PENDING\n"));
#endif

	switch (BubbleHelpState)
	{
		case STATE_ACTIVE:
			// Kill off bubble help (if it exists) and fall through
			BubbleHelpKill();

		case STATE_DEAD:
		case STATE_INITIAL_DISABLED:
			// Sample timer (i.e. reset delay)
			PendingTimer.Sample();
			BubbleHelpState = STATE_INITIAL_PENDING;
			break;

		default:
			ENSURE(FALSE, "Wrong state in bubble help");
			break;
	}
}

/********************************************************************************************

>	void ControlHelper::InitialDisabledHandler()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Purpose:	Sets the bubble help state to STATE_INITIAL_DISABLED, which means that the
				cursor is over a Camelot bar but not over a control, or that for some
				reason (key press, focus change etc) bubble help should not be displayed,
				even though the cursor is over a control that supports bubble help.
	SeeAlso:	ControlHelper::DeadHandler; ControlHelper::ActiveHandler; 
				ControlHelper::InitialPendingHandler; ControlHelper::DisabledHandler;
				ControlHelper::UpdatePendingHandler

********************************************************************************************/

void ControlHelper::InitialDisabledHandler()
{
#if STATE_TRACE
	if (IsUserName("Tim"))
		TRACE( _T("STATE_INITIAL_DISABLED\n"));
#endif

	// If a control has focus, then don't change state unless it's in order to die.
	if (ControlHasFocus)
	{
		if (BubbleHelpState != STATE_DEAD)
			return;
	}

	switch (BubbleHelpState)
	{
		case STATE_DEAD:
		case STATE_INITIAL_PENDING:
			// Don't do anything.
			BubbleHelpState = STATE_INITIAL_DISABLED;
			break;

		default:
			ENSURE(FALSE, "Wrong state in bubble help");
			break;
	}
}

/********************************************************************************************

>	void ControlHelper::UpdatePendingHandler()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Purpose:	Set the bubble help state to STATE_UPDATE_PENDING, which means that the
				bubble help will appear very quickly if moved over a control.  This is used
				when the user moves from a control to another control, so it enables the 
				user to move along the controls and browse the bubble text for them quite 
				quickly.
	SeeAlso:	ControlHelper::DeadHandler; ControlHelper::ActiveHandler; 
				ControlHelper::InitialPendingHandler; ControlHelper::InitialDisabledHandler;
				ControlHelper::DisabledHandler

********************************************************************************************/

void ControlHelper::UpdatePendingHandler()
{
#if STATE_TRACE
	if (IsUserName("Tim"))
		TRACE( _T("STATE_UPDATE_PENDING\n"));
#endif

	switch (BubbleHelpState)
	{
		case STATE_ACTIVE:
			// Kill the bubble help window and fall through.
			// NB. There may not really be a bubble help window, as we may have come via a
			// brief 'dummy' ACTIVE state (i.e. ACTIVE refused to be the current state
			// because a mouse button was down or we don't have focus etc).
			BubbleHelpKill();

		case STATE_DISABLED:
			// Sample the pending timer
			PendingTimer.Sample();
			BubbleHelpState = STATE_UPDATE_PENDING;
			break;

		default:
			ENSURE(FALSE, "Wrong state in bubble help");
			break;
	}
}

/********************************************************************************************

>	void ControlHelper::DisabledHandler()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Purpose:	Set the bubble help state to STATE_DISABLED, which means that the
				bubble help will appear very quickly if moved over a control.  This is used
				when the user moves from a control to a Camelot bar, so it enables the 
				user to move between bars (or even controls which are separated on a bar)
				and browse the bubble text for them quite quickly.
	SeeAlso:	ControlHelper::DeadHandler; ControlHelper::ActiveHandler; 
				ControlHelper::InitialPendingHandler; ControlHelper::InitialDisabledHandler;
				ControlHelper::UpdatePendingHandler

********************************************************************************************/

void ControlHelper::DisabledHandler()
{
#if STATE_TRACE
	if (IsUserName("Tim"))
		TRACE( _T("STATE_DISABLED\n"));
#endif

	// If a control has focus, then don't change state unless it's in order to die.
	if (ControlHasFocus)
	{
		if (BubbleHelpState != STATE_DEAD)
			return;
	}

	switch (BubbleHelpState)
	{
		case STATE_ACTIVE:
			// Kill the bubble help window and fall through.
			ENSURE(BubbleWnd != NULL, "No bubble help window!");
			BubbleHelpKill();

		case STATE_UPDATE_PENDING:
			BubbleHelpState = STATE_DISABLED;
			break;

		default:
			ENSURE(FALSE, "Wrong state in bubble help");
			break;
	}
}

/********************************************************************************************

>	void ControlHelper::BubbleHelpDisable()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Purpose:	Cause the bubble help to go into its 'disabled' state, so that even if it
				is over a control that supports bubble help, bubble help will not be 
				displayed.  If bubble help is currently displayed, it is removed.
				This function should be called on events that ought to kill off bubble
				help, e.g. key presses, focus switches etc.
	SeeAlso:	ControlHelper::BubbleHelpKill

********************************************************************************************/

void ControlHelper::BubbleHelpDisable()
{
	// If bubble help is around, disable it.
	switch (BubbleHelpState)
	{
		case STATE_ACTIVE:
		case STATE_UPDATE_PENDING:
			// Bubble help is on screen or will be RSN so slide into the disabled
			// state in order to get rid of/avoid it.
			SetState(STATE_DISABLED);
			break;

		case STATE_INITIAL_PENDING:
			// Bubble help will be on screen RSN so slide into the initial disabled
			// state in order to get rid of/avoid it.
			SetState(STATE_INITIAL_DISABLED);
			break;
	}
}

/********************************************************************************************

>	void ControlHelper::BubbleHelpKill()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Purpose:	Low level function to kill off the bubble help window and delete it, and
				set various variables for ad-hoc bubble help to sensible 'null' values.
	SeeAlso:	ControlHelper::BubbleHelpDisable
	Scope:		Private

********************************************************************************************/

void ControlHelper::BubbleHelpKill()
{
	// Destroy the bubble help window.
	if (BubbleWnd != NULL)
	{
		BubbleWnd->Hide();
		delete BubbleWnd;
		BubbleWnd = NULL;
	}

	// Disable ad-hoc bubble help.
	// NB we don't change the AdHocControl, because then we know that we're over the same
	// control and should remain disabled (e.g. if user clicks on button and bubble help
	// goes away, it shouldn't come back until they move to another control).
	AdHocWindow = NULL;
	AdHocCallback = NULL;
	AdHocReference = NULL;
}

/********************************************************************************************

>	void ControlHelper::DoBubbleHelpOn(wxWindow* Window, UINT32 NewControl, 
									   BubbleHelpCallback Callback, void *Reference)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/06/94
	Inputs:		Window - the window to provide bubble help on.
				NewControl - pseudo-ID of the imaginary control - this is so the bubble
							 knows when a different control has been moved onto, and hence
							 that it should update the bubble help window.
				Callback - the callback routine for ad-hoc bubble help - this is called to
						   get the bubble help text when the bubble window needs to be shown.
				Reference - arbritrary data that the Callback may make use of to provide
							it with context information for the callback event.
	Purpose:	Provide bubble-help on an ad-hoc basis, i.e. this allows bubble help to
				be provided for GUI elements which are not necessarily 'controls' as such
				on a Camelot bar.  For example the colour bar uses this to pretend that
				each colour swatch is a control, and provides the bubble help to be 
				shown on it.  This function should be called on mouse move messages over
				such GUI elements. (It cannot be done automatically by the bubble help
				system because we do not subclass such elements - because they're not
				really individual windows!).

********************************************************************************************/

void ControlHelper::DoBubbleHelpOn(wxWindow* Window, UINT32 NewControl, 
								   BubbleHelpCallback Callback, void *Reference)
{
	// Caller is telling us that they have detected a mouse move over one of their
	// controls - therefore take a record of their window handle, and fake an event
	// to the state machine.
	AdHocWindow = Window;
	AdHocCallback = Callback;
	AdHocReference = Reference;

	// Work out if this is a different control we're being told about.
	AdHocControlIsDifferent = (AdHocControl != NewControl);

	// Remember control ID for next time (we do this before calling BubbleHelpStateMachine()
	// because it uses AdHocControl to display the bubble help when it flips into the
	// Active state).
	AdHocControl = NewControl;

	// We give our WndProc to the state machine so that it thinks that NewControl is one of
	// our 'proper' controls.
	POINT Pos;
	::GetCursorPos(&Pos);
	BubbleHelpStateMachine(Pos, Window, ControlHelper::MyWndProc);
}


/********************************************************************************************

>	void ControlHelper::InformModalDialogOpened()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/94
	Purpose:	Informs the ControlHelper class that a modal dialog which is NOT managed
				by the kernel Dialog Manager (i.e. NOT a DialogOp) has been opened.
				This allows us to disable bubble help whiole modal dialogs are open.
				Examples of the use of this are the error box, or use of the Windows
				Common Dialog functions.
	SeeAlso:	ControlHelper::DecModalDialogCount

********************************************************************************************/

void ControlHelper::InformModalDialogOpened()
{
	ModalDialogs++;
	// Record the currently active window
	DialogManager::RecordActiveDialogState();
 	// This is the ideal place to disable all modeless dialogs
	DialogManager::EnableAllDialogs(FALSE);
}


/********************************************************************************************

>	void ControlHelper::InformModalDialogClosed()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/09/94
	Purpose:	Informs the ControlHelper class that a modal dialog which is NOT managed
				by the kernel Dialog Manager (i.e. NOT a DialogOp) has been closed.
				This allows us to disable bubble help whiole modal dialogs are open.
				Examples of the use of this are the error box, or use of the Windows
				Common Dialog functions.
	SeeAlso:	ControlHelper::IncModalDialogCount

********************************************************************************************/

void ControlHelper::InformModalDialogClosed()
{
	ModalDialogs--;

	// Restore the active window
	DialogManager::RestoreActiveDialogState();

	// Sanity check
	ENSURE(ModalDialogs >= 0, 
		   "Unbalanced call to ControlHelper::InformModalDialogClosed()!");

}


/********************************************************************************************

>	BOOL ControlHelper::IsUserInterfaceCaptured()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/01/95
	Returns:	TRUE => User interface is captured in some way (e.g. a menu is up)
				FALSE => User interface is not captured.
	Purpose:	Determine if the user interface is 'captured' in any way, e.g. if a
				bar is being dragged or a menu is up, and so on. This is used to 
				determine whether or not to do certain background/idle operations, e.g.
				background rendering and the like.  This does NOT include drag operations
				such as any tool drags etc., as this would mean that the push tool would
				not work properly etc.

********************************************************************************************/

BOOL ControlHelper::IsUserInterfaceCaptured()
{
	// Bodge for delta - don't ever disable background rendering.
	return FALSE;

	if (Operation::GetCurrentDragOp() != NULL)
		// Camelot drag in progress - this does not count as capture.
		return FALSE;

	// Find out the handle of our main window
	CWnd *pMainCWnd = AfxGetMainWnd();

	wxWindow* MainwxWindow* = NULL;

	if (pMainCWnd != NULL)
		MainwxWindow* = pMainCWnd->GetSafewxWindow*();

	// Find out which window, if any, is captured.
	wxWindow* CapturedwxWindow* = ::GetCapture();

	// Return TRUE if mouse is captured, and it is not our window.
	return ((CapturedwxWindow* != NULL) && (CapturedwxWindow* != MainwxWindow*));
}
