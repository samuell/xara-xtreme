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

#include "attrval.h"
#include "nodeattr.h"
#include "userattr.h"
#include "cxfrech.h"
#include "tmpltatr.h"

#include "gadget.h"
#include "uielem.h"
#include "visiattr.h"	// the visible bits

//#include "tmpltdlg.h"	// for slight bodginess

#include "camfiltr.h"
#include "cxftags.h"
#include "cxfdefs.h"
#include "cxfrec.h"

#include "tmpltarg.h"	// WizOp

//#include "mario.h"		// for _R(IDE_NOMORE_MEMORY)
#include "styles.h"
//#include "tim.h"		// for _R(IDE_FILE_WRITE_ERROR)
//#include "tmpltres.h"	// for _R(IDE_FILE_WRITE_ERROR)

#include "ngsentry.h"

#ifdef _DEBUG
#include "convert.h"
#endif

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(TemplateAttribute, AttrUser)
CC_IMPLEMENT_MEMDUMP(VisibleTemplateAttribute, VisibleUserAttribute)
CC_IMPLEMENT_DYNAMIC(TemplateAttrRecordHandler, CamelotRecordHandler)

CC_IMPLEMENT_MEMDUMP(StyleReferenceAttribute, TemplateAttribute)
CC_IMPLEMENT_MEMDUMP(VisibleStyleReference, VisibleAttribute)
CC_IMPLEMENT_DYNAMIC(WizOpStyleRefRecordHandler, CamelotRecordHandler)


// Give this file in memory dumps
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



const String_8 NullString = TEXT("");

const String_8 TemplateAttribute::s_TemplateKey		= _T("WizOp");
const TCHAR TemplateAttribute::s_ParamDelimiter		= TEXT('/');

//const String_8 NameAttribute::NameKey = "Name";

#define ENSURE_KIND(pInstance, Class)	ERROR3IF(!pInstance->IS_KIND_OF(Class), #pInstance" is not kind of "#Class);


/********************************************************************************************

>	TemplateAttribute::TemplateAttribute(	const StringBase& InternalName,
											const StringBase& Question,
											const StringBase& Param)
 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Default constructor for TemplateAttribute

	Inputs:		InternalName 
				Question
				Param

	Notes:		Currently forms a UserAttribute with a LongKey of
				WizOp\InternalName/Param

********************************************************************************************/
TemplateAttribute::TemplateAttribute(	const StringBase& InternalName,
										const StringBase& Question,
										const StringBase& Param)
{
	// Copy as much of the question as we can.
	SetWizOpNameAndParam(InternalName, Param);

	// Cache this test for faster Attribute gallery scans.
	m_fIsAnObjectName = (GetInternalName() == TA_NAME);

	if (Question.Length() > 0)
	{
		String_256 NewValue;
		INT32 LengthToCopy = (NewValue.MaxLength() < Question.Length())
								? TRACE( _T("TemplateAttribute::TemplateAttribute: Q truncated\n")),
								  NewValue.MaxLength()
								: Question.Length();
		if (LengthToCopy > 0)
		{
			String_256 TruncatedQuestion;
			Question.Left(&TruncatedQuestion, LengthToCopy);

			if (!Value.Value.Alloc(TruncatedQuestion.Length()))
			{
				ERROR1RAW(_R(IDE_NOMORE_MEMORY));
				return;
			}

			Value.Value = TruncatedQuestion;
		}
	}
}


/********************************************************************************************

>	BOOL TemplateAttribute::SetWizOpNameAndParam(	const StringBase& Name, 
													const StringBase& Param)
 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Sets the Key fields in UserAttribute from the WizOp name and Param.

	Inputs:		InternalName 
				Param

	Notes:		Currently forms a UserAttribute with a LongKey of
				WizOp\InternalName/Param

********************************************************************************************/
BOOL TemplateAttribute::SetWizOpNameAndParam(	const StringBase& InternalName, 
												const StringBase& Param)
{
	BOOL NotTruncated = TRUE;		// return this
	String_128 NewLongKey(s_TemplateKey);

	if (InternalName.Length() > 0)
	{
		// Don't forget the delimiter
		INT32 SpaceFree = NewLongKey.MaxLength() - NewLongKey.Length() - 1;

		if (SpaceFree > 0)
		{
			NewLongKey += s_KeyDelimiter;
		}

		if (SpaceFree > 1)
		{
			String_128 TruncatedName;
			INT32 LengthToCopy = (SpaceFree < InternalName.Length()) ? SpaceFree : InternalName.Length();
			InternalName.Left(&TruncatedName, LengthToCopy);
			NewLongKey += TruncatedName;
		}
		else
		{
			NotTruncated = FALSE;
		}
	}

	if (Param.Length() > 0)
	{
		// Don't forget the delimiter
		INT32 SpaceFree = NewLongKey.MaxLength() - NewLongKey.Length() - 1;

		if (SpaceFree > 0)
		{
			NewLongKey += s_ParamDelimiter;
		}

		if (SpaceFree > 1)
		{
			String_256 TruncatedParam;
			INT32 LengthToCopy = (SpaceFree < Param.Length()) ? SpaceFree : Param.Length();
			Param.Left(&TruncatedParam, LengthToCopy);
			NewLongKey += TruncatedParam;
		}
		else
		{
			NotTruncated = FALSE;
		}
	}

	ERRORIF(!Value.Key.Alloc(s_TemplateKey.Length()), _R(IDE_NOMORE_MEMORY), FALSE);
	Value.Key = s_TemplateKey;

	ERRORIF(!Value.LongKey.Alloc(NewLongKey.Length()), _R(IDE_NOMORE_MEMORY), FALSE);
	Value.LongKey = NewLongKey;

	return NotTruncated;
}

