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
// The dialog that lets Mr Smart User make attributes templatable

#include "camtypes.h"
#include "dialogop.h"
#include "tmpltdlg.h"

//#include "tmpltres.h"
//#include "barsdlgs.h"		// _R(IDD_BARCONTROLSTORE)
#include "xshelpid.h"

#include "app.h"			//for GetApplication()
#include "document.h"
#include "attrmgr.h"

#include "cxfrech.h"

#include "attrval.h"
#include "nodeattr.h"
#include "userattr.h"
#include "tmpltatr.h"


#include "gadget.h"
#include "uielem.h"
#include "visiattr.h"

#include "attraggl.h"

#include "selop.h"
#include "opnode.h"

#include "tmpltarg.h"
#include "styles.h"

#include "namedlg.h"


DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(TemplateDialog, DialogOp);


/**************************************************************************************

>	class VisibleListWithEditableEntries : public VisibleList

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	This rendition of a VisibleList has VisibleListItems that are editable
				The functionality should be in the base class but that TemplateDialog
				parameter ruins things a bit.

**************************************************************************************/
class VisibleListWithEditableEntries : public VisibleList
{
	CC_DECLARE_MEMDUMP(VisibleListWithEditableEntries);

public:
	VisibleListWithEditableEntries() : 
		m_pLastSelectedEntry(NULL) {}

	virtual BOOL DeleteSelectedEntries();
	virtual BOOL DeleteAllEntries();
	virtual void RemoveSelection();
	virtual void OnSelectionChange(TemplateDialog& Dialog);

protected:
	BOOL IsLastSelectedEntry() const							{	return m_pLastSelectedEntry != NULL;	}
	VisibleListItem* GetLastSelectedEntry() const				{	return m_pLastSelectedEntry;	}

	void SetNoLastSelectedEntry()								{	m_pLastSelectedEntry = NULL;	}
	void SetLastSelectedEntry(VisibleListItem& Entry)			{	m_pLastSelectedEntry = &Entry;	}

	virtual void HideLastSelectedEntry();

private:
	VisibleListItem*	m_pLastSelectedEntry;
};





/**************************************************************************************

>	class AvailableProperty : public VisibleListItem

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	This abstract class represent an item in the "Available Properties"
				list.

**************************************************************************************/
class AvailableProperty : public VisibleListItem
{
	CC_DECLARE_MEMDUMP(AvailableProperty);

public:
	virtual AppliedAttribute* CreateUsedProperty() = 0;
	virtual BOOL CanBecomeStyle() const = 0;
	virtual Style* MakeStyle(const StringBase& StyleName)	{	return NULL;	}

private:
};


class VisibleWizOpStyle;
/**************************************************************************************

>	class VisibleTemplateHandler : public AvailableProperty

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Provides the user with something tangible in the way of WizOp's.
				They can appear in a list control via this class.

**************************************************************************************/
class VisibleTemplateHandler : public AvailableProperty
{
	CC_DECLARE_MEMDUMP(VisibleTemplateHandler);

public:
	VisibleTemplateHandler(const WizOp& Handler);
	VisibleTemplateHandler(const VisibleTemplateHandler& Other);
	VisibleTemplateHandler& operator=(const VisibleTemplateHandler& Other);

	~VisibleTemplateHandler();

//	Overridden implementations
	virtual BOOL Display(DialogOp& Dialog);
	virtual BOOL Interact(DialogOp& Dialog);
	virtual void Hide();

	virtual StringBase& GetText(StringBase& Description) const;

	virtual AppliedAttribute* CreateUsedProperty();
	virtual BOOL CanBecomeStyle() const						{	return TRUE;	}
	virtual Style* MakeStyle(const StringBase& StyleName);

protected:
	const WizOp& GetWizOp() const		{	return m_Handler;	}
	const StringBase& GetWorkingQuestion();
	const StringBase& GetWorkingParam();

private:
	const WizOp&				m_Handler;
	VisibleTemplateAttribute*	m_pWorkingVisibleAttr;
};


/**************************************************************************************

>	class VisibleWizOpStyle : public AvailableProperty

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Provides the user with a style to play with.
				They can appear in a list control via this class.

**************************************************************************************/
class VisibleWizOpStyle : public AvailableProperty
{
	CC_DECLARE_MEMDUMP(VisibleWizOpStyle);

public:
	VisibleWizOpStyle(const WizOpStyle& Style);

	virtual BOOL Display(DialogOp& Dialog);
	virtual BOOL Interact(DialogOp& Dialog);
	virtual void Hide();
	virtual StringBase& GetText(StringBase& Description) const		{	return (Description = m_Style.GetName());	}

	virtual AppliedAttribute* CreateUsedProperty();
	virtual BOOL CanBecomeStyle() const						{	return FALSE;	}

private:
	const WizOpStyle&	m_Style;
	EditableText		m_Question;	// actually we can't edit these, but they appear 
	EditableText		m_Param;	// in a box, disabled
	VisibleLabel		m_ParamHint;

};


/**************************************************************************************

>	class CustomProperty : public AvailableProperty

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Provides a "custom" visible template handler so the user can enter
				raw user attributes.

**************************************************************************************/
class CustomProperty : public AvailableProperty
{
	CC_DECLARE_MEMDUMP(CustomProperty);

public:
	CustomProperty() : m_pWorkingVisibleAttr(NULL)	{}

// Overridden functions
	virtual BOOL Display(DialogOp& Dialog);
	virtual BOOL Interact(DialogOp& Dialog);
	virtual void Hide();

	virtual StringBase& GetText(StringBase& Description) const	{	Description.Load(_R(IDS_CUSTOM_WIZOP));return Description;	}
	virtual BOOL operator > (const VisibleListItem& OtherItem) const	{	return TRUE;	}

	virtual AppliedAttribute* CreateUsedProperty();
	virtual BOOL CanBecomeStyle() const						{	return FALSE;	}

private:
	VisibleUserAttribute*	m_pWorkingVisibleAttr;
};


/**************************************************************************************

>	class AvailablePropertiesList : public VisibleListWithEditableEntries

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Provides a visible list of template handlers

**************************************************************************************/
class AvailablePropertiesList : public VisibleListWithEditableEntries
{
	CC_DECLARE_MEMDUMP(AvailablePropertiesList);

public:
	BOOL AddFromTemplateHandler(const WizOp& Handler);
	BOOL AddStyle(const WizOpStyle& NewStyle);

	BOOL AddTemplateHandlers(const WizOps& Handlers);
	BOOL AddStyles(const WizOpStyles& Styles);

private:
};


/**************************************************************************************

>	class UsedPropertiesList : public VisibleListWithEditableEntries

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/96

	Purpose:	Provides a visible list of user attributes

**************************************************************************************/
class UsedPropertiesList : public VisibleListWithEditableEntries
{
	CC_DECLARE_MEMDUMP(UsedPropertiesList);

public:
	UsedPropertiesList() : 
		m_pOriginalEntries(NULL), m_pRemovedEntries(NULL), m_ArraysSize(0) {}

	~UsedPropertiesList();

// Overridden
	virtual BOOL InsertEntry(VisibleListItem& EntryToInsert);
	virtual BOOL DeleteAllEntries();


	BOOL AddAvailableProperty(AvailableProperty& NewProperty);
	BOOL BuildFromEnumerator(RenderableNodeEnumerator& Enumerator);
	BOOL Commit(Operation& OpToApplyWith);
	BOOL DeleteAttributes(String_128 AttrIDs[]);

protected:
// Overridden
	virtual BOOL DeleteEntryAtIndex(UINT32 Index);
//	virtual void HideLastSelectedEntry();

//	virtual BOOL IsDuplicate(const AppliedAttribute& Entry) const;
//	virtual void WarnUser(StringID WarningResourceID) const;
//	virtual void RetractEntryChanges(VisibleListItem& Entry);

// Data Access
	String_128*					GetRemovedEntries()		{	return m_pRemovedEntries;	}
	const AppliedAttribute**	GetOriginalEntries()	{	return m_pOriginalEntries;	}

// Implementation
	BOOL NoteNewEntries();
	CommonAttrMultiSet* FindCommonUserAttributes(RenderableNodeEnumerator& Enumerator);


private:
	const AppliedAttribute**	m_pOriginalEntries;
	String_128*					m_pRemovedEntries;
	UINT32						m_ArraysSize;
};



