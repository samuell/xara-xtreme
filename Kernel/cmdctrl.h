// $Id: cmdctrl.h 662 2006-03-14 21:31:49Z alex $
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
 *  */

/********************************************************************************************

>	Class CmdControl : public ListItem

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Purpose:	
		A CmdControl is an abstract class from which all types of interface controls inherit. 
		These controls include MenuItems, Keyboard Accelerators, Buttons etc. They all have 
		one main thing in common and that is that they are built out of Operations. Unlike 
		Tools Controls have only one operation behind them.

	Errors:		None yet.
	SeeAlso:	ListItem

********************************************************************************************/

#ifndef INC_CMD_CONTROL
#define INC_CMD_CONTROL

#include "list.h"
#include "listitem.h" 
#include "strings.h"  
#include "opdesc.h"

/********************************************************************************************
>	class CCAPI CmdControl : public ListItem

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	sometime in 1993
	Purpose:	Base class for Camelot menu items.  Unfortunately neither Mario or Andy
				ever got round to documenting this stuff, ha ha ha
	SeeAlso:	-
********************************************************************************************/

class CCAPI CmdControl : public ListItem
{ 
public:
	
	CmdControl() { /* Empty */ }
	CmdControl(OpDescriptor* Owner);

	virtual BOOL IsEnabled();
	virtual void PerformAction();
	virtual BOOL UpdateControlState();
	virtual String_256* GetWhyDisabled();

	virtual OpDescriptor* GetOpDescriptor() const;

protected:                        
	UINT32 ControlId;
	String_256 WhyDisabled;
	OpState ControlState;
	OpDescriptor* OwnerOperation;

private:
	CC_DECLARE_MEMDUMP(CmdControl);
};

/********************************************************************************************

>	Class HotKeyO : public CmdControl

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	
		The HotKey class represents keyboard accelerators for menu items and other short-cut 
		keys used for other types of controls. HotKey inherits from the CmdControl class 
		provides it with the ability to perform an operation which is the commonality between
		all types of CmdControls.

	Errors:		None yet.
	SeeAlso:	CmdControl

********************************************************************************************/

#if 0
// Not obvious when & why this was all disabled - AB20060106
class CCAPI HotKeyO : public CmdControl
{ 

	CC_DECLARE_MEMDUMP(HotKeyO);

private:
	// List of all Hotkeys
	static List HotKeys; 
	
	String_256 Description;
	String_256 MenuTextDesc;

public:
	
	HotKeyO() {}

	HotKeyO( OpDescriptor *Owner, String_256 *MenuText );

	BOOL UpdateControlState();

	UINT32 GetHotKeyId();
	
	String_256* GetDescription();
	String_256* GetMenuTextDesc();
	
   	static UINT32 AutomaticHotKeyID;
	static UINT32 GetNextAutomaticHotKeyID();

	// a search function that returns a pointer to the HotKey
	static HotKeyO* FindHotKey(UINT32 HotKeyId);
	
	static HotKeyO* FindHotKey(OpDescriptor* OpDesc);

	// Need a method of killing off all the HotKeys at program end
	static void DestroyAll();

/********************************************************************************************

>	static HotKey* HotKey::GetFirstHotKey()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	returns the first HotKey in the list

********************************************************************************************/
	static HotKeyO* GetFirstHotKey()	
	{ 
		return (HotKeyO*) HotKeys.GetHead(); 
	}


	
/********************************************************************************************

>	static HotKey* HotKey::GetNextHotKey( HotKey* CurrHotKey )

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		pointer to last HotKey return by GetFirst to GetNextHotKey
	Purpose:	return the next HotKey in the list

********************************************************************************************/
	static HotKeyO* HotKeyO::GetNextHotKey( HotKeyO* CurrHotKey )
	{ 
		return (HotKeyO*) HotKeys.GetNext( CurrHotKey ); 
	}



private:          

/********************************************************************************************

>	static void HotKey::LinkHotKey( HotKey* ThisHotKey )

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		pointer to this HotKey
	Purpose:	Add the HotKey to the list of all HotKeys
	Scope:		private

********************************************************************************************/