/***********************************************************************************************

>	Node* TemplateAttribute::SimpleCopy()

    Author:     Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
    Created:    27/06/97
	
    Purpose:    See base class for details.
   	
***********************************************************************************************/
Node* TemplateAttribute::SimpleCopy()
{
	TemplateAttribute* pNodeCopy = new TemplateAttribute();
	ERRORIF(pNodeCopy == 0, _R(IDE_NOMORE_MEMORY), 0); 
	CopyNodeContents(pNodeCopy);
	return pNodeCopy;
} 



/***********************************************************************************************
>	void TemplateAttribute::CopyNodeContents(TemplateAttribute* pAttr)

    Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
    Created:    27/08/99
    Purpose:    See base class for details.
***********************************************************************************************/

void TemplateAttribute::CopyNodeContents(TemplateAttribute* pAttr)
{
	AttrUser::CopyNodeContents(pAttr);
	pAttr->m_fIsAnObjectName = m_fIsAnObjectName;
}



/***********************************************************************************************
>   void TemplateAttribute::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void TemplateAttribute::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, TemplateAttribute), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, TemplateAttribute))
		CopyNodeContents((TemplateAttribute*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 TemplateAttribute::GetAttrNameID()

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Provides an attribute name for undo, etc.

	Returns:	A resource id for "Wizard Property"

********************************************************************************************/
UINT32 TemplateAttribute::GetAttrNameID()
{
	return _R(IDS_WIZOP);
}



/********************************************************************************************
>	virtual BOOL TemplateAttribute::IsAnObjectName() const

 	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/09/99
	Returns:	TRUE if this attribute has an 'ObjectName' key, FALSE if not.
********************************************************************************************/

BOOL TemplateAttribute::IsAnObjectName() const
{
	return m_fIsAnObjectName;
}



/********************************************************************************************

>	virtual BOOL TemplateAttribute::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Writes the TemplateAttribute to the filter

	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not

********************************************************************************************/
BOOL TemplateAttribute::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// Add a description of the TAG_WIZOP record for older importers
	pFilter->AddTagDescription(TAG_WIZOP, _R(IDS_TAG_WIZOP));

	CamelotFileRecord Rec(pFilter, TAG_WIZOP, CXF_UNKNOWN_SIZE);

	BOOL ok = Rec.Init();

	if (ok)
	{
		String_64 InternalName = GetInternalName();
		ok = Rec.WriteUnicode((TCHAR*)InternalName);
	}
	if (ok)
	{
		String_256 Question = GetQuestion();
		ok = Rec.WriteUnicode((TCHAR*)Question);
	}
	if (ok)
	{
		String_64 Param = GetParam();
		ok = Rec.WriteUnicode((TCHAR*)Param);
	}
	if (ok)
	{
		// Write out some patch space
		String_8 NonConstNullString = NullString;
		ok = Rec.WriteUnicode((TCHAR*)NonConstNullString);
	}

	if (ok)
	{
		ok = pFilter->Write(&Rec);
	}

	if (!ok)
	{
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
	}

	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual AttributeIdentifier	TemplateAttribute::GetAttributeClassID() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Generates an AttributeIdentifier for this Attribute.
				Since the RuntimeClass is inadequate for dynamically generated Atrributes
				(read Nodes), we need to provide an ID that can be created dynamically too.
				This is what an AttributeIdentifier is supposed to be.
				In the case of a TemplateAttribute it returns a pointer to a template
				handler.

********************************************************************************************/
AttributeIdentifier	TemplateAttribute::GetAttributeClassID() const
{
#ifdef _DEBUG
	String_128			ConstructedKey( s_TemplateKey );
	ConstructedKey += s_KeyDelimiter;
	ConstructedKey += GetInternalName();
	if (!GetParam().IsEmpty())
	{
		ConstructedKey += s_ParamDelimiter;
		ConstructedKey += GetParam();
	}
	ERROR3IF(ConstructedKey != Value.LongKey,
				"TemplateAttribute::GetAttributeClassID: ConstructedKey != LongKey");
#endif

	return Value.LongKey;
}


