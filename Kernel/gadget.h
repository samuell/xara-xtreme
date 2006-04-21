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

#ifndef INC_GADGET
#define INC_GADGET

#include "dialogop.h"

#ifndef ENSURE_NOT_NULL
#define ENSURE_NOT_NULL(p)	ERROR2IF(p == NULL, 0, "NULL Args");
#endif
#ifndef VOID_ENSURE_NOT_NULL
#define VOID_ENSURE_NOT_NULL(p)		if (p == NULL) {	ERROR2RAW("NULL Args");	}
#endif
#ifndef STRING_ENSURE_NOT_NULL
#define STRING_ENSURE_NOT_NULL(p)	ERROR2IF(p == NULL, NullString, "NULL Args");
#endif

extern const String_8 NullString;

/**************************************************************************************

>	class CCGadget : public CCCoreUI

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96
	Purpose:	Base class for a UI element

**************************************************************************************/

class CCCoreUI : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(CCCoreUI);
public:
//	Resize();
//	QueryGeometry();
};


/**************************************************************************************

>	class CCGadget : public CCCoreUI

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96
	Purpose:	Base class for a UI element

**************************************************************************************/
class Gadget : public CCCoreUI
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(Gadget);

public:
	virtual void Enable() = 0;
	virtual void Disable() = 0;

	virtual ~Gadget(){}

//	BOOL SetGadgetHelp( CGadgetID Gadget, 
//					    UINT32 BubbleID, 
//					    UINT32 StatusID, 
//					    UINT32 ModuleID = 0);   
//
//	BOOL Hide(BOOL Hide);
//	BOOL Redraw(BOOL Redraw);
//	void PaintNow(CGadgetID gid);
//	void Invalidate();
//	void InvalidatePart(ReDrawInfoType *ExtraInfo, DocRect *InvalidRect);
};

// Message Handling Classes -------------------------------------------------------------------

class Notifier;
/**************************************************************************************

>	class Notifiable

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Allows UIElements to interact with proxies

**************************************************************************************/
class Notifiable
{
public:
	virtual void Notify( Notifier* const pSender ) = 0;
	virtual ~Notifiable(){}
};


/**************************************************************************************

>	class Notifier

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Allows UIElements to interact with proxies

**************************************************************************************/
class Notifier
{
public:
	virtual ~Notifier(){};

	Notifier(): m_ShouldNotify(FALSE) {}

	void NotifyOff()				{ m_ShouldNotify = FALSE;}
	void NotifyOn()					{ m_ShouldNotify = TRUE;}
	
	void Notify(Notifiable* const pObserver)
	{
		if (m_ShouldNotify)
			pObserver->Notify(this);
	}

private:
	BOOL	m_ShouldNotify;

};


/**************************************************************************************

>	static class NotifyNobody: public Notifiable

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Dummy Notifiable object used to initialize references

**************************************************************************************/
static class NotifyNobody : public Notifiable
{
public:
	virtual void Notify(Notifier* const){}
}
Nobody;



/**************************************************************************************

>	class NotifyingGadget : public Gadget

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	This is an abstract class representing a gadget that notifies
				other Notifiable objects. Avoids multiple inheritance.

**************************************************************************************/
class NotifyingGadget : public Gadget
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(NotifyingGadget);

public:
	void Notify(Notifiable* const pObserver)	{	m_Notifier.Notify(pObserver);	}

	void NotifyOff()							{ m_Notifier.NotifyOff();}
	void NotifyOn()								{ m_Notifier.NotifyOn();}

private:
	Notifier	m_Notifier;
};


class DialogOp;
/**************************************************************************************

>	class DialogMgrGadget : public NotifyingGadget

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	This is a base class representing a gadget in a DialogOp. It should not
				be instantiated.

**************************************************************************************/
class DialogMgrGadget : public NotifyingGadget
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(DialogMgrGadget);

public:
	virtual void Enable();
	virtual void Disable();

protected:
	DialogMgrGadget(DialogOp* const pDialog, CGadgetID ControlID) : 
			m_pDialog(pDialog), m_ControlID(ControlID)	{}

protected:
	// Data Members
	DialogOp* const	m_pDialog;
	const CGadgetID	m_ControlID;
};


/*******************************************************************************************

>	void DialogMgrGadget::Enable()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Enables this gadget

*******************************************************************************************/
inline void DialogMgrGadget::Enable()
{
	ASSERT(m_ControlID != 0 && m_pDialog != NULL);
	
	m_pDialog->EnableGadget(m_ControlID, TRUE);
}

/*******************************************************************************************

>	void DialogMgrGadget::Disable()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Disables this gadget

*******************************************************************************************/
inline void DialogMgrGadget::Disable()
{
	ASSERT(m_ControlID != 0 && m_pDialog != NULL);
	
	m_pDialog->EnableGadget(m_ControlID, FALSE);
}



/**************************************************************************************

>	class Button : public DialogMgrGadget

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Represents a button in a DialogOp

**************************************************************************************/
class Button : public DialogMgrGadget
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(Button);

