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

// Attribute Template Dialog
#ifndef INC_TMPLTDLG
#define INC_TMPLTDLG

#define OPTOKEN_TEMPLATEDLG		_T("TemplateDlg")

class OpDescriptor;
class Msg;
class WizOps;
class WizOpStyles;
class Style;
class ListControl;
class TextControl;
class Button;
class StaticTextControl;
class AvailablePropertiesList;
class UsedPropertiesList;
class RenderableNodeEnumerator;
class BaseDocument;

/********************************************************************************************

>	class TemplateDialog : public DialogOp


	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from assorted dialog classes)
	Created:	09/06/97

	Purpose:	Allows intelligent users to make attributes changeable via templates.

********************************************************************************************/
PORTNOTE("dialog","Removed TemplateDialog - UI class")
#ifndef EXCLUDE_FROM_XARALX
class TemplateDialog : public DialogOp
{
	CC_DECLARE_DYNCREATE(TemplateDialog);

	//Constructors, destructors and initialiser
public:
	// Metaclass members
	static BOOL		Init();
	static OpState	GetState(String_256*, OpDescriptor*);

public:
	TemplateDialog();
	virtual ~TemplateDialog();

	// Creating the dialog
public:
	// Overridden 
	virtual void Do(OpDescriptor*);

	TextControl& GetKeyControl() const;
	TextControl& GetValueControl() const;
	StaticTextControl& GetParamHintControl() const;

	void SetUserCanModifyQuestion(BOOL TheyAre);

protected:
	// Overridden 
	virtual	MsgResult Message(Msg* Message);

	virtual void OnCreate();
	virtual void OnTextChange();
	virtual void OnParamChange();
	virtual void OnAddButtonClicked();
	virtual void OnRemoveButtonClicked();
	virtual void OnRemoveAllButtonClicked();
	virtual void OnUsedListHighlightChanged();
	virtual void OnAvailableListHighlightChanged();
	virtual void OnAvailableListDoubleClicked();
	virtual void OnSelectionChange();
	virtual void OnMakeStyleClicked();
	virtual void OnApplyClicked();

	void OnAvailableSelectionChange();
	void OnUsedSelectionChange();

protected:
	// Implementation
	void SetDialogInitialState(BaseDocument* const pSelectedDocument);
	void InitializeControls();
	void ReflectControlAvailability();
	void ShowInitialControlAvailablity();

	RenderableNodeEnumerator* CreateEnumerator() const;
	Style* AddNewStyle(const Style& NewStyle) const;

	BOOL BuildUsedPropertiesList();
	BOOL BuildAvailablePropertiesList(BaseDocument* const pSelectedDocument);
	void MakePropertyUsed();
	void RemoveUsedProperty();
	void RemoveAllUsedProperties();
	BOOL CreateGadgets();

protected:
	void ApplyUsedAttributes();

protected:
	// Data Member Access
	AvailablePropertiesList&	GetAvailableProperties();
	UsedPropertiesList&			GetUsedProperties();
	BOOL UserCanModifyQuestion() const;

	static BOOL ShowCustomProperty();

	WizOps& GetWizOps() const;
	WizOpStyles* GetStyles() const;

public:
	static const CDlgResID	s_IDD;
	static const CDlgMode	s_Mode;

private:
//	Should generate these dynamically...
	ListControl*		m_pUsedListGadget;
	ListControl*		m_pAvailableListGadget;
	TextControl*		m_pQuestionGadget;
	StaticTextControl*	m_pParamHintGadget;
	TextControl*		m_pParamGadget;
	Button*				m_pAddButton;
	Button*				m_pRemoveButton;
	Button*				m_pRemoveAllButton;
	Button*				m_pApplyButton;
	Button*				m_pMakeStyleButton;

	AvailablePropertiesList*	m_pAvailableProperties;
	UsedPropertiesList*			m_pUsedProperties;

// yuck! a state variable telling us we may want to enable the apply button if the text changes
	BOOL				m_UserCanModifyQuestion;

// preferences
	static String_32	s_PreferenceSection;	// all preferences go here
	static const UINT32	s_NumberOfPreferences;

	static String_16	s_ShowCustom;			// preference name
	static BOOL			s_ShowCustomProperty;	// ...and the var (ooh nasty no serialization)
};
#endif
#endif //INC_TMPLTDLG