/********************************************************************************************

>	VisibleAttribute* TemplateAttribute::CreateVisibleAttribute() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Creates a template attribute the user can play with. This is all wrong
				(arrggghh)

	Returns:	A pointer to the UI equivalent of this attribute

********************************************************************************************/
VisibleAttribute* TemplateAttribute::CreateVisibleAttribute() const
{
	VisibleAttribute* pVisibleAttribute = NULL;	// return this

	WizOp* pHandler = GetWizOps().FindWizOpFromInternalName(GetInternalName());

	if (pHandler != NULL)
	{
		pVisibleAttribute = new VisibleTemplateAttribute(*pHandler, GetQuestion(), GetParam());
	}

	return pVisibleAttribute;
}



/********************************************************************************************

>	String_256 TemplateAttribute::GetInternalName() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Retrieves the Internal Name of the WizOp associated with this 
				TemplateAttribute.

********************************************************************************************/
String_256 TemplateAttribute::GetInternalName() const
{
	String_256 Name;		// return this
	const StringBase& SourceString = Value.LongKey;

	// Find the bit after the WizOp key
	INT32 StartDelimiterPosition = SourceString.FindNextChar(s_KeyDelimiter);

	// Look for a parameter following
	INT32 EndDelimiterPosition = SourceString.FindNextChar(s_ParamDelimiter);
	if (EndDelimiterPosition < 0)
	{
		// there wasn't a parameter following so the delimiter is beyond the end of 
		// the string
		EndDelimiterPosition = SourceString.Length();
	}

	UINT32 LengthToCopy = EndDelimiterPosition - StartDelimiterPosition - 1;
	if (LengthToCopy > 0)
	{
		SourceString.Mid(&Name, StartDelimiterPosition + 1, LengthToCopy);
	}
	else
	{
		TRACE( _T("TemplateAttribute::GetInternalName - no name\n"));
	}

	return Name;
}



/********************************************************************************************

>	String_256 TemplateAttribute::GetParam() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Retrieves the Parameter that is given to the WizOp when it is invoked.

********************************************************************************************/
String_256 TemplateAttribute::GetParam() const
{
	String_256 Param;		// return this
	const StringBase& SourceString = Value.LongKey;

	// find the start of the parameter string
	INT32 StartDelimiter = SourceString.FindNextChar(s_ParamDelimiter);
	if (StartDelimiter > 0)		
	{
		// We've found the start, the rest of the key is the parameter
		UINT32 LengthToCopy = SourceString.Length() - StartDelimiter - 1;
		if (LengthToCopy > 0)
		{
			SourceString.Mid(&Param, StartDelimiter + 1, LengthToCopy);
		}
		else
		{
			TRACE( _T("TemplateAttribute::GetParam - empty param\n"));
		}
	}

	return Param;
}


/********************************************************************************************

>	String_256 TemplateAttribute::GetQuestion() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Retrieves the Question of this TemplateAttribute

********************************************************************************************/
String_256 TemplateAttribute::GetQuestion() const
{
	String_256 Question;
	const StringBase& SourceString = Value.Value;

	UINT32 LengthToCopy = SourceString.Length() > Question.MaxLength() ? 
								Question.MaxLength() : 
								SourceString.Length();

	SourceString.Left(&Question, LengthToCopy);
	return Question;
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/********************************************************************************************

>	virtual UINT32* TemplateAttrRecordHandler::GetTagList()

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Provides the record handler system with a list of records handled by this
				handler

	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END

	Notes:		Don't ever extend this. Derive a class from it. See GeneralAttrRecordHandler
				for how not to do it. 

********************************************************************************************/
UINT32* TemplateAttrRecordHandler::GetTagList()
{
	static const UINT32 TagList[] = { TAG_WIZOP, CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL TemplateAttrRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Handles the given TAG_WIZOP record.

	Inputs:		pCXaraFileRecord = ptr to record to handle

	Returns:	TRUE if handled successfuly
				FALSE otherwise

	Errors:		If pCXaraFileRecord isn't a TAG_WIZOP record

********************************************************************************************/
BOOL TemplateAttrRecordHandler::HandleRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pRecord->GetTag() != TAG_WIZOP, FALSE, "I don't handle this tag type");

	String_64 OpName;
	String_256 Question;
	String_256 Param;
	String_256 PatchSpace;

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
		ok = pRecord->ReadUnicode(PatchSpace, 256);
	}

	BaseCamelotFilter* pFilter = GetBaseCamelotFilter();
	if (ok)
	{
		NodeSetSentinel * pNodeSetSentinel = pFilter->GetDocument()->GetSetSentinel();

		if (pNodeSetSentinel)
		{
			Node * pContextNode = pFilter->GetInsertContextNode();

			// dont add the attrib if it is a duplicate and it is in the sentinel
			if ((pNodeSetSentinel != pContextNode->FindParent() &&
				pNodeSetSentinel != pContextNode ) ||
				SliceHelper::LookupNameGalleryItem(Param) == NULL)
			{
				// We managed to read it all OK so add a new TemplateAttribute to the tree
	 			TemplateAttribute* pNewAttribute = new TemplateAttribute(OpName, Question, Param);	
				ok = (pNewAttribute != 0) && InsertNode(pNewAttribute);

				// JCF: Web files don't include records for the NodeSetSentinel or its child
				// ObjectName attributes, so make sure that a copy of the TemplateAttribute
				// is inserted as a child of the sentinel as well, if there isn't one already
				// in place.
				if (ok && pNewAttribute->IsAnObjectName())
				{
					// Register/insert the imported ObjectName with the doc's NodeSetSentinel.
					ok = pNodeSetSentinel->OnLoadName(pFilter, pNewAttribute);
				}
			}
		}
	}

	if (!ok)
	{
		ENSURE_NOT_NULL(pFilter);
		pFilter->GotError(_R(IDE_FILE_READ_ERROR));
	}

	return ok;
}