/*
class Observer : public Notifiable
{
	TemplateDialog*	m_pTheDialog;
public:
	Observer( EditableText* const pContainer ): m_pTheTextObject(pContainer){}
	void Notify( Notifier* const pSender );
};


class DynamicDialog : public DialogOp
{
	CC_DECLARE_DYNCREATE(DynamicDialog);

	//Constructors, destructors and initialiser
public:
	// Metaclass members
	static BOOL		Declare();
	static OpState	GetState(String_256*, OpDescriptor*);

public:
	DynamicDialog();
	virtual ~DynamicDialog();

	// Creating the dialog
public:
	// Overridden 
	virtual void Do(OpDescriptor* pParam);

	PutMeOnYou(const StringBase& ElementName, Element);

	TextControl& GetValueControl() const;
	StaticTextControl& GetParamHintControl() const;

	void SetUserCanModifyQuestion(BOOL TheyAre);

protected:
	// Overridden 
	virtual	MsgResult Message(Msg* Message);

	virtual void OnCreate();
	virtual void OnSelectionChange();

	void OnAvailableSelectionChange();
	void OnUsedSelectionChange();

protected:
	// Implementation
	BOOL CreateGadgets();

protected:
	void ApplyUsedAttributes();

protected:
	// Data Member Access
	AvailablePropertiesList&	GetAvailableProperties();
	UsedPropertiesList&			GetUsedProperties();
	BOOL UserCanModifyQuestion() const;

	
public:
	static const CDlgResID	s_IDD;
	static const CDlgMode	s_Mode;

private:
//	Should generate these dynamically...
	m_Fields;
};

class DialogButton : public UserInterface, public Notifiable
{
public:
	DialogButton(DialogMgrGadget& GadgetToUse);

	virtual BOOL Display();
	virtual BOOL Interact();
	virtual void Hide();
	
	virtual void Notify	(Notifier* pSender);

private:
	Button*	m_pButton;
};


class AddButton : public DialogButton
{
public:
	AddButton();
	virtual ~AddButton();

	virtual BOOL Display(DialogOp& Dialog);
	virtual void Hide();
	
	virtual void Notify(Notifier* pSender);

private:
	Button*	m_pButton;
};


class TemplateDialog::MakeStyleButton : public DialogButton
{
	class StyleCreator : public Notifiable
	{
		MakeStyleButton&	m_Button;
		virtual void Notify( Notifier* pSender );
	public:
		StyleCreator(MakeStyleButton& Button) : m_Button(Button) {}
	}
	m_Creator;
	friend class TemplateDialog::StyleCreator;

	class Disabler : public Notifiable
	{
		MakeStyleButton&	m_Button;
		virtual void Notify( Notifier* pSender );
	public:
		Disabler(MakeStyleButton& Button) : m_Button(Button) {}
	}
	m_Disabler;
	friend class TemplateDialog::StyleCreator;
};

TemplateDialog::MakeStyleButton::Disabler::Notify(Notifier* pSender)
{
}

TemplateDialog::MakeStyleButton::StyleCreator::Notify(Notifier* pSender)
{
	ENSURE_NOT_NULL(pSender);
	ENSURE_KIND(pSender, ButtonNotifier);

	AvailableProperty* const pSelectedProperty = (AvailableProperty*)GetAvailableProperties().GetFirstSelectedEntry();
	if (pSelectedProperty != NULL)
	{
		// Invoke a dialog for the name
		NameDialogParam StyleNameParam;
		if (NameDialog::Invoke(StyleNameParam))		// do we still want it?
		{
			// if so create a new style
			Style* pNewStyle = pSelectedProperty->MakeStyle(StyleNameParam.GetNameEntered());
			if (pNewStyle != NULL)
			{
				Style* pStyleToUse = AddNewStyle(*pNewStyle);
				if (pStyleToUse != NULL)
				{
					GetAvailableProperties().AddStyle(*((WizOpStyle*)pStyleToUse));
					pSelectedProperty->Hide();
				}
			}
			delete pNewStyle;
			pNewStyle = NULL;
		}
	}

	// And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();

}





class PropertyAddedNotifier : public Notifiable, public Notifier
{
public:
	UsedPropertiesChangedNotifier(UsedProperties, AddButton, RemoveButton, ApplyButton, RemoveAllButton)

	virtual void Notify( Notifier* const pSender )
	{
		ButtonNotifier Notifier(ButtonNotifier::DISABLE);
		m_AddButton->Notify(Notifier);
	}
private:
	m_UsedProperties;
	m_AddButton;
	m_RemoveButton;
	m_RemoveAllButton;
	m_ApplyButton;
};

class PropertyRemovedNotifier : public Notifiable
{
public:
	UsedPropertiesChangedNotifier(AddButton, 
	virtual void Notify( Notifier* const pSender ) = 0;
	virtual ~Notifiable(){}
};

*/







CC_IMPLEMENT_MEMDUMP(VisibleListWithEditableEntries, VisibleList);
CC_IMPLEMENT_MEMDUMP(AvailableProperty, VisibleListItem);
CC_IMPLEMENT_MEMDUMP(VisibleTemplateHandler, AvailableProperty);
CC_IMPLEMENT_MEMDUMP(CustomProperty, AvailableProperty);
CC_IMPLEMENT_MEMDUMP(VisibleWizOpStyle, AvailableProperty);
CC_IMPLEMENT_MEMDUMP(AvailablePropertiesList, VisibleListWithEditableEntries);
CC_IMPLEMENT_MEMDUMP(UsedPropertiesList, VisibleListWithEditableEntries);

#define new CAM_DEBUG_NEW
#define ENSURE_THIS()		ENSURE(this != NULL, "this NULL")
#define ENSURE_KIND(pInstance, Class)	ERROR3IF(!pInstance->IS_KIND_OF(Class), #pInstance" is not kind of "#Class);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//	V  I  S  I  B  L  E  L  I  S  T  W  I  T  H  E  D  I  T  A  B  L  E  E  N  T  R  I  E  S
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //



/********************************************************************************************

>	virtual BOOL VisibleListWithEditableEntries::DeleteSelectedEntries()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Overrides the base class so that the last selected entry is set to
				none when 

	See Also:	VisibleList::DeleteSelectedEntries()

********************************************************************************************/
BOOL VisibleListWithEditableEntries::DeleteSelectedEntries()
{
	HideLastSelectedEntry();

	SetNoLastSelectedEntry();

	return VisibleList::DeleteSelectedEntries();
}



/********************************************************************************************

>	virtual BOOL VisibleListWithEditableEntries::DeleteAllEntries()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Overrides the base class so that the last selected entry is set to
				none when 

	See Also:	VisibleList::DeleteAllEntries()

********************************************************************************************/
BOOL VisibleListWithEditableEntries::DeleteAllEntries()
{
	HideLastSelectedEntry();

	SetNoLastSelectedEntry();

	return VisibleList::DeleteAllEntries();
}


/********************************************************************************************

>	virtual void VisibleListWithEditableEntries::RemoveSelection()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Overrides the base class so that when we remove the selection, the entry
				shown is hidden.

	See Also:	VisibleList::DeleteAllEntries()

********************************************************************************************/
void VisibleListWithEditableEntries::RemoveSelection()
{
	HideLastSelectedEntry();

	VisibleList::RemoveSelection();
}


/********************************************************************************************

>	virtual void VisibleListWithEditableEntries::HideLastSelectedEntry()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Support function to hide the last selected entry

********************************************************************************************/
void VisibleListWithEditableEntries::HideLastSelectedEntry()
{
	if (IsLastSelectedEntry())
	{
		VisibleListItem* const pPreviousSelection = GetLastSelectedEntry();
		if (pPreviousSelection != NULL)
		{
			pPreviousSelection->Hide();
		}
	}
}


/********************************************************************************************

>	virtual void VisibleListWithEditableEntries::OnSelectionChange(TemplateDialog& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	When the selected item(s) in this list, call this function.
				Should do a Notify but hides the last selected "EditableEntry" and Interacts
				with the newly selected one.

********************************************************************************************/
void VisibleListWithEditableEntries::OnSelectionChange(TemplateDialog& Dialog)
{
	// if there was an entry already showing its innards get rid of it
	HideLastSelectedEntry();

	// Display the right question, changing the "last" selected entry to the one we're gonna show
	if (GetNumberOfSelectedEntries() == 1)
	{
		// Show the entry's question / param pair
		VisibleListItem* const pSelectedEntry = GetFirstSelectedEntry();
		if (pSelectedEntry != NULL)
		{
			SetLastSelectedEntry(*pSelectedEntry);
			pSelectedEntry->Display(Dialog);
			pSelectedEntry->Interact(Dialog);
		}
		else
		{
			TRACE( _T("VisibleListWithEditableEntries::OnSelectionChange() - Selection count / entries incongruency\n"));
		}
	}
	else
	{
		// if there's more than one or none selected don't confuse the user by showing the 
		// question / param
		SetNoLastSelectedEntry();
	}
}







// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			V  I  S  I  B  L  E  T  E  M  P  L  A  T  E  H  A  N  D  L  E  R
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






/********************************************************************************************

>	VisibleTemplateHandler::VisibleTemplateHandler(const WizOp& Handler)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Constructs us a WizOp that can be on the screen (this is all horribly wrong).

********************************************************************************************/
VisibleTemplateHandler::VisibleTemplateHandler(const WizOp& Handler) : 
	m_Handler(Handler), 
	m_pWorkingVisibleAttr(NULL)
{
}


/********************************************************************************************

>	VisibleTemplateHandler::VisibleTemplateHandler(const VisibleTemplateHandler& Other)
	VisibleTemplateHandler& VisibleTemplateHandler::operator=(const VisibleTemplateHandler& Other)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Warns that I haven't done either of these (but I did do a destructor)

********************************************************************************************/
VisibleTemplateHandler::VisibleTemplateHandler(const VisibleTemplateHandler& Other) : 
	m_Handler(Other.m_Handler), 
	m_pWorkingVisibleAttr(NULL)
{
	TRACE( _T("VisibleTemplateHandler copy constructor not implemented\n"));
}

VisibleTemplateHandler& VisibleTemplateHandler::operator=(const VisibleTemplateHandler& Other)
{
	TRACE( _T("VisibleTemplateHandler assignment operator not implemented\n"));
	return *this;
}


/********************************************************************************************

>	VisibleTemplateHandler::~VisibleTemplateHandler()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Destructor gets rid of any working visible attribute

********************************************************************************************/
VisibleTemplateHandler::~VisibleTemplateHandler()
{
	delete m_pWorkingVisibleAttr;
	m_pWorkingVisibleAttr = NULL;
}


/********************************************************************************************

>	StringBase& VisibleTemplateHandler::GetText(StringBase& Description) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Creates some text suitable for putting in the "Available Properties" list.

	Outputs:	(Change this) Description:	The description of this VisibleTemplateHandler
				that will appear to the user.

	Returns:	A reference to the Text

********************************************************************************************/
StringBase& VisibleTemplateHandler::GetText(StringBase& Description) const
{
	return Description = m_Handler.GetUserName();
}