	static void HotKeyO::LinkHotKey( HotKeyO* ThisHotKey )
	{ 
		HotKeys.AddTail(ThisHotKey); 
	}

};

/********************************************************************************************

>	Class Button : public CmdControl

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	
		The Button class represents keyboard accelerators for menu items and other short-cut 
		keys used for other types of controls. Button inherits from the CmdControl class 
		provides it with the ability to perform an operation which is the commonality between
		all types of CmdControls.

	Errors:		None yet.
	SeeAlso:	CmdControl

********************************************************************************************/

class CCAPI Button : public CmdControl
{ 

	CC_DECLARE_MEMDUMP(Button);

private:
	// List of all Button Bar
	static List ButtonBar;

	static UINT32 NumberOfButtons;
	static UINT32 NumberOfSeparators;

	String_256 Description;
	
	BOOL FollowedBySeparator;
	
	UINT32 BitMapOffset;	

public:
	
	Button() {}

	Button( OpDescriptor *Owner, BOOL Separator, UINT32 BitMapNo = 0 );

	BOOL UpdateControlState();

	UINT32 GetButtonId();
	
	String_256* GetDescription();
	
   	static UINT32 AutomaticButtonID;
	static UINT32 GetNextAutomaticButtonID();

	// a search function that returns a pointer to the Button
	static Button* FindButton(UINT32 ButtonId);
	
	// Need a method of killing off all the Buttons at program end
	static void DestroyAll();

/********************************************************************************************

>	static Button* Button::GetFirstButton()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	returns the first Button in the list

********************************************************************************************/
	static Button* GetFirstButton()	
	{ 
		return (Button*) ButtonBar.GetHead(); 
	}


	
/********************************************************************************************

>	static Button* Button::GetNextButton( Button* CurrButton )

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		pointer to last Button return by GetFirst to GetNextButton
	Purpose:	return the next Button in the list

********************************************************************************************/
	static Button* Button::GetNextButton( Button* CurrButton )
	{ 
		return (Button*) ButtonBar.GetNext( CurrButton ); 
	}

/********************************************************************************************

>	static UINT32 Button::GetNumberOfButtons()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	Number of buttons on button bar
	Purpose:	returns the number of buttons on the button bar

********************************************************************************************/
	static UINT32 Button::GetNumberOfButtons()
	{ 
		return NumberOfButtons; 
	}

/********************************************************************************************

>	static UINT32 Button::GetNumberOfSeparators()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Outputs:	Number of Separators on button bar
	Purpose:	returns the number of separators on the button bar

********************************************************************************************/
	static UINT32 Button::GetNumberOfSeparators()
	{ 
		return NumberOfSeparators; 
	}

/********************************************************************************************

>	BOOL Button::IsFollowedBySeparator()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Returns:	TRUE if Button is followed by a separatorand FALSE otherwise
	Purpose:	Determines whether a button is followed by a separator on the Button bar

********************************************************************************************/
	BOOL Button::IsFollowedBySeparator()
	{ 
		return FollowedBySeparator; 
	}

/********************************************************************************************

>	UINT32 Button::GetBitMapOffset()

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/93
	Returns:	The Bitmap offset of the Button icon
	Purpose:	Determines the bitmap offset of the button's icon

********************************************************************************************/
	UINT32 Button::GetBitMapOffset()
	{ 
		return BitMapOffset; 
	}

private:          

/********************************************************************************************

>	static void Button::LinkButton( Button* ThisButton )

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Inputs:		pointer to this Button
	Purpose:	Add the Button to the list of all Buttons
	Scope:		private

********************************************************************************************/

	static void Button::LinkButton( Button* ThisButton )
	{ 
		ButtonBar.AddTail(ThisButton); 
	}

};

#endif

#endif
