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


#include "camtypes.h"
#include "cmdctrl.h"
#include "ensure.h"
//#include "mario.h"
#include "errors.h"
//#include "oilkeys.h"
//#include "oilbtns.h"

DECLARE_SOURCE("$Revision$");  

CC_IMPLEMENT_MEMDUMP(CmdControl, 	ListItem)
//CC_IMPLEMENT_MEMDUMP(HotKeyO, 		CmdControl)
//CC_IMPLEMENT_MEMDUMP(Button, 		CmdControl)

/********************************************************************************************

>	CmdControl::CmdControl(	OpDescriptor *Owner )

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Inputs:		Pointer to owner OpDescriptor
	Outputs:	None
	Returns:	None
	Purpose:	Constructor for the CmdControl Class.	
	Errors:		None

********************************************************************************************/

CmdControl::CmdControl(OpDescriptor *Owner)
{
	ENSURE( Owner, "No owner for CmdControl" );

	ControlId		= 0;
	OwnerOperation	= Owner;
	ControlState	= OpState(FALSE, TRUE);
	
    WhyDisabled.Empty();
}

/********************************************************************************************

>	BOOL CmdControl::IsEnabled()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Inputs:		None
	Outputs:	None
	Returns:	True if CmdControl is enabled
				False if CmdControl is disabled
	Purpose:	
		Identifies if the current CmdControl is available. This function is used when updating 
		the state of a CmdControl.
	Errors:		None

********************************************************************************************/

BOOL CmdControl::IsEnabled()
{
	return (!ControlState.Greyed);
}

/********************************************************************************************

>	String_256 *CmdControl::GetWhyDisabled()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Inputs:		None
	Outputs:	None
	Returns:	If Succesful returns a string representing why the CmdControl is disabled
				otherwise NULL
	Purpose:	Gets the reason why a CmdControl has been disabled
	Errors:		None

********************************************************************************************/

String_256 *CmdControl::GetWhyDisabled()
{
   	return &WhyDisabled;
}

/********************************************************************************************

>	void CmdControl::PerformAction()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Performs the action assoicated with a CmdControl. 
	Errors:		None

********************************************************************************************/

void CmdControl::PerformAction()
{
	ENSURE(OwnerOperation, "Cannot Perform Action with NULL Owner");

	OwnerOperation->Invoke();
}

/********************************************************************************************

>	BOOL CmdControl::UpdateControlState()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if state is updated 
				FALSE otherwise
	Purpose:	Obtains the state of a CmdControl. 
	Errors:		None

********************************************************************************************/

BOOL CmdControl::UpdateControlState()
{
	return TRUE;                      
}



/********************************************************************************************
>	virtual OpDescriptor* CmdControl::GetOpDescriptor() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/95
	Returns:	A pointer to the OpDescriptor associated with this object.
********************************************************************************************/

OpDescriptor* CmdControl::GetOpDescriptor() const
{
	return OwnerOperation;
}




/********************************************************************************************
								HotKeyO CLASS
********************************************************************************************/

// Here is the static list that belongs to OpDescriptor
//List HotKeyO::HotKeys;


/********************************************************************************************

>	HotKeyO::HotKeyO( OpDescriptor *Owner, String_256* MenuText )

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		Pointer to owner OpDescriptor
	Outputs:	None
	Returns:	None
	Purpose:	Constructor for the HotKey Class.HotKey
	Errors:		None

********************************************************************************************/

/*
HotKeyO::HotKeyO( OpDescriptor *Owner, String_256* MenuText )
 : CmdControl(Owner)
{               	
	ControlId = GetNextAutomaticHotKeyID();
	Owner->GetText(&Description, OP_DESC_TEXT);
	MenuTextDesc = *MenuText;
	LinkHotKey(this);
}
*/	
/********************************************************************************************

>	UINT32 HotKeyO::GetHotKeyId()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		None
	Outputs:	None
	Returns:	HotKey Id of current HotKey
	Purpose:	Gets the HotKey Id of the current HotKey.
	Errors:		None

********************************************************************************************/

/*
UINT32 HotKeyO::GetHotKeyId()
{
	return ControlId;
}
*/
/********************************************************************************************

>	BOOL HotKeyO::UpdateControlState()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if state is updated 
				FALSE otherwise
	Purpose:	Obtains the state of a HotKey. 
	Errors:		None

**************************** ****************************************************************/

/*
BOOL HotKeyO::UpdateControlState()
{
	ENSURE(OwnerOperation, "Cannot update the state of a NULL operation");

	String_256 HotKeyDesc;
	
	// get new hotkey state                            
	OpState newState = OwnerOperation->GetOpsState( &HotKeyDesc);

	// if a description has been returned
	if (!HotKeyDesc.IsEmpty() && (newState.Greyed))
	{
		String_256 disabledStub(_R(IDS_DISABLED_BECAUSE));
				
		WhyDisabled = disabledStub;

		// Set up reason why item is disabled
		WhyDisabled += HotKeyDesc;
    }
    
	// if the state has changed
	if ((newState.Greyed != ControlState.Greyed) || 			// if changed then
		(newState.Ticked != ControlState.Ticked))
	{
		ControlState = newState;								// record new state
		return TRUE;  
	}

	return FALSE;
}
*/
/********************************************************************************************

>	String_256 *HotKeyO::GetDescription()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/93
	Inputs:		None
	Outputs:	None
	Returns:	If Succesfull returns a string representing a description of the HotKey
				otherwise NULL
	Purpose:	Gets HotKey Text Description.
	Errors:		None

********************************************************************************************/

