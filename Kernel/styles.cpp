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

/*
*/

#include "camtypes.h"
//#include "doccomp.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "styles.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "basedoc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "camfiltr.h"	// BaseCamelotFilter - version 2 native filter - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"	// TAG_...
//#include "cxfdefs.h"	//  - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxfile.h"		// CXF_UNKNOWN_SIZE
//#include "cxfrec.h"		// CXaraFileRecord handler - in camtypes.h [AUTOMATICALLY REMOVED]
#include "userattr.h"
#include "tmpltatr.h"
#include "tmpltarg.h"
//#include "tmpltdlg.h"

//#include "tmpltres.h"
//#include "tim.h"		// _R(IDE_FILE_READ_ERROR)

/********************************************************************************************

>	class RecordReferenceItem : public ListItem

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/96

	Purpose:	When importing & exporting we want to hang on to references of the records
				we've written or read and the objects they represent, so that referencing 
				records can pick up the associated item.
				This abstract class provides just the record reference (XFileRef).
				Derived classes should provide the object.

	SeeAlso:	StyleRecordReference

********************************************************************************************/
class RecordReferenceItem : public ListItem
{
	CC_DECLARE_MEMDUMP(RecordReferenceItem)

public:
	virtual BOOL operator==(const RecordReferenceItem& Other) const = 0;
	virtual RecordReferenceItem* CreateCopy(const XFileRef& NewReference) const = 0;

// Data access
	XFileRef GetRecordRef() const									{	return m_RecordRef;	}

protected:
	// only derived classes can "construct" one of these
	RecordReferenceItem(XFileRef Ref = XFILEREF_ERROR) : m_RecordRef(Ref)	{}


private:
	XFileRef	m_RecordRef;
};



/********************************************************************************************

>	class StyleRecordReference : public RecordReferenceItem

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/96

	Purpose:	Represents an association twixt Style and Record reference

	SeeAlso:	StylesExporter, WizOpStyleRecordHandler

********************************************************************************************/
class StyleRecordReference : public RecordReferenceItem
{
	CC_DECLARE_MEMDUMP(StyleRecordReference)

public:
	StyleRecordReference(const Style& StyleExported, XFileRef Ref = XFILEREF_ERROR) : 
		RecordReferenceItem(Ref),
		m_StyleExported(StyleExported) {}

	RecordReferenceItem* CreateCopy(const XFileRef& NewReference) const;
	virtual BOOL operator==(const RecordReferenceItem& Other) const;

	const Style& GetStyle() const							{	return m_StyleExported;	}

private:
	const Style&	m_StyleExported;
};


/********************************************************************************************

>	class StyleContainerItem : public ListItem

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/96

	Purpose:	Provides an item for the container of a load of Style's

	SeeAlso:	StyleContainer

********************************************************************************************/
class StyleContainerItem : public ListItem
{
	CC_DECLARE_MEMDUMP(StyleContainerItem)

public:
	StyleContainerItem(Style& TheStyle) : m_Style(TheStyle)	{}
	virtual ~StyleContainerItem()								{	delete &m_Style;	}

	Style& GetStyle()											{	return m_Style;	}


	StyleContainerItem(StyleContainerItem& Other) : m_Style(Other.m_Style)
	{
		TRACE( _T("StyleContainerItem - Copy constructor not implemented"));
	}
	StyleContainerItem& operator=(StyleContainerItem& Other)
	{
		TRACE( _T("StyleContainerItem - Assignment operator not implemented"));
		return *this;
	}

private:
	Style& m_Style;
};


/********************************************************************************************

>	class StyleContainer : public List

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/96

	Purpose:	Provides a container for a load of Style's

	SeeAlso:	Styles

	Notes:		At present this is a list. Someone might want a more efficient data
				structure for searching...

********************************************************************************************/
class StyleContainer : public List
{
	CC_DECLARE_MEMDUMP(StyleContainer)
public:
	~StyleContainer()				{	DeleteAll();	}

	BOOL AddStyle(Style& TheStyle);

};








DECLARE_SOURCE("$Revision$");

#define ENSURE_KIND(pInstance, Class)	ERROR3IF(!pInstance->IS_KIND_OF(Class), #pInstance" is not kind of "#Class);


CC_IMPLEMENT_MEMDUMP(StyleContainerItem, ListItem)
CC_IMPLEMENT_MEMDUMP(StyleContainer, List)

CC_IMPLEMENT_DYNAMIC(StylesMessage, Msg)