/********************************************************************************************

>	virtual void TemplateAttrRecordHandler::GetRecordDescriptionText(
											CXaraFileRecord* pRecord, StringBase* pStr)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Debug function that provides descriptions for TemplateAttribute (WizOp) records.

	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update

********************************************************************************************/
#ifdef XAR_TREE_DIALOG
void TemplateAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
	{
		TRACE( _T("TemplateAttrRecordHandler::GetRecordDescriptionText - NULL Args\n"));
		return;
	}
	if (pRecord->GetTag() != TAG_WIZOP)
	{
		TRACE( _T("TemplateAttrRecordHandler::GetRecordDescriptionText - Incorrect record\n"));
		return;
	}

	//	 Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord, pStr);

	TCHAR OpName[64]	= {NULL};
	TCHAR Question[256]	= {NULL};
	TCHAR Param[256]	= {NULL};
	TCHAR Patch[256]	= {NULL};

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

		ok = pRecord->ReadUnicode(Patch, 256);
	}

	if (ok)
	{
		*pStr += TEXT("\r\nPatch = ");
		*pStr += Patch;
	}
}


/********************************************************************************************

>	virtual void TemplateAttrRecordHandler::GetTagText(UINT32 Tag,String_256& Str)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	A debug function that gives the tag description.

	Inputs:		Tag	= the tag value
	Outputs:	Str = place to put textual description of the tag

********************************************************************************************/
void TemplateAttrRecordHandler::GetTagText(UINT32 Tag,String_256& Str)
{
	Str = TEXT("TAG_WIZOP");
}
#endif	// _DEBUG








// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			V  I  S  I  B  L  E  T  E  M  P  L  A  T  E  A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //







/********************************************************************************************

>	VisibleTemplateAttribute::VisibleTemplateAttribute(	const WizOp& Handler, 
														const StringBase& Question, 
														const StringBase& Param)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Constructor to ensure the data members are set up correctly.

	Inputs:		Handler:	The WizOp that makes this VisibleTemplateAttribute changeable.
				Question:	The question to ask when changing the attribute
				Param:		Any parameter the WizOp may have.

********************************************************************************************/
VisibleTemplateAttribute::VisibleTemplateAttribute(	const WizOp& Handler, 
													const StringBase& Question, 
													const StringBase& Param) :
	VisibleUserAttribute(NullString, NullString),
	m_Handler(Handler)
{
	// do something here so it can create its text

	SetLongKey(Param);
	SetValue(Question);
}


/********************************************************************************************

>	StringBase& VisibleTemplateAttribute::GetText(StringBase& Description) const;

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Gives the user something to chew on in the "Used Properties" list of the
				Wizard Properties dialog.
	
	Returns:	A string

********************************************************************************************/
StringBase& VisibleTemplateAttribute::GetText(StringBase& Description) const
{
	Description = m_Handler.GetUserName();
	return Description; 
}


/********************************************************************************************

>	virtual BOOL VisibleTemplateAttribute::Display(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Lets the user see the contents of this VisibleTemplateAttribute on the given
				dialog.
				Overrides the base class to provide a param hint

	See Also:	VisibleUserAttribute::Display()
	

********************************************************************************************/
BOOL VisibleTemplateAttribute::Display(DialogOp& Dialog)
{
	SetParamHint( GetWizOp().GetParamHint() );
	// bodgy bit this...
	BOOL				fOk = GetParamHint().Display( Dialog );

	if( fOk )
	{
		fOk = VisibleUserAttribute::Display(Dialog);
	}

	return fOk;
}