/*
String_256 *HotKeyO::GetDescription()
{
   	return &Description;
}
*/
/********************************************************************************************

>	String_256 *HotKeyO::GetMenuTextDesc()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/93
	Inputs:		None
	Outputs:	None
	Returns:	If Succesfull returns a string representing a menu text of the HotKey
				otherwise NULL
	Purpose:	Gets HotKey Menu Text Description.
	Errors:		None

********************************************************************************************/

/*
String_256 *HotKeyO::GetMenuTextDesc()
{
   	return &MenuTextDesc;
}

// this is used to automatically create unique HotKey IDs. It ranges from two (OIL specific)
// values, AUTO_HKEY_ID_MIN and AUTO_HKEY_ID_MAX inclusive (defined in oilmenus.h)

UINT32 HotKeyO::AutomaticHotKeyID = AUTO_HKEY_ID_MIN;

*/
/********************************************************************************************

>	static UINT32 GetNextAutomaticHotKeyID()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		None
	Outputs:	None
	Returns:	The next automatic HotKey ID value
	Purpose:	Creates a unique ID for each HotKey, based on a monotonic increasing value
				(whose range is determined by the OIL layer). When the range fills, a search
				is done to find individual unused IDs.
	Friend:		HotKey
	Errors:		Will ENSURE when range is filled.
	Scope:		Private

********************************************************************************************/

/*
UINT32 HotKeyO::GetNextAutomaticHotKeyID()
{
	if (AutomaticHotKeyID > AUTO_HKEY_ID_MAX)
	{
		// we have run out of IDs, so search for gaps in the allocation
		// UNFINISHED CODE!!
		ENSURE( FALSE, "Run out of HotKey IDs" );
		return 0;
	}
	else
	{
		return AutomaticHotKeyID++;				// return old value, inc for next time
	}
	
}
*/
/********************************************************************************************

>	HotKeyO* HotKeyO::FindHotKey(UINT32 HotKeyId)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		HotKey Identifier
	Returns:	a pointer to the HotKey, or NULL if it could not be found
	Purpose:	To search the HotKey list and find the HotKey described by the id passed in.

********************************************************************************************/

/*
HotKeyO* HotKeyO::FindHotKey(UINT32 HotKeyId)
{
	HotKeyO* CurrentItem = GetFirstHotKey();
		
	while (CurrentItem != NULL)
	{
		// if we have found a match, return a pointer to it
		if ( CurrentItem -> GetHotKeyId() == HotKeyId )
			return CurrentItem;
		
		CurrentItem = GetNextHotKey( CurrentItem );		// next item in the list
	}
	
	return NULL;		// failed to find a match, so return null
}

*/
/********************************************************************************************

>	HotKeyO* HotKeyO::FindHotKey(OpDescriptor* OpDesc)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		pointer to a op descriptor
	Returns:	a pointer to the HotKey, or NULL if it could not be found
	Purpose:	To search the HotKey list and find the HotKey containing the 
				opdescriptor passed in.

********************************************************************************************/

/*
HotKeyO* HotKeyO::FindHotKey(OpDescriptor* OpDesc)
{
	HotKeyO* CurrentItem = GetFirstHotKey();
		
	while (CurrentItem != NULL)
	{
		// if we have found a match, return a pointer to it
		if ( CurrentItem -> OwnerOperation == OpDesc )
			return CurrentItem;
		
		CurrentItem = GetNextHotKey( CurrentItem );		// next item in the list
	}
	
	return NULL;		// failed to find a match, so return null
}

*/
/********************************************************************************************

>	void HotKeyO::DestroyAll()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	Since the HotKey maintains its own static list of all the
				HotKeys created in the program, and since none of them are actually
				destroyed with a call to a destructor, we have to do it ourselves by
				a call to this function in the DeInit kernel type of function. Basically
				it walks through the list of OpDescriptors and destroys them all.

********************************************************************************************/

/*
void HotKeyO::DestroyAll()
{
	ListItem* Item;
	
	while( (Item = HotKeys.RemoveHead()) != NULL )
		delete Item;
} 
*/
/********************************************************************************************
								Button CLASS
********************************************************************************************/
/*

// Here is the static list that belongs to OpDescriptor
List Button::ButtonBar;

UINT32 Button::NumberOfButtons = 0;
UINT32 Button::NumberOfSeparators = 0;
*/
/********************************************************************************************

>	Button::Button( OpDescriptor *Owner, BOOL separator, UINT32 BitMapNo)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		Pointer to owner OpDescriptor
				Separator - TRUE if button is followed by a separator, FALSE otherwise
				BitMapNo - BitMap id of button icon
	Outputs:	None
	Returns:	None
	Purpose:	Constructor for the Button Class.
	Errors:		None

********************************************************************************************/