/********************************************************************************************

>	BOOL VisibleTemplateHandler::Display(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Displays the UI associated with this VisibleTemplateHandler.

	Returns:	Hmmm,,,,

********************************************************************************************/
BOOL VisibleTemplateHandler::Display(DialogOp& Dialog)
{
	ERROR2IF(m_pWorkingVisibleAttr != NULL, FALSE, "VisibleTemplateHandler member should be NULL");

	BOOL Ok = TRUE;

	m_pWorkingVisibleAttr = new VisibleTemplateAttribute(m_Handler);
	Ok = (m_pWorkingVisibleAttr != NULL);

	if (Ok)
	{
		Ok = m_pWorkingVisibleAttr->Display(Dialog);
	}

	return Ok;
}


/********************************************************************************************

>	BOOL VisibleTemplateHandler::Interact(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Displays the UI associated with this VisibleTemplateHandler.
				It creates a new VisibleTemplateAttribute which is binned if the user selects
				another "Available Property" without Adding it (via hide), otherwise it's 
				supplied via CreateUsedProperty() to the Applied Properties list

	Returns:	Hmmm,,,,

********************************************************************************************/
BOOL VisibleTemplateHandler::Interact(DialogOp& Dialog)
{
	BOOL Ok = TRUE;

	if (m_pWorkingVisibleAttr != NULL)
	{
		Ok = m_pWorkingVisibleAttr->Interact(Dialog);
	}

	return Ok;
}


/********************************************************************************************

>	void VisibleTemplateHandler::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Hides whatever was displayed by Display()

	Returns:	Hmmm,,,,

********************************************************************************************/
void VisibleTemplateHandler::Hide()
{
	if (m_pWorkingVisibleAttr != NULL)
	{
		m_pWorkingVisibleAttr->Hide();
		delete m_pWorkingVisibleAttr;
		m_pWorkingVisibleAttr = NULL;
	}
}

const StringBase& VisibleTemplateHandler::GetWorkingQuestion()
{
	STRING_ENSURE_NOT_NULL(m_pWorkingVisibleAttr)

	return m_pWorkingVisibleAttr->GetValue();
}


const StringBase& VisibleTemplateHandler::GetWorkingParam()
{
	STRING_ENSURE_NOT_NULL(m_pWorkingVisibleAttr)

	return m_pWorkingVisibleAttr->GetLongKey();
}



/********************************************************************************************

>	AppliedAttribute* VisibleTemplateHandler::CreateUsedProperty()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Creates an item suitable for adding to the "Used Properties" list.

	Returns:	A new AppliedAttribute or derived class which is what Used Properties are.
				(Although this new one's not applied yet)

********************************************************************************************/
AppliedAttribute* VisibleTemplateHandler::CreateUsedProperty()
{
	AppliedAttribute* pNewAppliedAttr = NULL;

	if (m_pWorkingVisibleAttr != NULL)
	{
		m_pWorkingVisibleAttr->Hide();
		m_pWorkingVisibleAttr->MarkAsUsed();

		pNewAppliedAttr = new SingletonAppliedAttribute(*m_pWorkingVisibleAttr);

		if (pNewAppliedAttr != NULL)
		{
			// make sure Hide() doesn't throw the not-so-working VisibleAttribute away
			m_pWorkingVisibleAttr = NULL;
		}
	}

	return pNewAppliedAttr;
}



