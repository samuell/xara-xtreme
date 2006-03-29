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

#ifndef INC_TMPLTATR
#define INC_TMPLTATR

#define ENSURE_NOT_NULL(Pointer)		ERROR2IF(Pointer == NULL, 0, "NULL Args")


extern const String_8 NullString;


class WizOp;
class WizOpStyles;
/***********************************************************************************************

>	class TemplateAttribute : public AttrUser

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/97

	Purpose:	This represents a TemplateOp/Argument/user-changable from the template file
				attribute.
				It will probably be superceded, but for now it's derived from AttrUser with a 
				value of the form <InternalName>[/<Param>][;<Question>]
				Bring on the plug-in attributes I say.

***********************************************************************************************/
class TemplateAttribute : public AttrUser
{
	CC_DECLARE_DYNAMIC(TemplateAttribute)
public:
	TemplateAttribute(	const StringBase& ArgumentName = NullString, 
						const StringBase& Question = NullString,
						const StringBase& Param = NullString); 



// Overridden
	virtual Node* SimpleCopy();
	void CopyNodeContents(TemplateAttribute* pAttr);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);
	virtual UINT32 GetAttrNameID();
	virtual BOOL IsAnObjectName() const;
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual AttributeIdentifier	GetAttributeClassID() const;
	virtual VisibleAttribute* CreateVisibleAttribute() const;

// Data Access
	String_256 GetQuestion() const;
	String_256 GetInternalName() const;
	String_256 GetParam() const;
	StringBase&	GetUserName() const;

	BOOL SetWizOpNameAndParam(const StringBase& Name, const StringBase& Param);

protected:
	BOOL m_fIsAnObjectName;
	static const TCHAR s_ParamDelimiter;
	static const String_8 s_TemplateKey;
};


// Shorthand - we use these a lot in the Name gallery.
#define TA_CLASS	(CC_RUNTIME_CLASS(TemplateAttribute))
#define	TA_NAME		(String(TEXT("ObjectName")))



/***********************************************************************************************

>	class TemplateAttrRecordHandler : public CamelotRecordHandler

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Handles the reading of WizOp records in the v2 file format

***********************************************************************************************/
class TemplateAttrRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(TemplateAttrRecordHandler);

public:
	// functions that must be implemented.
	virtual UINT32*	GetTagList();
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);

#if XAR_TREE_DIALOG
	virtual void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord,StringBase* Str);
	virtual void GetTagText(UINT32 Tag,String_256& Str);
#endif

private:
	// nothing here but us mice
};

class WizOpStyle;
/***********************************************************************************************

>	class StyleReferenceAttribute : public TemplateAttribute

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/97

	Purpose:	This represents a TemplateOp/Argument/user-changable from the template file
				attribute.
				It will probably be superceded, but for now it's derived from AttrUser with a 
				value of the form <InternalName>[/<Param>][;<Question>]
				Bring on the plug-in attributes I say.

***********************************************************************************************/
class StyleReferenceAttribute : public TemplateAttribute
{
	CC_DECLARE_MEMDUMP(StyleReferenceAttribute)
public:
	StyleReferenceAttribute(const WizOpStyle& Style);

// Overridden
	virtual Node* SimpleCopy();
	virtual UINT32 GetAttrNameID(void); 
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL CopyComponentData(BaseDocument* SrcDoc,  BaseDocument* NodesDoc);

	virtual VisibleAttribute* CreateVisibleAttribute() const;

	const StringBase& GetStyleName() const;

protected:
	// Implementation
	BOOL SetStyle(const Style& StyleToReference);

	WizOpStyles* GetStylesForFilter(BaseCamelotFilter& Filter) const;
	WizOpStyles* GetStylesForDoc(BaseDocument& Doc) const;
//	WizOpStyles* GetStyles() const;
};



/***********************************************************************************************

>	class WizOpStyleRefRecordHandler : public CamelotRecordHandler

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/97

	Purpose:	Handles the reading of WizOpStyleRef records in the v2 file format

***********************************************************************************************/
class WizOpStyleRefRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(WizOpStyleRefRecordHandler);

public:
	WizOpStyleRefRecordHandler() : m_pStyleComponent(NULL) {}
	BOOL Init(BaseCamelotFilter* pFilter);

	// functions that must be implemented.
	virtual UINT32*	GetTagList();
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);

#if XAR_TREE_DIALOG
	virtual void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord,StringBase* Str);
	virtual void GetTagText(UINT32 Tag,String_256& Str);
#endif

protected:
	WizOpStyles&	GetImportedStyles() const;

private:
	WizOpStyles*	m_pStyleComponent;
};

#endif //ifdef INC_USERATTR