/*
Button::Button( OpDescriptor *Owner, BOOL Separator, UINT32 BitMapNo)
 : CmdControl(Owner)
{               	
	ControlId = GetNextAutomaticButtonID();
	
	Owner->GetText(&Description, OP_DESC_TEXT);

	NumberOfButtons++;

	FollowedBySeparator = Separator;

	if (FollowedBySeparator)
		NumberOfSeparators++;

	BitMapOffset = BitMapNo;

	LinkButton(this);
}
*/	
/********************************************************************************************

>	UINT32 Button::GetButtonId()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		None
	Outputs:	None
	Returns:	Button Id of current Button
	Purpose:	Gets the Button Id of the current Button.
	Errors:		None

********************************************************************************************/

/*
UINT32 Button::GetButtonId()
{
	return ControlId;
}
*/
/********************************************************************************************

>	BOOL Button::UpdateControlState()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if state is updated 
				FALSE otherwise
	Purpose:	Obtains the state of a Button. 
	Errors:		None

*********************************************************************************************/

/*
BOOL Button::UpdateControlState()
{
	ENSURE(OwnerOperation, "Cannot update the state of a NULL operation");

	String_256 ButtonDesc;
	
	// get new Button state                            
	OpState newState = OwnerOperation->GetOpsState( &ButtonDesc);

	// if a description has been returned
	if (!ButtonDesc.IsEmpty() && (newState.Greyed))
	{
		String_256 disabledStub(_R(IDS_DISABLED_BECAUSE));
				
		WhyDisabled = disabledStub;

		// Set up reason why item is disabled
		WhyDisabled += ButtonDesc;
    }
    
	// if the state has changed
	if ((newState.Greyed != ControlState.Greyed) || 			// if changed then
		(newState.Ticked != ControlState.Ticked))
	{
		ControlState = newState;								// record new state
		return TRUE;  
	}

	return FALSE;
}
*/
/********************************************************************************************

>	String_256 *Button::GetDescription()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/93
	Inputs:		None
	Outputs:	None
	Returns:	If Succesfull returns a string representing a description of the Button
				otherwise NULL
	Purpose:	Gets Button Text Description.
	Errors:		None

********************************************************************************************/

/*
String_256 *Button::GetDescription()
{
   	return &Description;
}

// this is used to automatically create unique Button IDs. It ranges from two (OIL specific)
// values, AUTO_HKEY_ID_MIN and AUTO_HKEY_ID_MAX inclusive (defined in oilmenus.h)

UINT32 Button::AutomaticButtonID = AUTO_BUTN_ID_MIN;

*/
/********************************************************************************************

>	static UINT32 GetNextAutomaticButtonID()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		None
	Outputs:	None
	Returns:	The next automatic Button ID value
	Purpose:	Creates a unique ID for each Button, based on a monotonic increasing value
				(whose range is determined by the OIL layer). When the range fills, a search
				is done to find individual unused IDs.
	Friend:		Button
	Errors:		Will ENSURE when range is filled.
	Scope:		Private

********************************************************************************************/

/*
UINT32 Button::GetNextAutomaticButtonID()
{
	if (AutomaticButtonID > AUTO_BUTN_ID_MAX)
	{
		// we have run out of IDs, so search for gaps in the allocation
		// UNFINISHED CODE!!
		ENSURE( FALSE, "Run out of Button IDs" );
		return 0;
	}
	else
	{
		return AutomaticButtonID++;				// return old value, inc for next time
	}
	
}
*/
/********************************************************************************************

>	Button* Button::FindButton(UINT32 ButtonId)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		Button Identifier
	Returns:	a pointer to the Button or NULL if it could not be found
	Purpose:	To search the Button list and find the Button described by the id passed in.

********************************************************************************************/

/*
Button* Button::FindButton(UINT32 ButtonId)
{
	Button* CurrentItem = GetFirstButton();
		
	while (CurrentItem != NULL)
	{
		// if we have found a match, return a pointer to it
		if ( CurrentItem -> GetButtonId() == ButtonId )
			return CurrentItem;
		
		CurrentItem = GetNextButton( CurrentItem );		// next item in the list
	}
	
	return NULL;		// failed to find a match, so return null
}


*/
/********************************************************************************************

>	void Button::DestroyAll()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	Since the Button maintains its own static list of all the
				Buttons created in the program, and since none of them are actually
				destroyed with a call to a destructor, we have to do it ourselves by
				a call to this function in the DeInit kernel type of function. Basically
				it walks through the list of OpDescriptors and destroys them all.

********************************************************************************************/

/*
void Button::DestroyAll()
{
	ListItem* Item;
	
	while( (Item = ButtonBar.RemoveHead()) != NULL )
		delete Item;
} 

*/