Style* VisibleTemplateHandler::MakeStyle(const StringBase& StyleName)
{
	ERROR2IF(GetWorkingQuestion().IsEmpty(), NULL, "Empty Question");

	return new WizOpStyle(StyleName, GetWorkingQuestion(), GetWizOp(), GetWorkingParam());
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//						C  U  S  T  O  M  P  R  O  P  E  R  T  Y
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





BOOL CustomProperty::Display(DialogOp& Dialog)
{
	ERROR2IF(m_pWorkingVisibleAttr != NULL, FALSE, "CustomProperty member should be NULL");

	BOOL Ok = TRUE;

	m_pWorkingVisibleAttr = new VisibleUserAttribute(NullString, NullString);
	Ok = (m_pWorkingVisibleAttr != NULL);

	if (Ok)
	{
		Ok = m_pWorkingVisibleAttr->Display(Dialog);
	}

	return Ok;
}

BOOL CustomProperty::Interact(DialogOp& Dialog)
{
	BOOL Ok = TRUE;

	if (m_pWorkingVisibleAttr != NULL)
	{
		Ok = m_pWorkingVisibleAttr->Interact(Dialog);
	}

	return Ok;
}


void CustomProperty::Hide()
{
	if (m_pWorkingVisibleAttr != NULL)
	{
		m_pWorkingVisibleAttr->Hide();
		delete m_pWorkingVisibleAttr;
		m_pWorkingVisibleAttr = NULL;
	}
}


AppliedAttribute* CustomProperty::CreateUsedProperty()
{
	AppliedAttribute* pNewAppliedAttr = NULL;

	if (m_pWorkingVisibleAttr != NULL)
	{
		m_pWorkingVisibleAttr->Hide();
		m_pWorkingVisibleAttr->MarkAsUsed();

		pNewAppliedAttr = new SingletonAppliedAttribute(*m_pWorkingVisibleAttr);

		if (pNewAppliedAttr != NULL)
		{
			// make sure Hide() doesn't throw the not-so-working VisibleAttribute away
			m_pWorkingVisibleAttr = NULL;
		}
	}

	return pNewAppliedAttr;
}








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//				V  I  S  I  B  L  E  W  I  Z  O  P  S  T  Y  L  E 
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //







VisibleWizOpStyle::VisibleWizOpStyle(const WizOpStyle& Style) :
	m_Style(Style)
{
	m_ParamHint	= Style.GetWizOp().GetParamHint();
	m_Param		= Style.GetParam();
	m_Question	= Style.GetQuestion();
}


BOOL VisibleWizOpStyle::Display(DialogOp& Dialog)
{
	BOOL Ok = TRUE;

	Ok = m_Param.Display(Dialog);

	if (Ok)
	{
		Ok = m_Question.Display(Dialog);
	}

	if (Ok)
	{
		Ok = m_ParamHint.Display(Dialog);
	}

	return Ok;
}

BOOL VisibleWizOpStyle::Interact(DialogOp& Dialog)
{
	ERROR3IF(!((&Dialog)->IS_KIND_OF(TemplateDialog)), "Dialog isn't kind of TemplateDialog");

	TemplateDialog& BetterBeThisDialog = (TemplateDialog&)Dialog;

	BOOL Ok = TRUE;

	Ok = m_Param.Display(BetterBeThisDialog.GetKeyControl());
	if (Ok)
	{
		Ok = m_Question.Display(BetterBeThisDialog.GetValueControl());
	}
	if (Ok)
	{
		Ok = m_ParamHint.Display(Dialog);
	}

	return Ok;
}


void VisibleWizOpStyle::Hide()
{
	m_Param.Hide();
	m_Question.Hide();
	m_ParamHint.Hide();
}

AppliedAttribute* VisibleWizOpStyle::CreateUsedProperty()
{
	StyleReferenceAttribute NewReference(m_Style);

	AppliedAttribute* pNewVisibleAttr = new SingletonAppliedAttribute(NewReference);

	return pNewVisibleAttr;
}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			A  V  A  I  L  A  B  L  E  P  R  O  P  E  R  T  I  E  S  L  I  S  T
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //




/********************************************************************************************

>	BOOL AvailablePropertiesList::AddFromTemplateHandler(const WizOp& Handler)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Adds a template handler to the list (which is shown in the dialog as
				"Available Properties"

********************************************************************************************/
BOOL AvailablePropertiesList::AddFromTemplateHandler(const WizOp& Handler)
{
	BOOL Success = TRUE;

	VisibleTemplateHandler* pNewEntry = new VisibleTemplateHandler(Handler);
	Success = (pNewEntry != NULL);

	if (Success)
	{
		Success = InsertEntry(*pNewEntry);
	}

	if (!Success)
	{
		delete pNewEntry;
		pNewEntry = NULL;
	}

	return Success;
}


/********************************************************************************************

>	BOOL AvailablePropertiesList::AddTemplateHandlers(const WizOps& Handlers)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Adds a template handler to the list (which is shown in the dialog as
				"Available Properties"

********************************************************************************************/
BOOL AvailablePropertiesList::AddTemplateHandlers(const WizOps& Handlers)
{
	BOOL Success = TRUE;

	WizOp* pHandler = Handlers.GetFirstWizOp();
	while (pHandler != NULL && Success)
	{
		Success = AddFromTemplateHandler(*pHandler);

		pHandler = Handlers.GetNextWizOp(pHandler);
	}

	return Success;
}


/********************************************************************************************

>	BOOL AvailablePropertiesList::AddStyle(const WizOpStyle& NewStyle)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Adds a WizOpStyle to this list (which is shown in the dialog as
				"Available Properties"

********************************************************************************************/
BOOL AvailablePropertiesList::AddStyle(const WizOpStyle& NewStyle)
{
	BOOL Ok = TRUE;

	VisibleWizOpStyle* pNewEntry = new VisibleWizOpStyle(NewStyle);
	Ok = (pNewEntry != NULL);

	if (Ok)
	{
		Ok = InsertEntry(*pNewEntry);
	}

	if (!Ok)
	{
		delete pNewEntry;
		pNewEntry = NULL;
	}

	return Ok;
}


/********************************************************************************************

>	BOOL AvailablePropertiesList::AddStyles(const WizOpStyles& Styles)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Adds the given WizOpStyles to this list.
				Hence, we can build our list of "Available Properties" to include the 
				WizOpStyle's in the selected document.

********************************************************************************************/
BOOL AvailablePropertiesList::AddStyles(const WizOpStyles& Styles)
{
	BOOL Ok = TRUE;

	StyleIterator Iterator = Styles.Begin();
	while (Iterator != Styles.End() && Ok)
	{
		Ok = AddStyle((WizOpStyle&)*Iterator);

		++Iterator;
	}

	return Ok;
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//						U  S  E  D  P  R  O  P  E  R  T  I  E  S  L  I  S  T
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //




/********************************************************************************************

>	UsedPropertiesList::~UsedPropertiesList()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Makes sure the embedded objects are deleted.

********************************************************************************************/
UsedPropertiesList::~UsedPropertiesList()
{
	delete [] m_pOriginalEntries;
	m_pOriginalEntries = NULL;

	delete [] m_pRemovedEntries;
	m_pRemovedEntries = NULL;
}


/********************************************************************************************

>	virtual BOOL UsedPropertiesList::InsertEntry(VisibleListItem& EntryToInsert)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Overrides the base class function to make sure the entries are unique.

	Notes:		See base class for interface details.

********************************************************************************************/
BOOL UsedPropertiesList::InsertEntry(VisibleListItem& EntryToInsert)
{
	ENSURE_KIND((&EntryToInsert), AppliedAttribute);

	BOOL Ok = TRUE;

	AppliedAttribute& AppliedAttributeToInsert = (AppliedAttribute&)EntryToInsert;

	// We don't want the user to think they can apply WizOps of the same class so
	// go through the VisibleList looking for one with a duplicate AttributeIdentifier
	VisibleListIterator Iterator = Begin();
	while (Iterator != End() && Ok)
	{
		AppliedAttribute& UsedProperty = (AppliedAttribute&)*Iterator;
		if (UsedProperty.GetAttrID() == AppliedAttributeToInsert.GetAttrID())
		{
			Ok = FALSE;
		}
		else
		{
			++Iterator;
		}
	}

	if (Ok)
	{
		Ok = VisibleListWithEditableEntries::InsertEntry(EntryToInsert);
	}

	return Ok;
}


/********************************************************************************************

>	BOOL UsedPropertiesList::DeleteEntryAtIndex(UINT32 Index)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Overrides the base class function to allow this UsePropertiesList to note 
				which items have been deleted.

	Notes:		See base class for interface details.
				This is a bit yuck (no class for original entries, etc.)

********************************************************************************************/
BOOL UsedPropertiesList::DeleteEntryAtIndex(UINT32 Index)
{
	BOOL Ok = TRUE;

	AppliedAttribute& Entry = (AppliedAttribute&)GetEntryAt(Index);

	UINT32 ArrayIndex = 0;
	BOOL Found = FALSE;
	while (ArrayIndex < m_ArraysSize && !Found)
	{
		if (m_pOriginalEntries[ArrayIndex] == &Entry)
		{
			Found = TRUE;
		}
		else
		{
			++ArrayIndex;
		}
	}

	if (Found)
	{
		m_pOriginalEntries[ArrayIndex] = NULL;
	}

	// Find the last entry in the removed array
	ArrayIndex = 0;
	Found = FALSE;
	while (ArrayIndex < m_ArraysSize && !Found)
	{
		if (m_pRemovedEntries[ArrayIndex].IsEmpty())
		{
			Found = TRUE;
		}
		else
		{
			++ArrayIndex;
		}
	}

	if (Found)
	{
		AttributeIdentifier AttrID = Entry.GetAttrID();

		m_pRemovedEntries[ArrayIndex] = AttrID;
	}


	Ok = VisibleListWithEditableEntries::DeleteEntryAtIndex(Index);

	return Ok;
}


/********************************************************************************************

>	BOOL UsedPropertiesList::DeleteAllEntries()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Overrides the base class function to allow this UsePropertiesList to note 
				which items have been deleted.

	Notes:		See base class for interface details.
				This is a bit yuck (no class for original entries, etc.)

********************************************************************************************/
BOOL UsedPropertiesList::DeleteAllEntries()
{
	BOOL Ok = TRUE;

	// Find the last entry in the removed array
	UINT32 RemovedEntriesIndex = 0;
	BOOL Found = FALSE;

	while (RemovedEntriesIndex < m_ArraysSize && !Found)
	{
		if (m_pRemovedEntries[RemovedEntriesIndex].IsEmpty())
		{
			Found = TRUE;
		}
		else
		{
			++RemovedEntriesIndex;
		}
	}

	if (Found)
	{
		// Look through the original entries array to find any that haven't been removed
		UINT32 OriginalEntriesIndex = 0;
		while (OriginalEntriesIndex < m_ArraysSize)
		{
			if (m_pOriginalEntries[OriginalEntriesIndex] != NULL)
			{
				AttributeIdentifier AttrID = m_pOriginalEntries[OriginalEntriesIndex]->GetAttrID();
				
				m_pRemovedEntries[RemovedEntriesIndex] = AttrID;
				m_pOriginalEntries[OriginalEntriesIndex] = NULL;

				++RemovedEntriesIndex;
			}
			++OriginalEntriesIndex;
		}
	}


	Ok = VisibleListWithEditableEntries::DeleteAllEntries();

	return Ok;
}


/********************************************************************************************

>	virtual void UsedPropertiesList::HideLastSelectedEntry()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Overrides the base class so the UsedPropertiesList can make sure the entry
				hasn't changed into another duplicate entry.

********************************************************************************************/
/*
void UsedPropertiesList::HideLastSelectedEntry()
{
	VisibleListItem* const pEntryShowing = GetLastSelectedEntry();

	if (pEntryShowing != NULL)
	{
		if (IsDuplicate(*pEntryShowing))
		{
			WarnUser(_R(IDS_ERROR_DUPLICATE_PARAM));

			RetractEntryChanges(*pEntryShowing);
		}
		//	If the current entry has changed
	}

	VisibleListWithEditableEntries::HideLastSelectedEntry();
}
*/

/********************************************************************************************

>	virtual BOOL UsedPropertiesList::IsDuplicate(const VisibleListItem& Entry) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Adds an available property to this list (which is shown in the dialog as
				"Used Properties"

********************************************************************************************/
/*
BOOL UsedPropertiesList::IsDuplicate(const VisibleListItem& Entry) const
{
	BOOL IsDuplicate = FALSE;

	return IsDuplicate;
}
*/

/********************************************************************************************

>	virtual void UsedPropertiesList::WarnUser(StringID WarningResourceID) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Tells the user that what they've done is a no-no.

	Inputs:		WarningResourceID	The resource id of the warning string that will appear
									in the message to the user.

********************************************************************************************/
/*
void UsedPropertiesList::WarnUser(StringID WarningResourceID) const
{
	Error::InformWarning(WarningResourceID);
}
*/

/********************************************************************************************

>	virtual void UsedPropertiesList::RetractEntryChanges(VisibleListItem& Entry)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	When a user has changed a Used Property to something they shouldn't have
				this member is called to undo the changes.


********************************************************************************************/
/*
void UsedPropertiesList::RetractEntryChanges(VisibleListItem& Entry)
{
	
}
*/


/********************************************************************************************

>	BOOL UsedPropertiesList::AddAvailableProperty(AvailableProperty& NewProperty)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Adds an available property to this list (which is shown in the dialog as
				"Used Properties"

********************************************************************************************/
BOOL UsedPropertiesList::AddAvailableProperty(AvailableProperty& NewProperty)
{
	BOOL Success = TRUE;

	// Create a new AppliedAttribute
	AppliedAttribute* pNewListEntry = NewProperty.CreateUsedProperty();
	Success = (pNewListEntry != NULL);

	if (Success)
	{
		Success = InsertEntry(*pNewListEntry);
	}

	if (!Success)
	{
		if (pNewListEntry != NULL)
		{
			delete pNewListEntry;
			pNewListEntry = NULL;
		}
	}

	return Success;
}


/********************************************************************************************

>	BOOL UsedPropertiesList::BuildFromEnumerator(RenderableNodeEnumerator& Enumerator)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Builds this UsedPropertiesList by finding the attributes applied to the
				objects found in the given iterator (the selection or a node).

	Inputs:		Enumerator:	This should return the collection of nodes for which this list
							represents the applied attributes.

********************************************************************************************/
BOOL UsedPropertiesList::BuildFromEnumerator(RenderableNodeEnumerator& Enumerator)
{
	BOOL Ok = TRUE;

	// Build a set of applied attributes, deleting any old list before we do

	CommonAttrMultiSet* pAttrSet = FindCommonUserAttributes(Enumerator);
	Ok = (pAttrSet != NULL);

	// For each applied attribute create a visible version and add it to this list in reverse
	// order so they come out in the same order in which they're applied. (Each is added as 
	// the first entry)
	MultiCommonAttrItem* pCommonItem = NULL;
	if (Ok)
	{
		pCommonItem = pAttrSet->FindFirst();
	}

	while (pCommonItem != NULL && Ok)
	{
		AppliedAttribute* const pNewVisibleAppliedAttr = pCommonItem->CreateVisibleAppliedAttribute();
		Ok = (pNewVisibleAppliedAttr != NULL);
		if (Ok)
		{
			Ok = InsertEntry(*pNewVisibleAppliedAttr);
		}

		pCommonItem = pAttrSet->FindNext(pCommonItem);
	}

	delete pAttrSet;

	if (Ok)
	{
		Ok = NoteNewEntries();
	}

	return Ok;
}


/********************************************************************************************

>	BOOL UsedPropertiesList::NoteNewEntries()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Support function which takes all the entries in this list and marks them
				as original entries. When it comes to commiting the entries, i.e., 
				applying them it then knows which ones to delete from the selection.

	Returns:	TRUE if it worked, FALSE if it didn't

********************************************************************************************/
BOOL UsedPropertiesList::NoteNewEntries()
{
	BOOL Ok = TRUE;

	delete [] m_pOriginalEntries;
	m_pOriginalEntries = NULL;

	delete [] m_pRemovedEntries;
	m_pRemovedEntries = NULL;

	m_ArraysSize = GetContainer().GetCount();

	if (m_ArraysSize != 0)
	{
		m_pOriginalEntries	= new const AppliedAttribute*[m_ArraysSize];
		m_pRemovedEntries	= new String_128[m_ArraysSize];
	}

	Ok = (m_pOriginalEntries != NULL && m_pRemovedEntries != NULL);

	if (Ok)
	{
		UINT32 Index = 0;
		VisibleListIterator Iterator = Begin();
		while (Iterator != End())
		{
			AppliedAttribute& ListEntry = (AppliedAttribute&)*Iterator;

			m_pOriginalEntries[Index]	= &ListEntry;
			m_pRemovedEntries[Index]	= NullString;

			++Iterator;
			++Index;
		}
	}

	return Ok;
}


/********************************************************************************************

>	CommonAttrMultiSet* UsedPropertiesList::FindCommonUserAttributes(
												RenderableNodeEnumerator& Enumerator)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Support function to create a CommonAttrMultiSet using the given Enumerator.

********************************************************************************************/
CommonAttrMultiSet* UsedPropertiesList::FindCommonUserAttributes(RenderableNodeEnumerator& Enumerator)
{
	CommonAttrMultiSet* pCommonAttrs = new CommonAttrMultiSet();	// return this

	if (pCommonAttrs != NULL)
	{
		UserAttributeAgglomerator AttrAgglr;
		AttrAgglr.FindCommonAttributes(Enumerator, pCommonAttrs);
	}

	return pCommonAttrs;
}


/********************************************************************************************

>	BOOL UsedPropertiesList::Commit(Operation& OpToApplyWith)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Applies all the attributes in this UsedPropertiesList list to the
				node.

********************************************************************************************/
BOOL UsedPropertiesList::Commit(Operation& OpToApplyWith)
{
	BOOL Ok = TRUE;

	// Remove any attributes that were removed
	if (GetRemovedEntries() != NULL)
	{
		Ok = DeleteAttributes(GetRemovedEntries());
	}

	// and apply the new attributes in reverse order, because the OpApply...sticks them in 
	// nearest the Node
	VisibleListIterator Iterator = BeginOfEnd();
	while (Iterator != End() && Ok)
	{
		AppliedAttribute& Entry = (AppliedAttribute&)(*Iterator);
		Ok = Entry.ApplyAttribute(OpToApplyWith);
		--Iterator;
	}

	return Ok;
}



/********************************************************************************************

>	BOOL UsedPropertiesList::DeleteAttributes(const StringBase* AttrIDs[])

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Support function to delete the given AttributeIdentifiers from the objects
				in the selection.

	Notes:		This isn't quite true 'cos it looks at the data members for size info
				(The array should be a class). We won't talk about the StringBase being
				an AttributeIdentifier.

********************************************************************************************/
BOOL UsedPropertiesList::DeleteAttributes(String_128 AttrIDs[])
{
	BOOL Ok = TRUE;

	// Obtain a pointer to the op descriptor for the attribute operation 
	OpDescriptor* const pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpRemoveAttributeFromSelection));
	ENSURE_NOT_NULL(pOpDesc);

	UINT32 Index = 0;
	while (Index < m_ArraysSize && !AttrIDs[Index].IsEmpty() && Ok)
	{
		AttributeIdentifier AttrID = (AttributeIdentifier)AttrIDs[Index];
		// Invoke the operation, passing AttrID as a parameter
		pOpDesc->Invoke(&(OpParam&)RemoveAttributeParam(AttrID), TRUE);	// do want undo

		// we've deleted that one so mark it as gone
		AttrIDs[Index] = NullString;

		++Index;
	}

	return Ok;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//						T  E  M  P  L  A  T  E  D  I  A  L  O  G
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






// All Temmplate Dialogs are created equal
const CDlgMode TemplateDialog::s_Mode	= MODELESS;
const CDlgResID TemplateDialog::s_IDD	= _R(IDD_TEMPLATE_DIALOG);

// Preference Strings
String_32 TemplateDialog::s_PreferenceSection		= TEXT("Wizard Properties");
const UINT32 TemplateDialog::s_NumberOfPreferences	= 1;	// ...in the section

/********************************************************************************************

	Preference:		Show Custom
	Section:		Wizard Properties
	Range:			TRUE or FALSE
	Purpose:		If TRUE, the Wizard Properties dialog will show an Available Property
					"Custom" through which the user can add 

********************************************************************************************/
String_16 TemplateDialog::s_ShowCustom			= TEXT("Show Custom");
BOOL TemplateDialog::s_ShowCustomProperty		= FALSE;


/*******************************************************************************************

>	static BOOL TemplateDialog::Init()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Creates an OpDescriptor for a TemplateDialog so you can make a template
				dialog pop up using pOpDescriptor->Invoke(). Call this before even thinking
				of invoking it 'cos otherwise it won't work. Somewhere in main3.cpp would be
				nice.

	Returns:	FALSE if it fails

*******************************************************************************************/
BOOL TemplateDialog::Init()
{
	BOOL Ok = TRUE;

	Ok = OpRemoveAttributeFromSelection::Declare();


	Ok |= RegisterOpDescriptor(
				0,									// Tool ID
 				_R(IDS_TEMPLATE_DIALOG),	 			// String resouirce ID
				CC_RUNTIME_CLASS(TemplateDialog),	// Runtime class
 				OPTOKEN_TEMPLATEDLG,				// Token string
 				TemplateDialog::GetState,			// GetState function
				0, // is now helpless _R(IDH_Command_Wizard_Properties),		// Help ID
				_R(IDBBL_TEMPLATE_DIALOG),				// Bubble ID
				_R(IDD_BARCONTROLSTORE),				// Resource ID
				_R(IDC_TMPLTDLG),						// Control ID
				SYSTEMBAR_UTILITIES,				// Bar ID
				TRUE,								// Recieve system messages (and button msgs)
				FALSE,								// Smart duplicate operation
				TRUE,								// Clean operation
				NULL,								// No vertical counterpart
				NULL,								// String to say only one instance allowed
				(	DONT_GREY_WHEN_SELECT_INSIDE | 
					GREY_WHEN_NO_CURRENT_DOC)
			);	
	
	Ok |= NameDialog::Init();

	Ok |= Camelot.DeclareSection(s_PreferenceSection, s_NumberOfPreferences);
	Ok |= Camelot.DeclarePref(s_PreferenceSection, s_ShowCustom, &s_ShowCustomProperty );

	return Ok;					
}   
 
      
static TemplateDialog* g_TheOneTemplateDialogInTheEntireUniverse = NULL;

/*******************************************************************************************

>	static OpState	TemplateDialog::GetState(String_256*, OpDescriptor*)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Silliness with statics (derived classes anyone?)

*******************************************************************************************/
OpState	TemplateDialog::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	if (g_TheOneTemplateDialogInTheEntireUniverse != NULL)
	{
		OpSt.Ticked = TRUE;
	}

	return OpSt;
}

		 
/*******************************************************************************************

>	static BOOL TemplateDialog::ShowCustomProperty()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Tells us if we should add a custom property to the Available Properties 
				list according to preference.

*******************************************************************************************/
BOOL TemplateDialog::ShowCustomProperty()
{
	return s_ShowCustomProperty;
}


