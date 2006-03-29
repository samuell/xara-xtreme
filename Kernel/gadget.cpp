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
// 


#include "camtypes.h"
#include "dialogop.h"
#include "gadget.h"


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(CCCoreUI, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_MEMDUMP(Gadget, CCCoreUI);
CC_IMPLEMENT_MEMDUMP(NotifyingGadget, Gadget);
CC_IMPLEMENT_MEMDUMP(DialogMgrGadget, NotifyingGadget);
CC_IMPLEMENT_MEMDUMP(Button, DialogMgrGadget);
CC_IMPLEMENT_MEMDUMP(ListControl, DialogMgrGadget);
CC_IMPLEMENT_MEMDUMP(TextControl, DialogMgrGadget);
CC_IMPLEMENT_MEMDUMP(StaticTextControl, DialogMgrGadget);


// We want better memory tracking
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


// Functions follow


/*******************************************************************************************

>	Button::Button( DialogOp* const pDialog, CGadgetID ControlID,
					Notifiable* pNotify = &Nobody )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Default destructor for a Button. Does nothing


*******************************************************************************************/
Button::Button( DialogOp* const pDialog, CGadgetID ControlID, Notifiable* const pNotify): 
		DialogMgrGadget(pDialog, ControlID), m_pNotifyOnPress(pNotify)
{
}


/*******************************************************************************************

>	Button::~Button()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Default destructor for a Button. Does nothing


*******************************************************************************************/
Button::~Button()
{
}


/*******************************************************************************************

>	void Button::OnClicked()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Notifies the Notifiable that this Button has been clicked


*******************************************************************************************/
void Button::OnClicked()
{
	Notify(m_pNotifyOnPress);
}


/*******************************************************************************************

>	TextControl::TextControl(	DialogOp* const pDialog, CGadgetID ControlID,
								const StringBase& InitialText = EmptyString,
								Notifiable* const pTell = &Nobody )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Default constructor for a TextControl

*******************************************************************************************/
TextControl::TextControl(	DialogOp* const pDialog, CGadgetID ControlID,
							const StringBase& InitialText,
							Notifiable* const pTell ) :
		DialogMgrGadget(pDialog, ControlID),
		m_pObserver(pTell)
{
	// The SetWindowText sends an EN_CHANGE, which calls OnChange, which
	// notifies the Text_control, which updates the String, which notifies
	// the Text_control, which updates the window text, and around and
	// and around. Solve the problem in the OnChange() function, but set
	// "initializing" true to tell OnChange what's going on. Turn off
	// notification to prevent this.
	//
	// I'm assuming synchronous behavior; I don't expect SentWindowText
	// to return until AFTER the EnChange() handler has been executed.
	// If this turns out not to be the case in the future, you'll have
	// to modify this code: add an "initializing" field to the class,
	// set it true instead of calling notify_off, set it false instead
	// of calling notify_on, and modify OnChange to not notify anybody
	// if "initializing" is true.


	if (!InitialText.IsEmpty())
	{
		NotifyOff();
		String_256 NonConstIdiocy = InitialText;
		pDialog->SetStringGadgetValue(m_ControlID, &NonConstIdiocy);
		NotifyOn();
	}
}


const TextControl& TextControl::operator=(const TextControl& OtherControl)
{
	ERROR3("TextControl assignment not implemented\n");

	return *this;
}

TextControl::TextControl(const TextControl& OtherControl) :
	DialogMgrGadget(NULL, 0)
{
	ERROR3("TextControl copy constructor not implemented\n");
}


/*******************************************************************************************

>	void TextControl::UpdateText(const StringBase &NewString)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Default destructor for a TextControl

	Input:		NewString : the string to set in the text control

*******************************************************************************************/
void TextControl::UpdateText(const StringBase &NewString)
{
	ASSERT(m_ControlID != 0 && m_pDialog != NULL);

	String_256 NonConstIdiocy = NewString;
	m_pDialog->SetStringGadgetValue(m_ControlID, &NonConstIdiocy);
	Notify(m_pObserver);
}


/*******************************************************************************************

>	void TextControl::RetrieveText(String* pDisplayedString) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Default destructor for a TextControl

	Outputs:	pDisplayedString : the string to set in the text control

*******************************************************************************************/
void TextControl::RetrieveText(StringBase* const pDisplayedString) const
{
	ASSERT(m_ControlID != 0 && m_pDialog != NULL);

	if (pDisplayedString != NULL)
	{
		*pDisplayedString = m_pDialog->GetStringGadgetValue(m_ControlID);
	}
}


/*******************************************************************************************

>	void TextControl::OnChange() 

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Default destructor for a TextControl

	Outputs:	pDisplayedString : the string to set in the text control

*******************************************************************************************/
void TextControl::OnChange() 
{
	Notify(m_pObserver);
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//	StaticTextControl
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //




/*******************************************************************************************

>	StaticTextControl::StaticTextControl(	DialogOp* const pDialog, CGadgetID ControlID,
									const StringBase& InitialText = EmptyString,
									Notifiable* const pTell = &Nobody )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Default constructor for a TextControl

*******************************************************************************************/
StaticTextControl::StaticTextControl(	DialogOp* const pDialog, CGadgetID ControlID,
										const StringBase& InitialText,
										Notifiable* const pTell ) :
	DialogMgrGadget(pDialog, ControlID),
	m_pObserver(pTell)
{
	if (!InitialText.IsEmpty())
	{
		String_256 NonConstIdiocy = InitialText;
		pDialog->SetStringGadgetValue(m_ControlID, &NonConstIdiocy);
	}
}


/*******************************************************************************************

>	void StaticTextControl::UpdateText(const StringBase &NewString)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Sets the text in this control

	Input:		NewString : the string to set in the static control (label)

*******************************************************************************************/
void StaticTextControl::UpdateText(const StringBase &NewString)
{
	ASSERT(m_ControlID != 0 && m_pDialog != NULL);

	String_256 NonConstIdiocy = NewString;
	m_pDialog->SetStringGadgetValue(m_ControlID, &NonConstIdiocy);
	Notify(m_pObserver);
}


/*******************************************************************************************

>	void StaticTextControl::RetrieveText(String* pDisplayedString) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Gets the text currently displayed in this StaticTextControl

	Outputs:	pDisplayedString : the string to set in the text control

*******************************************************************************************/
void StaticTextControl::RetrieveText(StringBase* const pDisplayedString) const
{
	ASSERT(m_ControlID != 0 && m_pDialog != NULL);

	if (pDisplayedString != NULL)
	{
		*pDisplayedString = m_pDialog->GetStringGadgetValue(m_ControlID);
	}
}


/*******************************************************************************************

>	void StaticTextControl::OnChange() 

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Don't use this (yet!)

*******************************************************************************************/
inline void StaticTextControl::OnChange() 
{
	Notify(m_pObserver);
}


const StaticTextControl& StaticTextControl::operator=(const StaticTextControl& OtherControl)
{
	ERROR3("StaticTextControl assignment not implemented\n");

	return *this;
}

StaticTextControl::StaticTextControl(const StaticTextControl& OtherControl) :
	DialogMgrGadget(NULL, 0)
{
	ERROR3("StaticTextControl copy constructor not implemented\n");
}