CC_IMPLEMENT_MEMDUMP(Style, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_DYNAMIC(Styles, DocComponent)

CC_IMPLEMENT_DYNAMIC(WizOpStyleComponentClass, DocComponentClass)
CC_IMPLEMENT_MEMDUMP(WizOpStyle, Style)
CC_IMPLEMENT_DYNAMIC(WizOpStyles, Styles)

CC_IMPLEMENT_MEMDUMP(RecordReferenceItem, ListItem)
CC_IMPLEMENT_MEMDUMP(StyleRecordReference, RecordReferenceItem)

CC_IMPLEMENT_DYNAMIC(WizOpStyleRecordHandler, CamelotRecordHandler)

CC_IMPLEMENT_MEMDUMP(RecordReferencingExporter, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(StylesExporter, RecordReferencingExporter)

// This should come after the IMPLEMENT macros
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



BOOL StyleContainer::AddStyle(Style& TheStyle)
{
	BOOL Ok = TRUE;

	StyleContainerItem* const pLink = new StyleContainerItem(TheStyle);
	Ok = (pLink != NULL);

	if (Ok)
	{
		AddTail(pLink);
	}

	return Ok;
}


StyleIterator::StyleIterator(StyleContainer& Container, StyleContainerItem* const pPosition) :
	m_CurrentItem(pPosition),
	m_Container(Container)
{
}



StyleIterator& StyleIterator::operator++()
{
	m_CurrentItem = (StyleContainerItem*)m_Container.GetNext(m_CurrentItem);

	return *this;
}


BOOL StyleIterator::operator!=(const StyleIterator& Other)
{
	return m_CurrentItem != Other.m_CurrentItem;
}


Style& StyleIterator::operator*()
{
	ASSERT(m_CurrentItem != NULL);

	return m_CurrentItem->GetStyle();
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//										S  T  Y  L  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






/********************************************************************************************

>	Style::Style(const StringBase& Name)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Constructs a Style of the given Name and thereby makes sure this Style has
				a name (unless you give it an empty string)

********************************************************************************************/
Style::Style(const StringBase& Name)
{
	SetName(Name);
}



/********************************************************************************************

>	Style::~Style()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Bizzaro pure implementation

********************************************************************************************/
Style::~Style()
{
}


/********************************************************************************************

>	BOOL Style::operator==(const Style& OtherStyle) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Compares two Style's

	Returns:	TRUE if the names are the same, FALSe if not

********************************************************************************************/
BOOL Style::operator==(const Style& OtherStyle) const
{
	return GetName() == OtherStyle.GetName();
}



/********************************************************************************************

>	const StringBase& Style::GetName() const
	BOOL Style::SetName(const StringBase& Name)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Derived class data member access

********************************************************************************************/
const StringBase& Style::GetName() const
{
	return m_Name;
}


BOOL Style::SetName(const StringBase& Name)
{
	m_Name = Name;

	return TRUE;
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//										S  T  Y  L  E  S
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //






/********************************************************************************************

>	Styles::Styles()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Default constructor would fail, so use an Init() if you don't want a lot
				of ERROR2's

********************************************************************************************/
Styles::Styles()
{
	m_pStyleContainer	= NULL;		// we'll fill this in in init()
	m_pCurrentExporter	= NULL;		// we create an exporter when we need one
	m_pStylesToAdd		= NULL;		// this is used during a Component Copy
}


/********************************************************************************************

>	BOOL Styles::Init()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Default constructor would fail, so use an Init() if you don't want a lot
				of ERROR2's

********************************************************************************************/
BOOL Styles::Init()
{
	m_pStyleContainer = new StyleContainer;

	return (m_pStyleContainer != NULL);
}


/********************************************************************************************

>	Styles::~Styles()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97
	Purpose:	Clean up a colour list component's data structures - deletes the colour
				list.
	Errors:		ENSURE if IndexedColour copying stuff has not been cleaned up (this will
				occur if End/AbortComponentCopy is not called after a component copy)
	SeeAlso:	WizOpStyles

********************************************************************************************/
Styles::~Styles()
{
	if (m_pCurrentExporter)
	{
		// This should have been deleted in EndExport()
		TRACE( _T("Styles::~Styles() - m_pCurrentExporter not gone"));
		delete m_pCurrentExporter;
		m_pCurrentExporter = NULL;
	}

	delete m_pStyleContainer;
	m_pStyleContainer = NULL;
}


/********************************************************************************************

>	Styles::Styles(const Styles&)
	Styles& Styles::operator=(const Styles&)	

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Warns us that we've done a destructor but we haven't done either of these

********************************************************************************************/
Styles::Styles(const Styles&)
{
	TRACE( _T("Styles - Copy constructor not implemented"));
}

Styles& Styles::operator=(const Styles&)
{
	TRACE( _T("Styles - Assignment operator not implemented"));

	return *this;
}


/********************************************************************************************

>	StyleContainer& Styles::GetContainer() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	The actual styles are held in an internal container. This member provides
				access to them.

********************************************************************************************/
StyleContainer& Styles::GetContainer() const
{
	// This static dummy object is used for returning in the ERROR2 case
	// The original used to return a ptr to a local variable, which is a tad dangerous.
	// This solution is not ideal, because there's a permanent instance of an object
	// that will probably never be used.
	static StyleContainer Dummy;

	ERROR2IF(m_pStyleContainer == NULL, Dummy, "NULL Member");

	return *m_pStyleContainer;
}



/********************************************************************************************

>	virtual Style* Styles::AddStyle(const Style& NewStyle)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Adds the given style to this Styles collection, ensuring that there's only
				one with the style's name.

	Inputs:		NewStyle - the style to add
	Returns:	A pointer to the style contained in this collection or NULL if it didn't
				work.

********************************************************************************************/
Style* Styles::AddStyle(const Style& NewStyle)
{
	ENSURE_NOT_NULL(m_pStyleContainer);

	// Look for an existing one which we could return
	Style* pStyleToUse = FindStyleFromName(NewStyle.GetName());
	if (pStyleToUse == NULL)
	{
		// didn't find one so create a spanking new version
		Style* pNewStyleCopy = NewStyle.CreateCopy(NewStyle.GetName());
		if (pNewStyleCopy != NULL)
		{
			if (!GetContainer().AddStyle(*pNewStyleCopy))
			{
				delete pNewStyleCopy;
				pNewStyleCopy = NULL;
			}
		}
		pStyleToUse = pNewStyleCopy;
	}

	return pStyleToUse;
}

/********************************************************************************************

>	virtual StyleIterator Styles::Begin() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Allows access to individual members of the Styles collection

	Returns:	An iterator which will run through to the End() of the styles collection.

********************************************************************************************/
StyleIterator Styles::Begin() const
{
	if (m_pStyleContainer != NULL)
	{
		return StyleIterator(*m_pStyleContainer, (StyleContainerItem*)(GetContainer().GetHead()));
	}
	else
	{
		StyleContainer	Container;
		return StyleIterator( Container, NULL );
	}
}


/********************************************************************************************

>	const StyleIterator& Styles::End() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Allows access to individual members of the Styles collection

	Returns:	A reference to a constant iterator which marks the end of the collection

********************************************************************************************/
const StyleIterator& Styles::End() const
{
	StyleContainer		Container;
	static const StyleIterator TheEnd( Container, NULL);

	return TheEnd;
}


/********************************************************************************************

>	virtual Style* Styles::AddStyle(const Style& NewStyle)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Adds the given style to this Styles collection, ensuring that there's only
				one with the style's name.

	Inputs:		NewStyle - the style to add
	Returns:	A pointer to the style contained in this collection or NULL if it didn't
				work.

********************************************************************************************/
Style* Styles::FindStyleFromName(const StringBase& StyleName) const
{
	ENSURE_NOT_NULL(m_pStyleContainer);

	Style* pStyleFound = NULL;	// return this

	StyleContainerItem* pCurrentStyle = (StyleContainerItem*)m_pStyleContainer->GetHead();
	while (pCurrentStyle != NULL && pStyleFound == NULL)
	{
		if (pCurrentStyle->GetStyle().GetName() == StyleName)
		{
			pStyleFound = &(pCurrentStyle->GetStyle());
		}
		else
		{
			pCurrentStyle = (StyleContainerItem*)m_pStyleContainer->GetNext(pCurrentStyle);
		}
	}

	return pStyleFound;
}


/********************************************************************************************

>	virtual BOOL Styles::MergeStyles(const Styles& OtherStyles)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Removes the styles in the given styles container and adds them to this one.
				Duplicates are ignored, so be careful.

	Inputs:		OtherStyles - the styles to merge
	Returns:	TRUE if it worked

********************************************************************************************/
BOOL Styles::MergeStyles(Styles& OtherStyles)
{
	BOOL Ok = TRUE;

	StyleContainer& OtherContainer = OtherStyles.GetContainer();
	StyleContainer& ThisContainer = GetContainer();

	ListItem* pLink = OtherContainer.RemoveHead();
	while (pLink != NULL && Ok)
	{
		ThisContainer.AddTail(pLink);

		pLink = OtherContainer.RemoveHead();
	}

	return Ok;
}


/********************************************************************************************

>	virtual BOOL Styles::StartComponentCopy()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	This function gets called to prepare for an inter-document styles copy.

	Returns:	TRUE if it succeeded. FALSE if the copy must be aborted

	SeeAlso:	WizOpStyles::EndComponentCopy;
				WizOpStyles::AbortComponentCopy;
				WizOpStyles::CopyComponentData

********************************************************************************************/
BOOL Styles::StartComponentCopy()
{
	ERROR2IF(m_pStylesToAdd != NULL, FALSE, "Styles::StartComponentCopy() - Copy already in progress?\n");

	BOOL Ok = TRUE;
	
	m_pStylesToAdd = new Styles();
	Ok = (m_pStylesToAdd != NULL);

	if (Ok)
	{
		m_pStylesToAdd->Init();
	}

	return Ok;
}


/********************************************************************************************

>	virtual Style* Styles::CopyComponentData(const Style& StyleToCopy)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	In between StartComponentCopy() & EndComponentCopy() this function is called
				by StyleReferenceAttribute::CopyComponentData() to ensure that styles are
				merged between documents.

	Inputs:		The style that should be used by any reference.
	Returns:	The style that should be used by any reference.


********************************************************************************************/
Style* Styles::CopyComponentData(const Style& StyleToCopy)
{
	// it would be helpful if one of these were set up in StartComponentCopy()
	ENSURE_NOT_NULL(m_pStylesToAdd);

	Style* pStyleToUse = NULL;		// return this

	// If we've already got a style of the same name 
	//	If the style definitions are the same then use the existing one
	//	If they're different, rename the style, and add it to the list to add at the end

	Style* pStyleToAdd = NULL;	// if any
	Style* const pExistingStyle = FindStyleFromName(StyleToCopy.GetName());
	if (pExistingStyle != NULL)
	{
		if (StyleToCopy == *pExistingStyle)
		{
			pStyleToUse = pExistingStyle;
		}
		else	// conflicting style names so create a new one
		{
			String_64 NewStyleName = StyleToCopy.GetName();
			GenerateUniqueName(NewStyleName, &NewStyleName);
			pStyleToAdd = StyleToCopy.CreateCopy(NewStyleName);
		}
	}
	else
	{
		// there was no existing style so add the style to the list of styles to add at the end
		pStyleToAdd = StyleToCopy.CreateCopy(StyleToCopy.GetName());
	}

	// We may need to add a new style to the list to merge at the end
	if (pStyleToAdd != NULL)
	{
		pStyleToUse = GetStylesToAdd().AddStyle(*pStyleToAdd);
		// AddStyle copies the one we give it, so delete it
		delete pStyleToAdd;
	}

	return pStyleToUse;
}


/********************************************************************************************

>	virtual BOOL Styles::EndComponentCopy()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Will commit all changes made by doing the component copy, returning TRUE.
				Any new colours will be inserted as instructed by the InsertBefore parameter.
				If some colours will not be insterted because they were merged, then a report
				to this effect can be given if desired.

	Returns:	TRUE if it succeeded


********************************************************************************************/
BOOL Styles::EndComponentCopy()
{
	// it would be helpful if one of these were set up in StartComponentCopy()
	// Unfortunately Undo'ing a Copy will just call this without a Start...we'll do nothing 
	// then
	if (m_pStylesToAdd != NULL)
	{
		MergeStyles(GetStylesToAdd());
		delete m_pStylesToAdd;
		m_pStylesToAdd = NULL;

		SendStylesChangeMessage();
	}

	return TRUE;
}



/********************************************************************************************

>	void Styles::AbortComponentCopy()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Will abort all changes made for component copies.
				This means that things such as the colours referenced by the nodes
				you have just copied do not exist, so you must clean up to the state
				the document was in before you strated copying.

	Notes:		May be called even if StartComponentCopy has not been called.
				May be called multiple times

	SeeAlso:	WizOpStyles::EndComponentCopy;
				WizOpStyles::AbortComponentCopy;
				WizOpStyles::CopyColourAcross

********************************************************************************************/
void Styles::AbortComponentCopy()
{
	delete m_pStylesToAdd;
	m_pStylesToAdd = NULL;
}


/********************************************************************************************

>	Styles& Styles::GetStylesToAdd() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	During a component copy (or import) we have some styles to add at the end
				(although they maybe empty).
				This function provides access to them.

	Errors:		If you call this at the wrong time an ERROR2 will result.

********************************************************************************************/
Styles& Styles::GetStylesToAdd() const
{
	// This static dummy object is used for returning in the ERROR2 case
	// The original used to return a ptr to a local variable, which is a tad dangerous.
	// This solution is not ideal, because there's a permanent instance of an object
	// that will probably never be used.
	static Styles Dummy;

	ERROR2IF(m_pStylesToAdd == NULL, Dummy, "NULL Member");

	return *m_pStylesToAdd;
}


/********************************************************************************************

>	BOOL Styles::NameExists(const StringBase& Name) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Tells us if the given name exists in this Styles collection or the Styles
				we're adding (cos we only want to eliminate duplicates when we're adding
				styles see).

	Inputs:		Name:	the name of the style we want to add
	Returns:	TRUE if there's a style in here with that Name, FALSE if not

	Notes:		Used by GenerateUniqueName() during ComponentCopy

********************************************************************************************/
BOOL Styles::NameExists(const StringBase& Name) const
{
	return (FindStyleFromName(Name) != NULL || 
			GetStylesToAdd().FindStyleFromName(Name) != NULL);
}


/********************************************************************************************

>	BOOL Styles::GenerateUniqueName(const StringBase& Prefix, StringBase* pResult)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Jason (From code by Tim))
	Created:	11/07/97 (16/12/94 (8/8/94))

	Purpose:	Scans the styles to determine if the suggested name is unique
				within this collection. If it is not, the name is altered (currently by
				removing the last 14 characters and appending a decimal number) to provide
				a unique name, which is returned in the supplied string.

	Inputs:		pPrefix - Points to the suggested name for this style.

	Outputs:	pResult - is filled in with a unique name

	Returns:	TRUE if the pResult contains a different string from that supplied in
				pPrefix. (pResult will always contain a valid result, but checking this flag
				may save you the bother of copying the string over)


	Notes:		This method should only be called when importing or copying styles.
				Name matching is done using StringBase::IsIdentical, so see that method
				to determine the rules by which strings are matched

********************************************************************************************/
BOOL Styles::GenerateUniqueName(const StringBase& Prefix, StringBase* pResult)
{
	ENSURE_NOT_NULL(pResult);

	// Either the List is empty, or this name is unique, so return it (ensuring < 64 chars)
	Prefix.Left(pResult, pResult->MaxLength());
	if (!NameExists(*pResult))
	{
		return(FALSE);
	}

	Prefix.Left(pResult, pResult->MaxLength() - 14);					// Copy into NewName, truncating
	INT32 PrefixLen = pResult->Length();

	LPTSTR pPrefixEnd = (TCHAR*)(*pResult);
	pPrefixEnd += PrefixLen;

	INT32 i = 2;

	do 
	{
		camSnprintf( pPrefixEnd, 256, _T(" %ld"), i++ );
	}
	while (NameExists(*pResult));

	return TRUE;
}



void Styles::SendStylesChangeMessage()
{
	PORTNOTETRACE("other","Styles::SendStylesChangeMessage - do nothing - using TemplateDialog");
#ifndef EXCLUDE_FROM_XARALX
	if (MessageHandler::MessageHandlerExists(CC_RUNTIME_CLASS(TemplateDialog)))
	{
		BROADCAST_TO_CLASS(StylesMessage(*this), DialogOp);
	}
#endif
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//		V  I  S  I  B  L  E     T  E  M  P  L  A  T  E     A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //



WizOpStyle::WizOpStyle(	const StringBase& Name, const StringBase& Question, 
						const WizOp& WizOpRef, const StringBase& Param) :
	Style(Name),
	m_WizOp(WizOpRef)
{
	SetQuestion(Question);

	SetParam(Param);
}



Style* WizOpStyle::CreateCopy(const StringBase& NewName) const
{
	return new WizOpStyle(NewName, GetQuestion(), GetWizOp(), GetParam());
}


BOOL WizOpStyle::operator==(const Style& OtherStyle) const
{
	ERROR3IF(!OtherStyle.IsKindOf(CC_RUNTIME_CLASS(WizOpStyle)), "OtherStyle isn't a WizOpStyle");

	const WizOpStyle& OtherWizOpStyle = (const WizOpStyle&)OtherStyle;

	return (Style::operator==(OtherStyle) &&
			m_Question	== OtherWizOpStyle.m_Question &&
			&m_WizOp	== &(OtherWizOpStyle.m_WizOp) &&
			m_Param		== OtherWizOpStyle.m_Param);
}


BOOL WizOpStyle::SetQuestion(const StringBase& NewQuestion)
{
	if (NewQuestion.Length() > m_Question.MaxLength())
	{
		TRACE( _T("WizOpStyle::SetQuestion - Couldn't you ask a shorter question?\n"));
	}
	NewQuestion.Left(&m_Question, m_Question.MaxLength());

	return TRUE;
}

BOOL WizOpStyle::SetParam(const StringBase& NewParam)
{
	BOOL AllCopied = TRUE;

	if (NewParam.Length() > m_Param.MaxLength())
	{
		TRACE( _T("WizOpStyle::SetParam - That param's too long!\n"));
		AllCopied = FALSE;
	}
	NewParam.Left(&m_Param, m_Param.MaxLength());

	return AllCopied;
}


const StringBase& WizOpStyle::GetQuestion() const
{
	return m_Question;
}

const WizOp& WizOpStyle::GetWizOp() const
{
	return m_WizOp;
}

const StringBase& WizOpStyle::GetParam() const
{
	return m_Param;
}



/********************************************************************************************

>	BOOL WizOpStyles::StartImport(BaseCamelotFilter *pFilter)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
	Returns:	TRUE if the component was able to prepare for importing;
				FALSE if not (e.g. out of memory)
	Purpose:	Inform the colour list document component that a Native or Web import is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/
BOOL WizOpStyles::StartImport(BaseCamelotFilter* pFilter)
{
	ENSURE_NOT_NULL(pFilter);

	BOOL Ok = StartComponentCopy();

	return Ok;
}



/********************************************************************************************

>	WizOpStyle* WizOpStyles::FindStyleFromRecordReference(XFileRef RecordRef) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	In between StartImport & EndImport this function is called by
				WizOpStyleRefRecordHandler::HandleRecord so it knows which WizOpStyle to use
				for its StyleRefAttribute.

********************************************************************************************/
WizOpStyle* WizOpStyles::FindStyleFromRecordReference(XFileRef RecordRef) const
{
	return GetRecordHandler().FindStyleFromRecordReference(RecordRef);
}


/********************************************************************************************

>	BOOL WizOpStyles::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Inform the component that a Native or Web import has just finished.
				Calls EndCopyComponent(q.v)

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - TRUE => The import was successful;
						 FALSE => The import failed - abandon any changes.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)

********************************************************************************************/
BOOL WizOpStyles::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
	ENSURE_NOT_NULL(pFilter);

	BOOL Ok = EndComponentCopy();

	m_pImporter = NULL;

	return Ok;
}


BOOL WizOpStyles::SetRecordHandler(WizOpStyleRecordHandler& RecordHandler)
{
#ifdef _DEBUG
	/*Styles& ThisShouldWork =*/ GetStylesToAdd();		// there should be some styles that we can add to
#endif

	m_pImporter = &RecordHandler;

	return TRUE;
}


WizOpStyleRecordHandler& WizOpStyles::GetRecordHandler() const
{
	// This static dummy object is used for returning in the ERROR2 case
	// The original used to return a ptr to a local variable, which is a tad dangerous.
	// This solution is not ideal, because there's a permanent instance of an object
	// that will probably never be used.
	static WizOpStyleRecordHandler Dummy;

	ERROR2IF(m_pImporter == NULL, Dummy, "NULL Member");

	return *m_pImporter;
}


/********************************************************************************************

>	BOOL WizOpStyles::StartExport(BaseCamelotFilter *pFilter)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97
	Returns:	TRUE if the component was able to prepare for exporting;
				FALSE if not (e.g. out of memory)
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Purpose:	Inform the colour list document component that a WEb or Native export is
				about to start.
	SeeAlso:	DocComponent

********************************************************************************************/
BOOL WizOpStyles::StartExport(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ENSURE_NOT_NULL(pFilter);

	// We don't support multi-threading
	ERROR2IF(m_pCurrentExporter != NULL, FALSE, "Current exporter already exists");

	// Create an exporter for the styles
	m_pCurrentExporter = new StylesExporter(*this, *pFilter);
	if (m_pCurrentExporter == NULL)
		return FALSE;
#endif
	return TRUE;
}


/********************************************************************************************

>	virtual XFileRef WizOpStyles::GetRecordRefForStyle(const WizOpStyle& Style)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Writes out a WizOpStyle record

	Inputs:		Style	- The document colour to save

	Returns:	A record reference to be used for the style.
				This will be zero if a problem has occurred. 

	Notes:		In the process of getting a record reference for the style the style itself
				may be written to the record stream. As this does save information, it
				*MUST* be called before the caller has called StartRecord() as otherwise 
				the records will become intertwined!

********************************************************************************************/
XFileRef WizOpStyles::GetRecordRefForStyle(const WizOpStyle& Style)
{
#ifdef DO_EXPORT
	// We should only be here after a StartExport
	ENSURE_NOT_NULL(m_pCurrentExporter);

	INT32 RecordNumber = m_pCurrentExporter->WriteStyle(Style);
	
	return RecordNumber;
#endif
	return 0L;
}



/********************************************************************************************

>	BOOL WizOpStyles::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to import a file.
				Success - True if everything went swimmingly, False if just a clean up is required.
	Purpose:	Inform the colour list document component that a Web or Native export has
				just finished.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/
BOOL WizOpStyles::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
#ifdef DO_EXPORT
	ENSURE_NOT_NULL(pFilter);
	ENSURE_NOT_NULL(m_pCurrentExporter);

	delete m_pCurrentExporter;
	m_pCurrentExporter = NULL;

	return TRUE;
#endif		
	return TRUE;
}





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			V  I  S  I  B  L  E  T  E  M  P  L  A  T  E  A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





/********************************************************************************************

>	BOOL WizOpStyleComponentClass::Init()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/97

	Purpose:	Registers this WizOpStyleComponentClass with the main application. It should
				be called at start-up, somewhere in main3.cpp would be nice.

	Returns:	TRUE if all went well;
				FALSE if not.

********************************************************************************************/
BOOL WizOpStyleComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	WizOpStyleComponentClass *pClass = new WizOpStyleComponentClass;
	if (pClass == NULL)
		return FALSE;

	// Register it
	GetApplication()->RegisterDocComponent(pClass);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL WizOpStyleComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/97

	Purpose:	Adds the WizOpStyleComponent to the specified document.

	Inputs:		pDocument - the document to add the WizOpStyleComponent to.

	Returns:	TRUE if the unit list was added ok;
				FALSE if not.

********************************************************************************************/
BOOL WizOpStyleComponentClass::AddComponent(BaseDocument* pDocument)
{
	BOOL Ok = TRUE;

	// Check to see if this document already has a WizOp Style list; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(WizOpStyles)) != NULL)
		return TRUE;

	// Ok - create the WizOp Style component using this list.
	WizOpStyles* pComponent = new WizOpStyles();
	Ok = (pComponent != NULL);

	if (Ok)
	{
		Ok = pComponent->Init();
	}


	if (Ok)
	{
		// All ok - add the component to the document.
		pDocument->AddDocComponent(pComponent);
	}

	return Ok;
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			V  I  S  I  B  L  E  T  E  M  P  L  A  T  E  A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //




/********************************************************************************************

>	WizOpStyleRecordHandler::WizOpStyleRecordHandler()

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	Makes sure data members are initialized to safe values.

********************************************************************************************/
WizOpStyleRecordHandler::WizOpStyleRecordHandler() : 
	m_pTentativeStyles(NULL)
{
}


/********************************************************************************************

>	WizOpStyleRecordHandler::~WizOpStyleRecordHandler()

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	Makes sure data members are unheaped properly

********************************************************************************************/
WizOpStyleRecordHandler::~WizOpStyleRecordHandler()
{
	if (!m_ImportedItems.IsEmpty())
	{
		TRACE( _T("WizOpStyleRecordHandler - Imported items left over\n"));
	}
}


/********************************************************************************************

>	WizOpStyleRecordHandler::WizOpStyleRecordHandler(const WizOpStyleRecordHandler& Other)
	WizOpStyleRecordHandler& WizOpStyleRecordHandler::operator=(const WizOpStyleRecordHandler& Other)

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	Warns us that we've done a destructor but we haven't done either of these

********************************************************************************************/
WizOpStyleRecordHandler::WizOpStyleRecordHandler(const WizOpStyleRecordHandler& Other)
{
	TRACE( _T("WizOpStyleRecordHandler - Copy constructor not implemented"));
}

WizOpStyleRecordHandler& WizOpStyleRecordHandler::operator=(const WizOpStyleRecordHandler& Other)
{
	TRACE( _T("WizOpStyleRecordHandler - Assignment operator not implemented"));
	return *this;
}


/********************************************************************************************

>	virtual UINT32* WizOpStyleRecordHandler::GetTagList()

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	Provides the record handler system with a list of records handled by this
				handler

	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END

********************************************************************************************/
UINT32* WizOpStyleRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_WIZOP_STYLE,
							  CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}


/********************************************************************************************

>	virtual BOOL WizOpStyleRecordHandler::Init(BaseCamelotFilter* pFilter)

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	Initialises the WizOpStyleRecordHandler

	Returns:	TRUE if ok
				FALSE otherwise

********************************************************************************************/
BOOL WizOpStyleRecordHandler::Init(BaseCamelotFilter* pFilter)
{
	BOOL Ok = TRUE;

	Ok = CamelotRecordHandler::Init(pFilter);

	Document* const pDoc = GetDocument();
	Ok = (pDoc != NULL);

	if (Ok)
	{
		m_pTentativeStyles = (WizOpStyles*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(WizOpStyles));
		Ok = (m_pTentativeStyles != NULL);
	}

	// give this WizOpStyleRecordHandler to the style component so it can call us back
	if (Ok)
	{
		Ok = m_pTentativeStyles->SetRecordHandler(*this);
	}

	return Ok;
}



/********************************************************************************************

>	virtual BOOL WizOpStyleRecordHandler::EndImport()

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	Informs the record handler that importing is over
				Reverses all the stuff done in Init(). Should it be done in BeginImport?

	Returns:	TRUE if ok
				FALSE otherwise

	Notes:			If you override this func, be sure to call the base class varient

********************************************************************************************/
BOOL WizOpStyleRecordHandler::EndImport()
{
	BOOL Ok = TRUE;

	m_ImportedItems.DeleteAll();

	Ok = CXaraFileRecordHandler::EndImport();

	return Ok;
}


/********************************************************************************************

>	virtual BOOL WizOpStyleRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.

********************************************************************************************/
BOOL WizOpStyleRecordHandler::HandleRecord(CXaraFileRecord* pRecord)
{
	ENSURE_NOT_NULL(pRecord);
	ERROR2IF(pRecord->GetTag() != TAG_WIZOP_STYLE, FALSE, "I don't handle this tag type");

	String_64 OpName;
	String_256 Question;
	String_256 Param;
	String_256 StyleName;

	BOOL ok = pRecord->ReadUnicode(OpName, 64);

	if (ok)
	{
		ok = pRecord->ReadUnicode(Question, 256);
	}

	if (ok)
	{
		ok = pRecord->ReadUnicode(Param, 256);
	}

	if (ok)
	{
		ok = pRecord->ReadUnicode(StyleName, 256);
	}

	if (ok)
	{
		// We managed to read it all OK so add a new Style to the styles we've imported
		// and remember its record number for reference purposes
		WizOpStyle* pStyleToUse = AddStyle(StyleName, Question, OpName, Param);
		if (pStyleToUse == NULL)
		{
			BaseCamelotFilter* const pFilter = GetBaseCamelotFilter();
			ENSURE_NOT_NULL(pFilter);
			pFilter->AppendWarning(_R(IDS_STYLE_OF_UNKNOWN_WIZOP));
		}
		else
		{
			ok = AddRecordReference(pRecord->GetRecordNumber(), *pStyleToUse);
		}

	}

	if (!ok)
	{
		BaseCamelotFilter* const pFilter = GetBaseCamelotFilter();
		ENSURE_NOT_NULL(pFilter);
		pFilter->GotError(_R(IDE_FILE_READ_ERROR));
	}

	return ok;
}


/********************************************************************************************

>	WizOpStyle* WizOpStyleRecordHandler::AddStyle(	const StringBase& Name, 
													const StringBase& Question, 
													const StringBase& WizOpName, 
													const StringBase& Param)
 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	Adds a WizOpStyle to the importing document's WizOpStyles component

	Inputs:		Name, Question, Param:	Passed on to the WizOpStyle
				WizOpName:	The name of the WizOp that the style refers to

	Returns:	A pointer to the WizOpStyle to use in any reference
				NULL if it failed

********************************************************************************************/
WizOpStyle* WizOpStyleRecordHandler::AddStyle(	const StringBase& Name, const StringBase& Question, 
										const StringBase& WizOpName, const StringBase& Param)
{
	WizOpStyle* pStyleToUse = NULL;

	WizOp* const pWizOp = GetWizOps().FindWizOpFromInternalName(WizOpName);

	if (pWizOp != NULL)
	{
		WizOpStyle NewStyle(Name, Question, *pWizOp, Param);
		pStyleToUse = (WizOpStyle*)GetTentativeStyles().CopyComponentData(NewStyle);
	}

	return pStyleToUse;
}


/********************************************************************************************

>	BOOL WizOpStyleRecordHandler::AddRecordReference(	XFileRef RecordRef, 
														const WizOpStyle& ImportedStyle)

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	We need to remember the record numbers of the styles so that Style References
				can find the style they want. This function associates a record number
				with the newly imported style to that end.

	Inputs:		RecordRef:		The record number of the newly imported style
				ImportedStyle:	The newly imported style

	Returns:	TRUE if it managed to add the reference
				FALSE if not

	See Also:	WizOpStyleRefRecordHandler::HandleRecord

********************************************************************************************/
BOOL WizOpStyleRecordHandler::AddRecordReference(XFileRef RecordRef, const WizOpStyle& ImportedStyle)
{
	BOOL Ok = TRUE;

	StyleRecordReference* pNewRecRef = new StyleRecordReference(ImportedStyle, RecordRef);

	Ok = (pNewRecRef != NULL);

	if (Ok)
	{
		m_ImportedItems.AddHead(pNewRecRef);
	}

	return Ok;
}


/********************************************************************************************

>	virtual WizOpStyle* WizOpStyleRecordHandler::FindStyleFromRecordReference(
																XFileRef RecordRef) const
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	Finds an imported style, given the record number.

	Inputs:		RecordRef: The number of the record in which the style was.

********************************************************************************************/
WizOpStyle* WizOpStyleRecordHandler::FindStyleFromRecordReference(XFileRef RecordRef) const
{
	WizOpStyle* pFoundStyle = NULL;

	StyleRecordReference* pCurrentSRR = (StyleRecordReference*)m_ImportedItems.GetHead();

	while (pCurrentSRR != NULL)
	{
		if (pCurrentSRR->GetRecordRef() == RecordRef)
		{
			pFoundStyle = (WizOpStyle*)&(pCurrentSRR->GetStyle());
			break;
		}
		else
		{
			pCurrentSRR = (StyleRecordReference*)m_ImportedItems.GetNext(pCurrentSRR);
		}
	}

	return pFoundStyle;
}


/********************************************************************************************

>	virtual void WizOpStyleRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	This provides descriptions for the define units records.

	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update

********************************************************************************************/
#ifdef XAR_TREE_DIALOG
void WizOpStyleRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
	{
		TRACE( _T("WizOpStyleRecordHandler::GetRecordDescriptionText - NULL Args\n"));
		return;
	}
	if (pRecord->GetTag() != TAG_WIZOP_STYLE)
	{
		TRACE( _T("WizOpStyleRecordHandler::GetRecordDescriptionText - Incorrect record\n"));
		return;
	}

	//	 Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord, pStr);

	TCHAR OpName[64]	= {0};
	TCHAR Question[256]	= {0};
	TCHAR Param[256]	= {0};
	TCHAR StyleName[256]	= {0};

	BOOL ok = pRecord->ReadUnicode(OpName, 64);

	if (ok)
	{
		*pStr += TEXT("OpName  = ");
		*pStr += OpName;

		ok = pRecord->ReadUnicode(Question, 256);
	}

	if (ok)
	{
		*pStr += TEXT("\r\nQuestion = ");
		*pStr += Question;

		ok = pRecord->ReadUnicode(Param, 256);
	}

	if (ok)
	{
		*pStr += TEXT("\r\nParam = ");
		*pStr += Param;

		ok = pRecord->ReadUnicode(StyleName, 256);
	}

	if (ok)
	{
		*pStr += TEXT("\r\nStyleName = ");
		*pStr += StyleName;
	}
}


/********************************************************************************************

>	virtual void WizOpStyleRecordHandler::GetTagText(UINT32 Tag,String_256& Str)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	A debug function that gives the tag description.

	Inputs:		Tag	= the tag value
	Outputs:	Str = place to put textual description of the tag

********************************************************************************************/
void WizOpStyleRecordHandler::GetTagText(UINT32 Tag,String_256& Str)
{
	Str = TEXT("TAG_WIZOP_STYLE");
}
#endif // _DEBUG



/********************************************************************************************

>	WizOpStyles& WizOpStyleRecordHandler::GetTentativeStyles()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Data Access

********************************************************************************************/
WizOpStyles& WizOpStyleRecordHandler::GetTentativeStyles()
{
	// This static dummy object is used for returning in the ERROR2 case
	// The original used to return a ptr to a local variable, which is a tad dangerous.
	// This solution is not ideal, because there's a permanent instance of an object
	// that will probably never be used.
	static WizOpStyles Dummy;

	ERROR2IF(m_pTentativeStyles == NULL, Dummy, "NULL Member");

	return *m_pTentativeStyles;
}




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//							E  X  P  O  R  T  I  T  E  M
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //







// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//		R  E  C  O  R  D  R  E  F  E  R  E  N  C  I  N  G  E  X  P  O  R  T  E  R
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //




const INT32 XFILEREF_ERROR = 0L;


/********************************************************************************************

>	RecordReferencingExporter::~RecordReferencingExporter()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Deletes the internal data structures

********************************************************************************************/
RecordReferencingExporter::~RecordReferencingExporter()
{
	m_ExportedItems.DeleteAll();
}



/********************************************************************************************

>	RecordReferencingExporter::RecordReferencingExporter(const RecordReferencingExporter& Other)
	const RecordReferencingExporter& RecordReferencingExporter::operator=(const RecordReferencingExporter& Other)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Let's us know we should be copying this

********************************************************************************************/
RecordReferencingExporter::RecordReferencingExporter(const RecordReferencingExporter& Other) :
	m_Filter(Other.m_Filter)
{
	TRACE( _T("RecordReferencingExporter - Copy constructor not implemented"));
}

const RecordReferencingExporter& RecordReferencingExporter::operator=(const RecordReferencingExporter& Other)
{
	TRACE( _T("RecordReferencingExporter - Assignment operator not implemented"));
	return *this;
}


/********************************************************************************************

>	virtual BOOL RecordReferencingExporter::AddExportedItem(RecordReferenceItem& NewItem)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Adds the RecordReferenceItem to the list of exported items. 
				You can override it if you want.

********************************************************************************************/
BOOL RecordReferencingExporter::AddExportedItem(RecordReferenceItem& NewItem)
{
	m_ExportedItems.AddHead(&NewItem);

	return TRUE;
}

/********************************************************************************************

>	const RecordReferenceItem* RecordReferencingExporter::FindExportedItem(const RecordReferenceItem& ItemToFind)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Looks for the given item in the list of exported items.
				If you're worried about efficiency override it.

********************************************************************************************/
const RecordReferenceItem* RecordReferencingExporter::FindExportedItem(const RecordReferenceItem& ItemToFind)
{
	const RecordReferenceItem* pFoundItem = NULL;		// return this

	RecordReferenceItem* pCurrentItem = (RecordReferenceItem*)m_ExportedItems.GetHead();

	while (pCurrentItem != NULL && pFoundItem == NULL)
	{
		if (ItemToFind == *pCurrentItem)
		{
			pFoundItem = pCurrentItem;
		}
		else
		{
			pCurrentItem = (RecordReferenceItem*)m_ExportedItems.GetNext(pCurrentItem);
		}
	}

	return pFoundItem;
}


/********************************************************************************************

>	XFileRef RecordReferencingExporter::WriteItem(const RecordReferenceItem& TentativeExportItem)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Provides support for derived classes using record references.
				Attempts to write out an export item, but will just return an XFileRef if the
				record has already been written for the item.
				Derived classes should create a suitable item on the stack, and call this 
				function to do the spade work.

	Inputs:		TentativeExportItem:	The item to be exported that will be copied if it
										needs to be written out.

	Returns:	The record number for the given RecordReferenceItem
				XFILEREF_ERROR if an error occurred.

********************************************************************************************/
XFileRef RecordReferencingExporter::WriteItem(const RecordReferenceItem& TentativeExportItem)
{
#ifdef DO_EXPORT
	XFileRef RecordReference = XFILEREF_ERROR;	// return this

	const RecordReferenceItem* const pExistingItem = FindExportedItem(TentativeExportItem);
	if (pExistingItem == NULL)
	{
		// The item hasn't been exported, so do it now
		RecordReference = WriteRecord(TentativeExportItem);
		if (RecordReference != XFILEREF_ERROR)
		{
			RecordReferenceItem* pNewItem = TentativeExportItem.CreateCopy(RecordReference);
			if (pNewItem != NULL)
			{
				AddExportedItem(*pNewItem);
			}
			else
			{
				RecordReference = XFILEREF_ERROR;
			}
		}
	}
	else	// the record has already been written, so just get the reference
	{
		RecordReference = pExistingItem->GetRecordRef();
	}

	return RecordReference;
#else
	return 0L;
#endif
}






// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//				S  T  Y  L  E  R  E  C  O  R  D  R  E  F  E  R  E  N  C  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //









/********************************************************************************************

>	RecordReferenceItem* StyleRecordReference::CreateCopy(const XFileRef& NewReference) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Creates a copy of this StyleRecordReference with the given NewReference

********************************************************************************************/
RecordReferenceItem* StyleRecordReference::CreateCopy(const XFileRef& NewReference) const
{
	return new StyleRecordReference(m_StyleExported, NewReference);
}



/********************************************************************************************

>	BOOL StyleRecordReference::operator==(const RecordReferenceItem& Other)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Two StyleRecordReference's are equal if the addresses of the Style's they 
				represent are equal

********************************************************************************************/
BOOL StyleRecordReference::operator==(const RecordReferenceItem& Other) const
{
	ENSURE_KIND((&Other), StyleRecordReference);
	StyleRecordReference& OtherStyle = (StyleRecordReference&) Other;

	return &(GetStyle()) == &(OtherStyle.GetStyle());
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//						S  T  Y  L  E  S  E  X  P  O  R  T  E  R
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //





/********************************************************************************************

>	StylesExporter::StylesExporter(Styles& StylesToExport, BaseCamelotFilter& FilterToUse)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Default constructor to ensure the data members are set up correctly.
				We need a Styles collection and a Filter to export by.

********************************************************************************************/
StylesExporter::StylesExporter(Styles& StylesToExport, BaseCamelotFilter& FilterToUse) : 
	RecordReferencingExporter(FilterToUse),
	m_Styles(StylesToExport)
{
}



/********************************************************************************************

>	virtual XFileRef StylesExporter::WriteStyle(const WizOpStyle& StyleToWrite)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Attempts to write a WizOpStyle via the filter given in this object's 
				constructor.

	Returns:	A Record Number to use as a reference (or XFILEREF_ERROR if it failed)

	See Also:	Styles::GetRecordRefForStyle which delegates all responsibility to this
				member.

********************************************************************************************/
XFileRef StylesExporter::WriteStyle(const WizOpStyle& StyleToWrite)
{
	XFileRef RecordReference = XFILEREF_ERROR;	// return this

	StyleRecordReference RecordReferenceItem(StyleToWrite);

	RecordReference = WriteItem(RecordReferenceItem);

	return RecordReference;
}


/********************************************************************************************

>	XFileRef StylesExporter::WriteRecord(const RecordReferenceItem& ItemToExport)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/97

	Purpose:	Provides implementation for the base class pure function to write the given
				RecordReferenceItem (i.e. StyleRecordReference) via the filter given in this object's
				constructor.

	Notes:		Called from RecordReferencingExporter::WriteItem, so look there for further
				info.

********************************************************************************************/
XFileRef StylesExporter::WriteRecord(const RecordReferenceItem& ItemToExport)
{
#ifdef DO_EXPORT
	XFileRef RecordNumber = XFILEREF_ERROR;		// return this (initialized to error)

	ENSURE_KIND((&ItemToExport), StyleRecordReference);

	// Add a description of the TAG_WIZOP_STYLE record for older importers
	GetFilter().AddTagDescription(TAG_WIZOP_STYLE, _R(IDS_TAG_WIZOP_STYLE));

	CamelotFileRecord Rec(&GetFilter(), TAG_WIZOP_STYLE, CXF_UNKNOWN_SIZE);

	BOOL ok = Rec.Init();

	WizOpStyle& StyleToExport = (WizOpStyle&)((StyleRecordReference&)ItemToExport).GetStyle();

	if (ok)
	{
		String_64 InternalName = StyleToExport.GetWizOp().GetInternalName();
		ok = Rec.WriteUnicode((TCHAR*)InternalName);
	}
	if (ok)
	{
		String_256 Question = StyleToExport.GetQuestion();
		ok = Rec.WriteUnicode((TCHAR*)Question);
	}
	if (ok)
	{
		String_64 Param = StyleToExport.GetParam();
		ok = Rec.WriteUnicode((TCHAR*)Param);
	}
	if (ok)
	{
		String_64 StyleName = StyleToExport.GetName();
		ok = Rec.WriteUnicode((TCHAR*)StyleName);
	}

	if (ok)
	{
		RecordNumber = GetFilter().WriteDefinitionRecord(&Rec);
	}
	return RecordNumber;
#else
	return XFILEREF_ERROR;
#endif
}