/*******************************************************************************************

>	TemplateDialog::TemplateDialog() : DialogOp(TemplateDialog::IDD, TemplateDialog::Mode) 

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Required constructor for DialogManager, gives Modeless dialog and resource
				ID.

*******************************************************************************************/
TemplateDialog::TemplateDialog() : DialogOp(TemplateDialog::s_IDD, TemplateDialog::s_Mode) 
{
	m_pAvailableProperties	= NULL;
	m_pUsedProperties		= NULL;

	m_pUsedListGadget		= NULL;
	m_pAvailableListGadget	= NULL;
	m_pQuestionGadget		= NULL;
	m_pParamHintGadget		= NULL;
	m_pParamGadget			= NULL;
	m_pAddButton			= NULL;
	m_pRemoveButton			= NULL;
	m_pRemoveAllButton		= NULL;
	m_pApplyButton			= NULL;
	m_pMakeStyleButton		= NULL;

	m_UserCanModifyQuestion	= FALSE;
}       



/*******************************************************************************************

>	TemplateDialog::~TemplateDialog()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Destructor deletes those member variables

*******************************************************************************************/
TemplateDialog::~TemplateDialog()
{
	delete m_pAvailableProperties;
	m_pAvailableProperties = NULL;

	delete m_pUsedProperties;
	m_pUsedProperties = NULL;


	delete m_pUsedListGadget;
	m_pUsedListGadget		= NULL;

	delete m_pAvailableListGadget;
	m_pAvailableListGadget	= NULL;

	delete m_pQuestionGadget;
	m_pQuestionGadget		= NULL;

	delete m_pParamHintGadget;
	m_pParamHintGadget		= NULL;

	delete m_pParamGadget;
	m_pParamGadget			= NULL;

	delete m_pAddButton;
	m_pAddButton			= NULL;

	delete m_pRemoveButton;
	m_pRemoveButton			= NULL;

	delete m_pRemoveAllButton;
	m_pRemoveAllButton		= NULL;

	delete m_pApplyButton;
	m_pApplyButton			= NULL;

	delete m_pMakeStyleButton;
	m_pMakeStyleButton		= NULL;
}       