public:
	Button( DialogOp* const pDialog, CGadgetID ControlID,
			Notifiable* const pNotify = &Nobody );

	virtual ~Button();

	virtual void OnClicked();

private:
	Notifiable*	m_pNotifyOnPress;
};


/**************************************************************************************

>	class ListControl : public DialogMgrGadget

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Represents a List in a DialogOp

**************************************************************************************/
class ListControl : public DialogMgrGadget
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(ListControl);

public:
	ListControl(DialogOp* const pDialog, CGadgetID ControlID, 
				Notifiable* pNotify = &Nobody) : 
		DialogMgrGadget(pDialog, ControlID),
		m_pNotifyOnSelectionChange(pNotify) {}

	virtual ~ListControl(){};

	BOOL AddItem(const StringBase& ItemText);
	BOOL InsertItem(const StringBase& ItemText, UINT32 AtIndex);
	BOOL DeleteItemAtIndex(UINT32 AtIndex);
	BOOL DeleteAllItems();

	UINT32 GetNumberOfSelectedItems() const;
	BOOL GetFirstSelectedIndex(INT32* pIndex) const;
	BOOL GetSelectedIndexes(INT32** pIndexes) const;
	BOOL DeselectAll();
	BOOL SetItem(const StringBase& ItemText, UINT32 AtIndex);

	virtual void OnSelect();
	virtual void OnDoubleClicked();

private:
	Notifiable*	m_pNotifyOnSelectionChange;

};


inline BOOL ListControl::AddItem(const StringBase& ItemText)
{
	ENSURE( 0 != m_ControlID, "Control ID is invalide" );
	ENSURE_NOT_NULL(m_pDialog);
	
	return m_pDialog->SetStringGadgetValue(m_ControlID, ItemText, TRUE);
}

inline BOOL ListControl::InsertItem(const StringBase& ItemText, UINT32 AtIndex)
{
	ENSURE( 0 != m_ControlID, "Control ID is invalide" );
	ENSURE_NOT_NULL(m_pDialog);
	
	return m_pDialog->SetStringGadgetValue(m_ControlID, ItemText, FALSE, AtIndex);
}

inline BOOL ListControl::DeleteItemAtIndex(UINT32 AtIndex)
{
	ENSURE( 0 != m_ControlID, "Control ID is invalide" );
	ENSURE_NOT_NULL(m_pDialog);
	return m_pDialog->DeleteValue(m_ControlID, FALSE, INT32(AtIndex));
}

inline BOOL ListControl::DeleteAllItems()
{
	ENSURE( 0 != m_ControlID, "Control ID is invalide" );
	ENSURE_NOT_NULL(m_pDialog);
	return m_pDialog->DeleteAllValues(m_ControlID);
}

inline UINT32 ListControl::GetNumberOfSelectedItems() const
{
	ENSURE( 0 != m_ControlID, "Control ID is invalide" );
	ENSURE_NOT_NULL(m_pDialog);
	return UINT32(m_pDialog->GetSelectedCount(m_ControlID));
}

inline BOOL ListControl::GetFirstSelectedIndex(INT32* pIndex) const
{
	ENSURE( 0 != m_ControlID, "Control ID is invalide" );
	ENSURE_NOT_NULL(m_pDialog);
	*pIndex = m_pDialog->GetFirstSelectedItem(m_ControlID);
	return (*pIndex != -1);
}

inline BOOL ListControl::GetSelectedIndexes(INT32** pIndexes) const
{
	ENSURE( 0 != m_ControlID, "Control ID is invalide" );
	ENSURE_NOT_NULL(m_pDialog);
	ENSURE_NOT_NULL(pIndexes);
	*pIndexes = m_pDialog->GetSelectedItems(m_ControlID);
	return (*pIndexes != NULL);
}


inline BOOL ListControl::DeselectAll()
{
	ENSURE( 0 != m_ControlID, "Control ID is invalide" );
	ENSURE_NOT_NULL(m_pDialog);
	return m_pDialog->SetBoolGadgetSelected(m_ControlID, FALSE, -1);
}


inline void ListControl::OnSelect()
{
	Notify(m_pNotifyOnSelectionChange);
}
inline void ListControl::OnDoubleClicked()
{
	Notify(m_pNotifyOnSelectionChange);
}


/**************************************************************************************

>	class TextControl : private	DialogMgrGadget, public Notifier

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Represents an Edit Box in a DialogOp

**************************************************************************************/
class TextControl : public DialogMgrGadget
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(TextControl);

public:
	TextControl(DialogOp* const pDialog, CGadgetID ControlID,
				const StringBase& InitialText = NullString,
				Notifiable* const pTell = &Nobody );

	virtual ~TextControl(){};

	void UpdateText(const StringBase &NewString);
	void RetrieveText(StringBase* const pExistingString) const;

	virtual void OnChange();

protected:
	const TextControl& operator=(const TextControl& OtherControl);
	TextControl(const TextControl& OtherControl);

private:
	Notifiable*		m_pObserver;

};


/**************************************************************************************

>	class StaticTextControl : public DialogMgrGadget

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Represents a Static Text Control (Label) in a DialogOp

**************************************************************************************/
class StaticTextControl : public DialogMgrGadget
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(StaticTextControl);