/********************************************************************************************

>	virtual BOOL VisibleTemplateAttribute::Interact(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This allows the user to change this VisibleTemplateAttribute
	

********************************************************************************************/
BOOL VisibleTemplateAttribute::Interact(DialogOp& Dialog)
{
	PORTNOTETRACE("other","TemplateAttribute::Interact - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(!((&Dialog)->IS_KIND_OF(TemplateDialog)), "Dialog isn't kind of TemplateDialog");

	TemplateDialog& BetterBeThisDialog = (TemplateDialog&)Dialog;
	
	// if there's a parameter hint, then let them edit the parameter
	// we'll blam 'em with an error message if they give us a duplicate
	if (IsNew() && !GetWizOp().GetParamHint().IsEmpty())
	{
		BetterBeThisDialog.GetParamHintControl().Enable();
		BetterBeThisDialog.GetKeyControl().Enable();
	}

	// always let them change the question
	BetterBeThisDialog.GetValueControl().Enable();
#endif
	return TRUE;
}


/********************************************************************************************

>	virtual void VisibleTemplateAttribute::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Hides this VisibleTemplateAttribute from the user.
				Overrides the base class to hide the param hint
	
	See Also:	VisibleUserAttribute::Hide()

********************************************************************************************/
void VisibleTemplateAttribute::Hide()
{
	GetParamHint().Hide();

	VisibleUserAttribute::Hide();
}


/********************************************************************************************

>	virtual NodeAttribute* VisibleTemplateAttribute::CreateNewAttribute()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Creates a new used property.
	
	Returns:	A new AttrUser

********************************************************************************************/
NodeAttribute* VisibleTemplateAttribute::CreateNewAttribute()
{
	return new TemplateAttribute( m_Handler.GetInternalName(), GetValue(), GetLongKey() );
}



const WizOp& VisibleTemplateAttribute::GetWizOp()
{
	return m_Handler;
}


void VisibleTemplateAttribute::SetParamHint(const StringBase& HintToShow)
{
	m_ParamHint = HintToShow;
}


VisibleLabel& VisibleTemplateAttribute::GetParamHint()
{
	return m_ParamHint;
}










// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			S  T  Y  L  E  R  E  F  E  R  E  N  C  E  A  T  T  R  I  B  U  T  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //







/********************************************************************************************

>	StyleReferenceAttribute::StyleReferenceAttribute(const WizOpStyle& Style)

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Constructor ensures this attribute actually references a style

********************************************************************************************/
StyleReferenceAttribute::StyleReferenceAttribute(const WizOpStyle& Style)
{
	SetWizOpNameAndParam( Style.GetWizOp().GetInternalName(), Style.GetParam() );
	SetStyle( Style );
}


/********************************************************************************************

>	Node* StyleReferenceAttribute::SimpleCopy()

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Creates a new copy of this StyleReferenceAttribute

	Returns:	A new Node

********************************************************************************************/
Node* StyleReferenceAttribute::SimpleCopy()
{
	ENSURE_NOT_NULL(Value.m_pStyle);
	ENSURE_KIND(Value.m_pStyle, WizOpStyle);

	WizOpStyle& TrueStyle = (WizOpStyle&)*Value.m_pStyle;
	StyleReferenceAttribute* pNodeCopy = new StyleReferenceAttribute(TrueStyle);

	ERRORIF(pNodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	CopyNodeContents(pNodeCopy);

	return pNodeCopy;
}


/********************************************************************************************

>	virtual UINT32 StyleReferenceAttribute::GetAttrNameID()

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Provides an attribute name for undo, etc.

	Returns:	A resource id for "Wizard Style Reference"

********************************************************************************************/
UINT32 StyleReferenceAttribute::GetAttrNameID()
{
	return _R(IDS_WIZOP_STYLEREF);
}


/********************************************************************************************

>	virtual BOOL StyleReferenceAttribute::CopyComponentData(BaseDocument* pSrcDoc, 
															BaseDocument* pTargetDoc)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Ensures that the style to which this refers is in the target document.

	Inputs:		pSrcDoc:   The document from where this node was copied
				pTargetDoc: The document where this node lives 

	Returns:	FALSE if unable to copy data

	SeeAlso:	NodeAttribute::CopyComponentData

********************************************************************************************/
BOOL StyleReferenceAttribute::CopyComponentData(BaseDocument* pSourceDoc, 
												BaseDocument* pTargetDoc)
{
	ENSURE_NOT_NULL(Value.m_pStyle);

	ENSURE_NOT_NULL(pSourceDoc);
	ENSURE_NOT_NULL(pTargetDoc);

	// Ask the base class to copy its data
	BOOL Ok = TemplateAttribute::CopyComponentData(pSourceDoc, pTargetDoc);

	// Get the style in our document
	WizOpStyles* pTargetStyles			= NULL;

	if (Ok)
	{
		pTargetStyles = GetStylesForDoc(*pTargetDoc);

		Ok = (pTargetStyles != NULL);
	}

	if (Ok)
	{
		// Attempt to add the style that was in the old (source) document and to which this 
		// StyleReferenceAttribute currently refers, to the new (target) document

		// If the new document already has a style of the same name, but different 
		// characteristics it will return a style with a different name, which we will need
		// to use as our reference.
		ENSURE_KIND(Value.m_pStyle, WizOpStyle);
		const WizOpStyle& SourceStyle = (WizOpStyle&)*Value.m_pStyle;

		WizOpStyle* const pTargetStyle = (WizOpStyle*)pTargetStyles->CopyComponentData(SourceStyle);
		if (pTargetStyle != NULL)
		{
			SetStyle(*pTargetStyle);
		}
		else
		{
			Ok = FALSE;
		}
	}

	return Ok;
}



/********************************************************************************************

>	virtual BOOL StyleReferenceAttribute::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Writes this StyleReferenceAttribute via the given filter.

	Inputs:		pFilter:   The filter to use

	Returns:	FALSE if it failed

	Notes:		this StyleReferenceAttribute attempts to write the referenced style out
				first.

********************************************************************************************/
BOOL StyleReferenceAttribute::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ENSURE_NOT_NULL(Value.m_pStyle);

	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// Must write out the style first
	ENSURE_KIND(Value.m_pStyle, WizOpStyle);
	const WizOpStyle& ActualStyle = (WizOpStyle&)*Value.m_pStyle;

	WizOpStyles* const pStyles = GetStylesForFilter(*pFilter);
	BOOL Ok = (pStyles != NULL);

	XFileRef RecordRef = 0;
	if (Ok)
	{
		RecordRef = pStyles->GetRecordRefForStyle(ActualStyle);

		// Is the style reference ok?
		Ok = (RecordRef != 0);
	}

	if (Ok)
	{
		// Add a description of the TAG_WIZOP_STYLEREF record for older importers
		pFilter->AddTagDescription(TAG_WIZOP_STYLEREF, _R(IDS_TAG_WIZOP_STYLEREF));
	}

	if (Ok)
	{
		CamelotFileRecord Rec(pFilter, TAG_WIZOP_STYLEREF, TAG_WIZOP_STYLEREF_SIZE);

		if (Ok) Ok = Rec.Init();
		if (Ok) Ok = Rec.WriteReference(RecordRef);
		if (Ok) Ok = pFilter->Write(&Rec);
	}

	if (!Ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return Ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual VisibleAttribute* StyleReferenceAttribute::CreateVisibleAttribute() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Creates the user with something to look at in the Wizard Properties dialog

	Returns:	A pointer to a new VisibleStyleReference (or NULL if not)

********************************************************************************************/
VisibleAttribute* StyleReferenceAttribute::CreateVisibleAttribute() const
{
	ENSURE_NOT_NULL(Value.m_pStyle);
	ENSURE_KIND(Value.m_pStyle, WizOpStyle);
	const WizOpStyle& ActualStyle = (WizOpStyle&)*Value.m_pStyle;

	return new VisibleStyleReference(ActualStyle);
}


/********************************************************************************************

>	BOOL StyleReferenceAttribute::SetStyle(const Style& StyleToReference)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	When the component copy does it's stuff it sometimes needs to change the
				reference. This member is for that purpose.

	Inputs:		StyleToReference	The Style to which this StyleReferenceAttribute refers
	Returns:	TRUE

********************************************************************************************/
BOOL StyleReferenceAttribute::SetStyle(const Style& StyleToReference)
{
	Value.m_pStyle = &StyleToReference;
	return TRUE;
}


/********************************************************************************************

>	const StringBase& StyleReferenceAttribute::GetStyleName() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Support function to get the name of the referred style.

	Returns:	A string for the referred style's name.

********************************************************************************************/
const StringBase& StyleReferenceAttribute::GetStyleName() const
{
	STRING_ENSURE_NOT_NULL(Value.m_pStyle);
	return Value.m_pStyle->GetName();
}


/********************************************************************************************

>	WizOpStyles* StyleReferenceAttribute::GetStylesForFilter(BaseCamelotFilter& Filter) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Support function to get the styles for the exporting document

	Returns:	Some WizOpStyles (or NULL)

********************************************************************************************/
WizOpStyles* StyleReferenceAttribute::GetStylesForFilter(BaseCamelotFilter& Filter) const
{
	WizOpStyles* pStyles = NULL;

	BaseDocument* const pDoc = Filter.GetDocument();
	if (pDoc != NULL)
	{
		pStyles = GetStylesForDoc(*pDoc);
	}

	return pStyles;
}


/********************************************************************************************

>	WizOpStyles* StyleReferenceAttribute::GetStylesForFilter(BaseCamelotFilter& Filter) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97

	Purpose:	Support function to get the styles for the given document

	Returns:	Some WizOpStyles (or NULL)

********************************************************************************************/
WizOpStyles* StyleReferenceAttribute::GetStylesForDoc(BaseDocument& Doc) const
{
	return (WizOpStyles*)Doc.GetDocComponent( CC_RUNTIME_CLASS(WizOpStyles) );
}



//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------



/********************************************************************************************

>	virtual UINT32* WizOpStyleRefRecordHandler::GetTagList()

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Provides the record handler system with a list of records handled by this
				handler

	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END

	Notes:		Don't ever extend this. Derive a class from it. See GeneralAttrRecordHandler
				for how not to do it. 

********************************************************************************************/
UINT32* WizOpStyleRefRecordHandler::GetTagList()
{
	static const UINT32 TagList[] = { TAG_WIZOP_STYLEREF, CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}


/********************************************************************************************

>	virtual BOOL WizOpStyleRefRecordHandler::Init(BaseCamelotFilter* pFilter)

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/97

	Purpose:	Initialises the WizOpStyleRecordHandler

	Returns:	TRUE if ok
				FALSE otherwise

********************************************************************************************/
BOOL WizOpStyleRefRecordHandler::Init(BaseCamelotFilter* pFilter)
{
	BOOL Ok = TRUE;

	Ok = CamelotRecordHandler::Init(pFilter);

	Document* const pDoc = GetDocument();
	Ok = (pDoc != NULL);

	if (Ok)
	{
		m_pStyleComponent = (WizOpStyles*)pDoc->GetDocComponent( CC_RUNTIME_CLASS(WizOpStyles) );
		Ok = (m_pStyleComponent != NULL);
	}

	return Ok;
}


/********************************************************************************************

>	virtual BOOL WizOpStyleRefRecordHandler::HandleRecord(CXaraFileRecord* pRecord)

 	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Handles the given TAG_WIZOP record.

	Inputs:		pCXaraFileRecord = ptr to record to handle

	Returns:	TRUE if handled successfuly
				FALSE otherwise

	Errors:		If pCXaraFileRecord isn't a TAG_WIZOP record

********************************************************************************************/
BOOL WizOpStyleRefRecordHandler::HandleRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pRecord->GetTag() != TAG_WIZOP_STYLEREF, FALSE, "I don't handle this tag type");

	XFileRef			StyleRecordRef;

	BOOL				Ok = pRecord->ReadINT32(&StyleRecordRef);
	if (Ok)
	{
		WizOpStyle* const pStyle = GetImportedStyles().FindStyleFromRecordReference(StyleRecordRef);
		if (pStyle != NULL)
		{
			// We managed to read it all OK so add a new StyleRefAttribute to the tree
		 	StyleReferenceAttribute* const pNewAttribute = new StyleReferenceAttribute(*pStyle);
		
			if (pNewAttribute != NULL)
			{
				Ok = InsertNode(pNewAttribute);
			}
			else
			{
				Ok = FALSE;
			}
		}
		else
		{
			BaseCamelotFilter* const pFilter = GetBaseCamelotFilter();
			ENSURE_NOT_NULL(pFilter);
			pFilter->AppendWarning(_R(IDS_STYLE_NOT_FOUND));
		}
	}

	return Ok;
}


/********************************************************************************************

>	WizOpStyles& WizOpStyleRefRecordHandler::GetImportedStyles() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Data access

	Errors:		Will ERROR2 if the Init function failed

********************************************************************************************/
WizOpStyles& WizOpStyleRefRecordHandler::GetImportedStyles() const
{
	// This static dummy object is used for returning in the ERROR2 case
	// The original used to return a ptr to a local variable, which is a tad dangerous.
	// This solution is not ideal, because there's a permanent instance of an object
	// that will probably never be used.
	static WizOpStyles Dummy;

	ERROR2IF(m_pStyleComponent == NULL, Dummy, "NULL Member");

	return *m_pStyleComponent;
}

/********************************************************************************************

>	virtual void WizOpStyleRefRecordHandler::GetRecordDescriptionText(
											CXaraFileRecord* pRecord, StringBase* pStr)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Debug function that provides descriptions for TemplateAttribute (WizOp) records.

	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update

********************************************************************************************/
#if XAR_TREE_DIALOG
void WizOpStyleRefRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
	{
		TRACE( _T("TemplateAttrRecordHandler::GetRecordDescriptionText - NULL Args\n"));
		return;
	}
	if (pRecord->GetTag() != TAG_WIZOP_STYLEREF)
	{
		TRACE( _T("TemplateAttrRecordHandler::GetRecordDescriptionText - Incorrect record\n"));
		return;
	}

	//	 Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord, pStr);

	XFileRef StyleRecordRef;

	BOOL Ok = pRecord->ReadINT32(&StyleRecordRef);

	WizOpStyle* pStyle = NULL;

	if (Ok)
	{
		*pStr += TEXT("Record Reference  = ");

		String_16 RecordRefString;
		Ok = Convert::LongToString(StyleRecordRef, &RecordRefString);
		if (Ok)
		{
			*pStr += RecordRefString;
		}

	}
}


/********************************************************************************************

>	virtual void WizOpStyleRefRecordHandler::GetTagText(UINT32 Tag,String_256& Str)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	A debug function that gives the tag description.

	Inputs:		Tag	= the tag value
	Outputs:	Str = place to put textual description of the tag

********************************************************************************************/
void WizOpStyleRefRecordHandler::GetTagText(UINT32 Tag,String_256& Str)
{
	Str = TEXT("TAG_WIZOP_STYLEREF");
}
#endif	// _DEBUG





// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //
//			V  I  S  I  B  L  E  S  T  Y  L  E  R  E  F  E  R  E  N  C  E
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //







/********************************************************************************************

>	VisibleStyleReference::VisibleStyleReference(const WizOpStyle& Style)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Sets up a VisibleStyleReference so it actually references a
				style and so we can play with it.

	Inputs:		Style:	The style that you want to show the user.

********************************************************************************************/
VisibleStyleReference::VisibleStyleReference(const WizOpStyle& Style) :
	m_Style(Style)
{
	m_Question = m_Style.GetQuestion();
}


/********************************************************************************************

>	StringBase& VisibleStyleReference::GetText(StringBase& Description) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Gives the VisibleListItem its text to display

	Outputs:	Description		Where the text should go
	Returns:	A reference to the given StringBase

********************************************************************************************/
StringBase& VisibleStyleReference::GetText(StringBase& Description) const
{
	return (Description = m_Style.GetName());
}



/********************************************************************************************

>	virtual BOOL VisibleStyleReference::Display(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This displays the StyleReference to the user.
	
	Notes:		This function takes a DialopOp as its argument to determine where to
				put this object. Since this requires an internal knowledge of the DialogOp
				at the moment it is cast to a TemplateDialog which has the fields all ready.
				This is likely to change in the future.

********************************************************************************************/
BOOL VisibleStyleReference::Display(DialogOp& Dialog)
{
	BOOL				Ok = TRUE;

	PORTNOTETRACE("other","TemplateAttribute::Display - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(!((&Dialog)->IS_KIND_OF(TemplateDialog)), "Dialog isn't kind of TemplateDialog");

	TemplateDialog& BetterBeThisDialog = (TemplateDialog&)Dialog;

	Ok = m_Question.Display(BetterBeThisDialog.GetValueControl());

	if (Ok)
	{
		SetParamHintString(GetStyle().GetWizOp().GetParamHint());
		// bodgy bit this...
		Ok = GetParamHintLabel().Display(Dialog);
	}

	if (Ok)
	{
		SetParamText(GetStyle().GetParam());
		// bodgy bit this...
		Ok = GetParamText().Display(BetterBeThisDialog.GetKeyControl());
	}
#endif
	return Ok;
}


/********************************************************************************************

>	virtual BOOL VisibleStyleReference::Interact(DialogOp& Dialog)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	This allows the user to change this VisibleStyleReference.
				Except it does nothing.
	

********************************************************************************************/
BOOL VisibleStyleReference::Interact(DialogOp& Dialog)
{
	return TRUE;
}


/********************************************************************************************

>	virtual void VisibleStyleReference::Hide()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/97
	
	Purpose:	Hides any interaction with the user

********************************************************************************************/
void VisibleStyleReference::Hide()
{
	m_Question.Hide();
	GetParamHintLabel().Hide();
	GetParamText().Hide();
}


/********************************************************************************************

>	virtual NodeAttribute* VisibleStyleReference::CreateNewAttribute()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Creates a new StyleReferenceAttribute that this VisibleStyleReference
				represents to put in the tree.

	Returns:	A pointer to the new StyleReferenceAttribute (or NULL)

********************************************************************************************/
NodeAttribute* VisibleStyleReference::CreateNewAttribute()
{
	return new StyleReferenceAttribute(m_Style);
}




void VisibleStyleReference::SetParamHintString(const StringBase& HintToShow)
{
	m_ParamHintLabel = HintToShow;
}


const WizOpStyle& VisibleStyleReference::GetStyle()
{
	return m_Style;
}



VisibleLabel& VisibleStyleReference::GetParamHintLabel()
{
	return m_ParamHintLabel;
}



EditableText& VisibleStyleReference::GetParamText()
{
	return m_Param;
}



void VisibleStyleReference::SetParamText(const StringBase& ParamToShow)
{
	m_Param = ParamToShow;
}