/********************************************************************************************

>	void TemplateDialog::Do(OpDescriptor*)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Waste some space
		
********************************************************************************************/
void TemplateDialog::Do(OpDescriptor*)
{
	// We're called when the menu item / button is clicked: once to open the dialog, once
	// to close it. This is pseudo model isn't it and rather silly.
	if (g_TheOneTemplateDialogInTheEntireUniverse == NULL)
	{
		// First time so stick the dialog on the screen
		if (Create())
		{
			g_TheOneTemplateDialogInTheEntireUniverse = this;

			Open();
		}
		else
		{
			End();
		}
	}
	else
	{
		// Clicked button / menu to close the dialog
		g_TheOneTemplateDialogInTheEntireUniverse->Close();
		g_TheOneTemplateDialogInTheEntireUniverse->End();

		End();		// throw away the TemplateDialog that was created for this Do()

		g_TheOneTemplateDialogInTheEntireUniverse = NULL;
	}
}


/********************************************************************************************

>	MsgResult TemplateDialog::Message( Msg* pMessage)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Inputs:		pMessage: The message that DialogManager may or may not send us
		
	Returns:	DialogOp::MsgResult

	Purpose:	Bog standard dispatch the message type switch statement.
	 
********************************************************************************************/
MsgResult TemplateDialog::Message( Msg* pMessage)
{
	if (IS_OUR_DIALOG_MSG(pMessage))
	{
		DialogMsg* const pDialogMsg = ((DialogMsg*) pMessage); 
	
		switch (pDialogMsg->DlgMsg)
		{
			case DIM_TEXT_CHANGED:
				// just respond to changes to the question
				// should use Notify really
				switch (pDialogMsg->GadgetID)
				{
					case _R(IDC_TMLPTDLG_QUESTION):
						// Make sure we don't respond to the changes we make internally
						OnTextChange();
						break;

					case _R(IDC_TMPLTDLG_PARAM):
						OnParamChange();
						break;
				}

				break;

			case DIM_SELECTION_CHANGED_COMMIT:	// who named this???
				if (pDialogMsg->GadgetID == _R(IDC_TMLPTDLG_AVAILABLE))
				{
					OnAvailableListDoubleClicked();
				}
				break;
				
			case DIM_LFT_BN_CLICKED:
				switch(pDialogMsg->GadgetID)
				{
					case _R(IDC_TMPLTDLG_ADD):
						OnAddButtonClicked();					
						break;

					case _R(IDC_TMPLTDLG_REMOVE):
						OnRemoveButtonClicked();
						break;

					case _R(IDC_TMPLTDLG_REMOVE_ALL):
						OnRemoveAllButtonClicked();
						break;

					case _R(IDC_TMPLTDLG_MAKE_STYLE):
						OnMakeStyleClicked();
						break;
				}
				break;

			case DIM_SELECTION_CHANGED: 
				// NB: This means Messrs Mouse & Keyboard have manipulated one of the list boxes.
				// It doesn't mean the selection has changed!
				if (pDialogMsg->GadgetID == _R(IDC_TMLPTDLG_AVAILABLE))
				{
					OnAvailableListHighlightChanged();
				}
				else if (pDialogMsg->GadgetID == _R(IDC_TMLPTDLG_USED))
				{
					OnUsedListHighlightChanged();
				}
				break;

			case DIM_CREATE:
				OnCreate();
				break;

			case DIM_COMMIT:
				OnApplyClicked();
				break;

			case DIM_CANCEL:	
				Close(); 		   
				End();
				g_TheOneTemplateDialogInTheEntireUniverse = NULL;
				break;
		}
	}
	else if (MESSAGE_IS_A(pMessage, SelChangingMsg))
	{
		if ( ((SelChangingMsg*)pMessage)->State == SelChangingMsg::SELECTIONCHANGED )
			OnSelectionChange();
	}
	else if (MESSAGE_IS_A(pMessage, DocChangingMsg))
	{
		// Look out for the selected document changing
		DocChangingMsg::DocState State = ((DocChangingMsg*)pMessage)->State;
		if (State == DocChangingMsg::SELCHANGED)
		{
			SetDialogInitialState(((DocChangingMsg*)pMessage)->pNewDoc);
		}
	}
	else if (MESSAGE_IS_A(pMessage, StylesMessage))
	{
		// We're sent one of these when the styles are added to (not by us)

		// Make sure they're the styles we've got displayed that are changing
		if (&(((StylesMessage*)pMessage)->GetStylesChanging()) == GetStyles())
		{
			SetDialogInitialState(Document::GetSelected());
		}
	}
		
	return DialogOp::Message(pMessage);
} 


/********************************************************************************************

>	void TemplateDialog::OnCreate()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Handles a message that the dialog has just been created.

********************************************************************************************/
void TemplateDialog::OnCreate()
{
	CreateGadgets();

	// Set up the dialog in its initial state
	SetDialogInitialState(Document::GetSelected());

	// And put the keyboard focus in the available properties list
	SetKeyboardFocus(_R(IDC_TMLPTDLG_AVAILABLE));
}


/********************************************************************************************

>	void TemplateDialog::OnTextChange()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Responds to the user's request to add the selected template argument to the
				list of used properties.

********************************************************************************************/
void TemplateDialog::OnTextChange()
{
	// if we're modifying a question, that means it's a Used Property, so
	// allow them to Apply the change
	if (UserCanModifyQuestion())
	{
		// bit bodgy this...
		m_pApplyButton->Enable();
		SetUserCanModifyQuestion(FALSE);	// well they can but who cares now?
	}
	else
	{
		// If we've got one Available Property selected we might want to
		// allow them to make a style of it

		BOOL ShouldEnableMakeStyle = FALSE;

		if (GetAvailableProperties().GetNumberOfSelectedEntries() == 1)
		{
			String_256 Question;
			VOID_ENSURE_NOT_NULL(m_pQuestionGadget);
			if (m_pQuestionGadget != NULL)
			{
				m_pQuestionGadget->RetrieveText(&Question);
			}

			AvailableProperty* pSelectedEntry = (AvailableProperty*)GetAvailableProperties().GetFirstSelectedEntry();
			if (pSelectedEntry != NULL)
			{
				ShouldEnableMakeStyle = pSelectedEntry->CanBecomeStyle() && !Question.IsEmpty();
			}
		}

		if (ShouldEnableMakeStyle)
		{
			m_pMakeStyleButton->Enable();
		}
		else
		{
			m_pMakeStyleButton->Disable();
		}
	}

}


/********************************************************************************************

>	void TemplateDialog::OnParamChange()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Responds to the user's request to add the selected template argument to the
				list of used properties.

	Notes:		The order of events should go:
					m_pParamGadget->OnChange()
					Notify(this)
					this->OnParamChange()
				but we'll bodge it a bit & go directly here 'cos DialogOp's a bit of a mess
				at the moment.

********************************************************************************************/
void TemplateDialog::OnParamChange()
{
/*
	if (UserCanModifyQuestion())
	{
		// bit bodgy this...
		m_pApplyButton->Enable();
		SetUserCanModifyQuestion(FALSE);	// well they can but who cares now?
	}
*/
}