public:
	StaticTextControl(DialogOp* const pDialog, CGadgetID ControlID,
				 const StringBase& InitialText = NullString,
				 Notifiable* const pTell = &Nobody );

	virtual ~StaticTextControl(){};

	void UpdateText(const StringBase &NewString);
	void RetrieveText(StringBase* const pExistingString) const;

	virtual void OnChange();

protected:
	const StaticTextControl& operator=(const StaticTextControl& OtherControl);
	StaticTextControl(const StaticTextControl& OtherControl);

private:
	Notifiable*		m_pObserver;

};




/**************************************************************************************

>	class Static : public Gadget

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96
	Purpose:	Base class for a UI element

class StaticText : public DialogMgrGadget
{
	CC_DECLARE_MEMDUMP(StaticText);

public:
	StaticText(	DialogOp* const pDialog, CGadgetID ControlID,
				const StringBase& InitialText = EmptyString);

	void UpdateText(StringID IDStr);
	void UpdateText(const StringBase& Text);
};



class CCComposite : public CCCoreUI
{
	CC_DECLARE_MEMDUMP(CCComposite);

public:
	
	InsertChild();
	DeleteChild();
};



class CCConstraint : public CCComposite
{
	CC_DECLARE_MEMDUMP(CCConstraint);

public:
};




class ComboList : public ListBox
{
	CC_DECLARE_MEMDUMP(ComboList);

public:

	void SetComboListLength();
};





class ValidatedTextControl : public TextControl
{
	CC_DECLARE_MEMDUMP(ValidatedText);

public:
	ValidatedTextControl(	DialogOp* const pDialog, CGadgetID ControlID,
					const StringBase& InitialText = EmptyString);

	void UpdateText(StringID IDStr);
	void UpdateText(const StringBase& Text);

protected:
	virtual BOOL IsValid(const StringBase& Text) = 0;
};

class NumericTextControl : public ValidatedTextControl
{
	CC_DECLARE_MEMDUMP(EditBox);

public:
	NumericTextControl(INT32 Min, INT32 Max);

protected:
	virtual BOOL IsValid(const StringBase& Text);
};



class BoolGadget : public DialogMgrGadget
{
	CC_DECLARE_MEMDUMP(BoolGadget);

public:
	BOOL SetSelected(CGadgetID Gadget, BOOL IsSelected);
	BOOL GetBoolGadgetSelected(CGadgetID GadgetID,
								UINT32 IDSInvalidMsg = 0,
								BOOL* Valid = NULL,
								INT32 ListPos = -1);
};


class RadioButton : public BoolGadget
{
	CC_DECLARE_MEMDUMP(RadioButton);

public:
};




class DualFunctionButton : public Button
{
	CC_DECLARE_MEMDUMP(DualFunctionButton);

public:


};
*/

/*
class Window;	// Forward references to classes in wrappers.h,
class Rect;		// avoids circular #include dependency;

class User_interface
{
public:
	virtual void hide	 ( void							 ) = 0;
	virtual bool display ( Window *win, const Rect &rect ) = 0;
	virtual bool interact( Window *win, const Rect &rect ) = 0;

	virtual ~User_interface( void ){}
};

#endif // USERINTF_H__


//-------------------------------------------------------------------
class Form::Proxy : public User_interface
{
public:
	 virtual ~Proxy();
	 Proxy(	User_interface *attribute_proxy,
			Element		   *notify_me_rather_than_delete_proxy = NULL);

	void release(void);

public: // overrides for User_interface base class
	virtual void hide	 ( void							 );
	virtual bool display ( Window *win, const Rect &rect );
	virtual bool interact( Window *win, const Rect &rect );

protected:	User_interface	*ui;
private:	Form::Element	*creator;

			friend void Form::debug_print(void);
};

//-------------------------------------------------------------------
// Elements can be copied, but there's nothing to copy at this level,
// so leave it up to the derived class. Note that, even though
// give_me_a_proxy_for(...) doesn't modify the class, it is not
// const because the returned proxy could modify the class.

class Form::Element
{
public:
	virtual ~Element(void);

private: friend class Form;
	virtual Proxy *give_me_proxy_for(const String &form_name,
									 const String &attribute_id )=0;
private: friend class Proxy;
	virtual void release_proxy(Proxy *p);

	friend void Form::debug_print(void);
};



BOOL ListControlWorker::PrepareListControl()
{
	DeleteAllValues(m_ControlID);

	INT32 ListIndex = 0;
	const Iterator I = m_pContainer.begin();
	for (const Iterator I = m_pContainer.begin(); I != m_pContainer.end() ++I)
	{
		StringBase& StringToShow = *I->GetStringToShow();
		m_pListControl->AddItem(&StringToShow);
	}

	return TRUE;
}




class UserVisibleString : public String_256, public Notifier
{
public:
	virtual	~String();
	String(Notifiable* pTell = &Nobody );
	String(const String& OtherString , Notifiable* pTell = &Nobody );

private:
	Notifiable*	m_pObserver;

};

*/


#endif	// INC_GADGET