/********************************************************************************************

>	void TemplateDialog::OnAvailableDoubleClicked()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Responds to the user's request to add the selected template argument to the
				list of used properties.

********************************************************************************************/
void TemplateDialog::OnAvailableListDoubleClicked()
{
	MakePropertyUsed();

	//And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}


/********************************************************************************************

>	void TemplateDialog::OnAddButtonClicked()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Responds to the user's request to add the selected template argument to the
				list of used properties.

********************************************************************************************/
void TemplateDialog::OnAddButtonClicked()
{
	// For each selected item in the Available list add the appropriate attribute
	MakePropertyUsed();

	// And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}


/********************************************************************************************

>	void TemplateDialog::OnRemoveButtonClicked()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Handles the message when the user clicks on the Remove button

********************************************************************************************/
void TemplateDialog::OnRemoveButtonClicked()
{
	RemoveUsedProperty();

	// And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}


/********************************************************************************************

>	void TemplateDialog::OnRemoveAllButtonClicked()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:

********************************************************************************************/
void TemplateDialog::OnRemoveAllButtonClicked()
{	
	RemoveAllUsedProperties();

	// And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}


/********************************************************************************************

>	void TemplateDialog::OnMakeStyleClicked()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Responds to a request to make a style from a WizOp

********************************************************************************************/
void TemplateDialog::OnMakeStyleClicked()
{	
	// (should probably do this somewhere else - GOD this is ugly)
	AvailableProperty* const pSelectedProperty = (AvailableProperty*)GetAvailableProperties().GetFirstSelectedEntry();
	if (pSelectedProperty != NULL)
	{
		// Invoke a dialog for the name
		NameDialogParam StyleNameParam;
		if (NameDialog::Invoke(StyleNameParam))		// returns TRUE if they didn't cancel
		{
			// if so create a new style
			Style* pNewStyle = pSelectedProperty->MakeStyle(StyleNameParam.GetNameEntered());
			if (pNewStyle != NULL)
			{
				WizOpStyle* pStyleToUse = (WizOpStyle*)AddNewStyle(*pNewStyle);
				if (pStyleToUse != NULL)
				{
					GetAvailableProperties().AddStyle(*pStyleToUse);
					pSelectedProperty->Hide();
				}

				// AddNewStyle creates a copy, so we can bin this
				delete pNewStyle;
				pNewStyle = NULL;
			}
			// disable the Make Style button 'cos we don't have a working attribute now
			m_pMakeStyleButton->Disable();
			m_pAddButton->Disable();
		}
	}

	// And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}




/********************************************************************************************

>	void TemplateDialog::OnAvailableListHighlightChanged()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes, mostly Graham's Web/Hot/URL/Link Dialog)
	Created:	09/06/97
	
	Purpose:	Handles the message that the Available Properties list box has had its 
				highlight changed. If there wasn't a highlight before you can now press the
				add button.

********************************************************************************************/
void TemplateDialog::OnAvailableListHighlightChanged()
{
	OnAvailableSelectionChange();

	// And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}


/********************************************************************************************

>	void TemplateDialog::OnUsedListHighlightChanged()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes, mostly Graham's Web/Hot/URL/Link Dialog)
	Created:	09/06/97
	
	Purpose:	Handles the message that the Used Properties list box has had its highlight
				changed. Updates the question edit box as appropriate.

********************************************************************************************/
void TemplateDialog::OnUsedListHighlightChanged()
{
	OnUsedSelectionChange();

	// And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}


/********************************************************************************************

>	void TemplateDialog::OnApplyClicked()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Responds to the user's request to apply the Name and Template Property
				attributes to the selection.

********************************************************************************************/
void TemplateDialog::OnApplyClicked()
{
	GetUsedProperties().RemoveSelection();	// this ensures the shown question is updated

	ApplyUsedAttributes();
	
	//And throw away the keyboard focus
	DialogManager::DefaultKeyboardFocus();
}


/********************************************************************************************

>	void TemplateDialog::OnSelectionChange()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes, mostly Graham's Web/Hot/URL/Link Dialog)
	Created:	09/06/97
	
	Purpose:	Handles a message that there's been a selection change

********************************************************************************************/
void TemplateDialog::OnSelectionChange()
{	
	//Put the dialog into its initial state
	InitializeControls();
}


/********************************************************************************************

>	void TemplateDialog::SetDialogInitialState(BaseDocument* const pSelectedDocument)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes, mostly Graham's Web/Hot/URL/Link Dialog)
	Created:	09/06/97
	
	Purpose:	Handles a message that there's been a selection change

********************************************************************************************/
void TemplateDialog::SetDialogInitialState(BaseDocument* const pSelectedDocument)
{	
	m_UserCanModifyQuestion	= FALSE;

	// Fill the left-hand list box with the available WizOps / Styles & a Custom property
	BuildAvailablePropertiesList(pSelectedDocument);

	// Set the controls up in their correct initial state
	InitializeControls();
}


/********************************************************************************************

>	void TemplateDialog::InitializeControls()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Sets up the dialog according to what attributes are applied to the
				selection. The cases are:

********************************************************************************************/
void TemplateDialog::InitializeControls()
{
	// Fill the right-hand one with applied user attributes
	BuildUsedPropertiesList();

	ShowInitialControlAvailablity();
}


/********************************************************************************************

>	void TemplateDialog::ShowInitialControlAvailablity()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
********************************************************************************************/
void TemplateDialog::ShowInitialControlAvailablity()
{
	m_pQuestionGadget->Disable();
	m_pParamGadget->Disable();

	m_pAddButton->Disable();
	m_pRemoveButton->Disable();
	m_pRemoveAllButton->Disable();

	m_pMakeStyleButton->Disable();

	m_pApplyButton->Disable();

	// if nothing's selected don't let them click on it
	SelRange* const pSelRange = GetApplication()->FindSelection();
	if (pSelRange != NULL && pSelRange->IsEmpty())
	{
		m_pAvailableListGadget->Disable();
		m_pUsedListGadget->Disable();
	}
	else
	{
		m_pAvailableListGadget->Enable();
		m_pUsedListGadget->Enable();
	}
}


/********************************************************************************************

>	void TemplateDialog::OnAvailableSelectionChange()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Responds to a change to the selected item in the Available List
	
********************************************************************************************/
void TemplateDialog::OnAvailableSelectionChange()
{
	VOID_ENSURE_NOT_NULL(m_pAddButton);

	if (m_pAddButton != NULL)
	{
		// Remove the selection from the selected list. This will remove any question / param 
		// so they can be used to add new entries
		if (GetUsedProperties().GetNumberOfSelectedEntries() > 0)
		{
			GetUsedProperties().RemoveSelection();
		}

		GetAvailableProperties().OnSelectionChange(*this);

		// should use a Notify for this...
		if (GetAvailableProperties().GetNumberOfSelectedEntries() > 0)
		{
			m_pAddButton->Enable();
		}
		else
		{
			m_pAddButton->Disable();
		}
	}

	VOID_ENSURE_NOT_NULL(m_pMakeStyleButton);

	if (m_pMakeStyleButton != NULL)
	{
		// ensure the style button is disabled
		m_pMakeStyleButton->Disable();
	}

}


/********************************************************************************************

>	void TemplateDialog::ReflectAvailableListChange()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Updates the state of the buttons which are determined solely by the 
				Available properties list.
	
********************************************************************************************/
/*
void TemplateDialog::ReflectAvailableListChange()
{
	// should use a Notify for this...
	if (GetAvailableProperties().GetNumberOfSelectedEntries() > 0)
	{
		m_pAddButton->Enable();
	}
	else
	{
		m_pAddButton->Disable();
	}


	BOOL ShouldEnableMakeStyle = FALSE;
	if (GetAvailableProperties().GetNumberOfSelectedEntries() == 1)
	{
		AvailableProperty* pSelectedEntry = (AvailableProperty*)GetAvailableProperties().GetFirstSelectedEntry();
		VOID_ENSURE_NOT_NULL(pSelectedEntry);

		ShouldEnableMakeStyle = pSelectedEntry->CanBecomeStyle();
	}

	if (ShouldEnableMakeStyle)
	{
		m_pMakeStyleButton->Enable();
	}
	else
	{
		m_pMakeStyleButton->Disable();
	}
}
*/

/********************************************************************************************

>	void TemplateDialog::OnUsedSelectionChange()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
********************************************************************************************/
void TemplateDialog::OnUsedSelectionChange()
{
	VOID_ENSURE_NOT_NULL(m_pRemoveButton);
	VOID_ENSURE_NOT_NULL(m_pRemoveAllButton);
	VOID_ENSURE_NOT_NULL(m_pMakeStyleButton);
	VOID_ENSURE_NOT_NULL(m_pAddButton);

	if (m_pRemoveButton != NULL && m_pRemoveAllButton != NULL && m_pMakeStyleButton != NULL &&
		m_pAddButton != NULL)
	{
		// Remove the selection from the available list
		if (GetAvailableProperties().GetNumberOfSelectedEntries() > 0)
		{
			GetAvailableProperties().RemoveSelection();
			m_pMakeStyleButton->Disable();
			m_pAddButton->Disable();
		}

		GetUsedProperties().OnSelectionChange(*this);

		// Can only remove selected items
		if (GetUsedProperties().GetNumberOfSelectedEntries() > 0)
		{
			m_pRemoveButton->Enable();
		}
		else
		{
			m_pRemoveButton->Disable();
		}

		// Whenever there's anything in the used list, enable the Remove All Button
		if (GetUsedProperties().GetNumberOfEntries() > 0)
		{
			m_pRemoveAllButton->Enable();
		}
		else
		{
			m_pRemoveAllButton->Disable();
		}
	}
}


/********************************************************************************************

>	void TemplateDialog::ReflectUsedListChange()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Updates the state of the buttons which are determined solely by the 
				Available properties list.
	
********************************************************************************************/
/*
void TemplateDialog::ReflectUsedListChange()
{
	// Can only remove selected items
	if (GetUsedProperties().GetNumberOfSelectedEntries() > 0)
	{
		m_pRemoveButton->Enable();
	}
	else
	{
		m_pRemoveButton->Disable();
	}

	// Whenever there's anything in the used list, enable the Remove All Button
	if (GetUsedProperties().GetNumberOfEntries() > 0)
	{
		m_pRemoveAllButton->Enable();
	}
	else
	{
		m_pRemoveAllButton->Disable();
	}
}
*/


/********************************************************************************************

>	BOOL TemplateDialog::BuildAvailablePropertiesList(BaseDocument* const pSelectedDocument)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
********************************************************************************************/
BOOL TemplateDialog::BuildAvailablePropertiesList(BaseDocument* const pSelectedDocument)
{
	BOOL Ok = TRUE;

	// Delete the last load of available properties we got when the selected document changed
	delete m_pAvailableProperties;
	m_pAvailableProperties = NULL;

	if (pSelectedDocument != NULL)
	{
		// Fill 'er up with template handlers aka WizOps
		m_pAvailableProperties = new AvailablePropertiesList();
	}

	Ok = (m_pAvailableProperties != NULL && m_pAvailableListGadget != NULL);

	if (Ok)
	{
		Ok = m_pAvailableProperties->Interact(*m_pAvailableListGadget);
	}

	if (Ok)
	{
		Ok = m_pAvailableProperties->AddTemplateHandlers(GetWizOps());
	}

	// ...and any styles that are hanging about
	if (Ok)
	{
		WizOpStyles* const pStyles = GetStyles();
		Ok = (pStyles != NULL);

		if (Ok)
		{
#if FALSE
			WizOpStyle NewStyle(String_8("Style"), String_64("The shading in the trees"), 
								*GetWizOps().GetFirstWizOp(), String_8("P1"));
			pStyles->AddStyle(NewStyle);
#endif
			Ok = m_pAvailableProperties->AddStyles(*pStyles);
		}
	}
	// ...and add a Custom Property entry
	if (Ok && ShowCustomProperty())
	{
		CustomProperty* pCustomProperty = new CustomProperty();
		if (pCustomProperty != NULL)
		{
			Ok = m_pAvailableProperties->InsertEntry(*pCustomProperty);
		}
	}

	if (!Ok)
	{
		delete m_pAvailableProperties;
		m_pAvailableProperties = NULL;
	}

	return Ok;
}



/********************************************************************************************

>	BOOL TemplateDialog::BuildUsedPropertiesList()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

********************************************************************************************/
BOOL TemplateDialog::BuildUsedPropertiesList()
{
	BOOL Ok = TRUE;

	// when the selection changes, bin the old used properties list, and start afresh
	delete m_pUsedProperties;
	m_pUsedProperties = NULL;

	m_pUsedProperties = new UsedPropertiesList();

	Ok = (m_pUsedProperties != NULL && m_pUsedListGadget != NULL);

	if (Ok)
	{
		Ok = m_pUsedProperties->Interact(*m_pUsedListGadget);
	}

	RenderableNodeEnumerator* pEnumerator = NULL;

	if (Ok)
	{
		pEnumerator = CreateEnumerator();
		if (pEnumerator != NULL)
		{
			Ok = m_pUsedProperties->BuildFromEnumerator(*pEnumerator);
		}
	}

	delete pEnumerator;
	pEnumerator = NULL;

	return Ok;
}


/********************************************************************************************

>	void TemplateDialog::MakePropertyUsed()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

********************************************************************************************/
void TemplateDialog::MakePropertyUsed()
{
	ENSURE_THIS();

	const UINT32 OrignalEntries = GetUsedProperties().GetNumberOfEntries(); // should be Notified really

	// Get the selected template handler in the available properties list
	VisibleListIterator Iterator = GetAvailableProperties().SelectionBegin();
	while (Iterator != GetAvailableProperties().End())
	{
		//  and add a new entry in the used list
		AvailableProperty& PropertyToUse = (AvailableProperty&)*Iterator;
		ENSURE_KIND((&PropertyToUse), AvailableProperty);

		GetUsedProperties().AddAvailableProperty(PropertyToUse);
		++Iterator;
	}

	// if we actually added something then enable the Apply button
	if (GetUsedProperties().GetNumberOfEntries() > OrignalEntries)
	{
		m_pApplyButton->Enable();		// should be Notified really
		m_pRemoveAllButton->Enable();	// should be Notified really
	}

	// disable the Make Style button 'cos we don't have a working attribute now
	m_pMakeStyleButton->Disable();
	m_pAddButton->Disable();
}



/********************************************************************************************

>	void TemplateDialog::RemoveUsedProperty()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

********************************************************************************************/
void TemplateDialog::RemoveUsedProperty()
{
	ENSURE_THIS();

	GetUsedProperties().DeleteSelectedEntries();

	m_pApplyButton->Enable();	// should be Notified really

	m_pRemoveButton->Disable();	// should be Notified really

	if (GetUsedProperties().GetNumberOfEntries() == 0)
	{
		m_pRemoveAllButton->Disable();	// should be Notified really
	}
}


/********************************************************************************************

>	void TemplateDialog::RemoveAllUsedProperties()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

********************************************************************************************/
void TemplateDialog::RemoveAllUsedProperties()
{
	ENSURE_THIS();

	GetUsedProperties().DeleteAllEntries();

	m_pApplyButton->Enable();	// should be Notified really

	m_pRemoveButton->Disable();	// should be Notified really

	m_pRemoveAllButton->Disable();	// should be Notified really
}


/********************************************************************************************

>	Style* TemplateDialog::AddNewStyle(const Style& NewStyle) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Support function that attempts to add the given style to the styles
				associated with the selected document.

	Returns:	A pointer to the style to use (the one that was added)
				NULL if the addition failed or the Style Name already exists in the selected
				document's style component.

********************************************************************************************/
Style* TemplateDialog::AddNewStyle(const Style& NewStyle) const
{
	Style* pStyleToUse = NULL;

	WizOpStyles* const pStyles = GetStyles();
	if (pStyles != NULL)
	{
		// don't give them a style to use if one with the same name already exists
		pStyleToUse = pStyles->FindStyleFromName(NewStyle.GetName());
		if (pStyleToUse == NULL)
		{
			pStyleToUse = pStyles->AddStyle(NewStyle);
		}
		else
		{
			pStyleToUse = NULL;
		}
	}
	
	return pStyleToUse;
}



/********************************************************************************************

>	WizOps& TemplateDialog::GetWizOps()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

********************************************************************************************/
WizOps& TemplateDialog::GetWizOps() const
{
	return ::GetWizOps();
}


/********************************************************************************************

>	RenderableNodeEnumerator* TemplateDialog::CreateEnumerator() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Provides a suitable Node enumerator depending on the selection

********************************************************************************************/
RenderableNodeEnumerator* TemplateDialog::CreateEnumerator() const
{
	RenderableNodeEnumerator* pEnumerator = NULL;

	SelRange* const pSelRange = GetApplication()->FindSelection();

	if (pSelRange != NULL && !pSelRange->IsEmpty())
	{
		pEnumerator = new RangeEnumerator(pSelRange);
	}
	else if (pSelRange != NULL && pSelRange->IsEmpty())
	{
		// If there's no selection the AttrUser's need to be applied to the Document (maybe)
//		pEnumerator = new DocPseudoEnumerator(Document::GetSelected());
	}

	return pEnumerator;
}



/********************************************************************************************

>	void TemplateDialog::ApplyUsedAttributes()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97
	
	Purpose:	Applies all the properties marked as used to the current selection

********************************************************************************************/
void TemplateDialog::ApplyUsedAttributes()
{
	GetUsedProperties().Commit(*this);

	m_pApplyButton->Disable();
}






/********************************************************************************************

>	AvailablePropertiesList& TemplateDialog::GetAvailableProperties()
	UsedPropertiesList& TemplateDialog::GetUsedProperties()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Data Member access

********************************************************************************************/
AvailablePropertiesList& TemplateDialog::GetAvailableProperties()
{
	// This static dummy object is used for returning in the ERROR2 case
	// The original used to return a ptr to a local variable, which is a tad dangerous.
	// This solution is not ideal, because there's a permanent instance of an object
	// that will probably never be used.
	static AvailablePropertiesList DummyList;

	ERROR2IF(m_pAvailableProperties == NULL, DummyList, "NULL Member");
	return *m_pAvailableProperties;
}


UsedPropertiesList& TemplateDialog::GetUsedProperties()
{
	// This static dummy object is used for returning in the ERROR2 case
	// The original used to return a ptr to a local variable, which is a tad dangerous.
	// This solution is not ideal, because there's a permanent instance of an object
	// that will probably never be used.
	static UsedPropertiesList DummyList;

	ERROR2IF(m_pUsedProperties == NULL, DummyList, "NULL Member");
	return *m_pUsedProperties;
}


TextControl& TemplateDialog::GetKeyControl() const
{
	ASSERT(m_pParamGadget != NULL);

	return *m_pParamGadget;
}


TextControl& TemplateDialog::GetValueControl() const
{
	ASSERT(m_pQuestionGadget != NULL);

	return *m_pQuestionGadget;
}

StaticTextControl& TemplateDialog::GetParamHintControl() const
{
	ASSERT(m_pParamHintGadget != NULL);

	return *m_pParamHintGadget;
}

WizOpStyles* TemplateDialog::GetStyles() const
{
	WizOpStyles* pStyles = NULL;

	BaseDocument* pDoc = Document::GetSelected();
	if (pDoc != NULL)
	{
		pStyles = (WizOpStyles*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(WizOpStyles));
	}

	return pStyles;
}


BOOL TemplateDialog::UserCanModifyQuestion() const
{
	return m_UserCanModifyQuestion;
}

void TemplateDialog::SetUserCanModifyQuestion(BOOL TheyCan)
{
	m_UserCanModifyQuestion = TheyCan;
}

/********************************************************************************************

>	BOOL TemplateDialog::CreateGadgets()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Support function to create the controls in the dialog.
				The DialogManager should do this but I haven't time.

********************************************************************************************/
BOOL TemplateDialog::CreateGadgets()
{
	m_pUsedListGadget = new ListControl(this, _R(IDC_TMLPTDLG_USED));

	m_pAvailableListGadget = new ListControl(this, _R(IDC_TMLPTDLG_AVAILABLE));

	m_pQuestionGadget = new TextControl(this, _R(IDC_TMLPTDLG_QUESTION));

	m_pParamHintGadget = new StaticTextControl(this, _R(IDC_TMPLTDLG_PARAM_TEXT));

	m_pParamGadget = new TextControl(this, _R(IDC_TMPLTDLG_PARAM));

	m_pAddButton = new Button(this, _R(IDC_TMPLTDLG_ADD));

	m_pRemoveButton = new Button(this, _R(IDC_TMPLTDLG_REMOVE));

	m_pRemoveAllButton = new Button(this, _R(IDC_TMPLTDLG_REMOVE_ALL));

	m_pApplyButton = new Button(this, IDOK);

	m_pMakeStyleButton = new Button(this, _R(IDC_TMPLTDLG_MAKE_STYLE));

	return (m_pUsedListGadget != NULL && m_pAvailableListGadget != NULL &&
			m_pQuestionGadget != NULL && m_pParamHintGadget != NULL && m_pParamGadget != NULL && 
			m_pAddButton != NULL && m_pRemoveButton != NULL && m_pRemoveAllButton != NULL &&
			m_pApplyButton != NULL &&
			m_pMakeStyleButton != NULL
			);
}


