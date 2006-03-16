// $Id: ngsentry.h 662 2006-03-14 21:31:49Z alex $
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
	$Header: /Camelot/kernel/ngsentry.h 13    7/01/04 16:47 Phil $
	Special sentinel node that has all existing set names applied to it.
*/

#ifndef NGSENTRY_H
#define NGSENTRY_H

#include "node.h"
#include "ngprop.h"
#include "opbarcreation.h"
#include <vector>

class BaseCamelotFilter;
class TemplateAttribute;


/**********************************************************************************************
>	class CCAPI NodeSetProperty : public Node

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/99
	Purpose:	Child node of NodeSetSentinel that stores individual property records
				for a set of objects (SGNameItem).
	SeeAlso:	SGNameProp; SGNameItem; Node; NodeSetSentinel; NameGallery
**********************************************************************************************/

class CCAPI NodeSetProperty : public Node
{
	CC_DECLARE_DYNAMIC(NodeSetProperty);
public:
	// Creation & destruction.
	NodeSetProperty();
	NodeSetProperty(const StringBase& strName);
	virtual ~NodeSetProperty();

	// The name of the set these properties are for.
	const StringBase& GetName() const
			{ return m_strName; }

	// Get and set the given property held by this node, returning the old one.
	virtual SGNameProp* SetProperty(SGNameProp* pNewProp);
	SGNameProp* GetProperty(INT32 nIndex) const
			{ return m_pProp[nIndex]; }

	// Create default properties of all types for the named set.
	BOOL CreateDefaults();

	// Copy the properties of the given NodeSetProperty into this.
	BOOL CopyProperties(NodeSetProperty* pOther);

	// Node copying.
	virtual Node* SimpleCopy();
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

protected:
	virtual UINT32 GetNodeSize() const;
	void CopyNodeContents(NodeSetProperty* pCopy);

	// Version 2 native streaming.  
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter*);

	// Version 2 web streaming.
	virtual BOOL CanWriteChildrenWeb(BaseCamelotFilter*);
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter*);
	virtual BOOL WriteBeginChildRecordsWeb(BaseCamelotFilter*);
	virtual BOOL WriteEndChildRecordsWeb(BaseCamelotFilter*);

private:
	// Data.
	StringBase	m_strName;
	SGNameProp* m_pProp[SGNameProp::nPropertyCount];	// properties of the named set

public:
	// Debugging.
	BOOL m_Imported;
#if DEBUG_TREE	
	virtual void GetDebugDetails(StringBase* pStr);
	virtual void ShowDebugTreeDetails() const;
#endif
};



/**********************************************************************************************
>	class CCAPI NodeBarProperty : public Node

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/99
	Purpose:	Child node of NodeSetSentinel that stores individual property records
				for a bars.
	SeeAlso:	SGNameProp; SGNameItem; Node; NodeSetSentinel; NameGallery
**********************************************************************************************/

class CCAPI NodeBarProperty : public Node
{
	CC_DECLARE_DYNAMIC(NodeBarProperty);
public:
	// Creation & destruction.
	NodeBarProperty();
	NodeBarProperty(Node* pContext, AttachNodeDirection eDir);

	// Return how many entries in the array and allow it to be reinitialised.
	size_t HowMany() const						{ return m_nCount; }
	void Clear()								{ m_nCount = 0; }

	// Access the array of bar properties (NB. no range checks on these.)
	BarDataType& Bar(INT32 n);					//{ return m_vecBars[n]; }
	const BarDataType& Bar(INT32 n) const;		//{ return m_vecBars[n]; }

	// Add a new bar property to the end of the array, returning its new index, or
	// UINT_MAX for an overflow condition.
	size_t Add(const BarDataType& bdt);

	// Changes entry n to be the new bar property in an undoable manner.
	BOOL MakeChange(INT32 n, const BarDataType& bdt);

	// Node copying.
	virtual Node* SimpleCopy();
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

protected:
	virtual UINT32 GetNodeSize() const;
	void CopyNodeContents(NodeBarProperty* pCopy);

	// Version 2 native streaming.  
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter*);

	// Version 2 web streaming.
	virtual BOOL CanWriteChildrenWeb(BaseCamelotFilter*);
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter*);
	virtual BOOL WriteBeginChildRecordsWeb(BaseCamelotFilter*);
	virtual BOOL WriteEndChildRecordsWeb(BaseCamelotFilter*);

private:
	// Data.
	std::vector<BarDataType> m_vecBars;			// indexable array of bar properties
	size_t m_nCount;							// how many properties are in the array
	BarDataType m_NoBar;

public:
	// Debugging.
#if DEBUG_TREE	
	virtual void GetDebugDetails(StringBase* pStr);
	virtual void ShowDebugTreeDetails() const;
#endif
};


// The maximum number of bars the user may have.
const size_t MAX_BAR_PROPERTIES = 100;



/**********************************************************************************************
>	class CCAPI NodeSetSentinel : public Node

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/99
	Purpose:	Dummy node that has one instance of each different Wix ObjectName attribute 
				also applied to it.  Every document has one of these nodes after the last
				default attribute and before the first chapter.  Whenever a new name
				is first applied to any objects in the document, it is also applied
				to this node; similarly, whenever a name is deleted (ie. hidden) from
				a document, it is also deleted from this node.  Hence, during the normal
				course of creating or deleting objects with particular names, even if the
				last object which has a name is deleted by the user, the name still exists
				as a single attribute applied to this node, will remain an active item
				in the Attribute gallery, and can be saved out in the native format.  And
				when the user creates or deletes names that never refer to any objects,
				the action always result in tree manipulations and are properly integrated
				into the undo system.
	SeeAlso:	Node; NodeSetProperty; NameGallery
**********************************************************************************************/

class CCAPI NodeSetSentinel : public Node
{
	CC_DECLARE_DYNAMIC(NodeSetSentinel);
public:
	// Creation & destruction.
	NodeSetSentinel();
	NodeSetSentinel(Node* pContext, AttachNodeDirection eDir);

	// Set names.
	virtual BOOL IsSetCandidate() const;
	TemplateAttribute* GetNameAttr(const StringBase& strName) const;

	// Set properties.
	NodeSetProperty* FindPropertyNode(const StringBase& strName) const;
	NodeSetProperty* CreatePropertyNode(const StringBase& strName);

	NodeBarProperty* FindBarProperty();

	BOOL TargetsExist() const;

	// Fix-ups of loaded/imported Wix ObjectName attributes.
	BOOL OnLoadName(BaseCamelotFilter* pFilter, TemplateAttribute* pImportedName);

	// virtual override of Node::AllowOp, to prevent AllowOp calls
	// travelling up the tree hierarchy.
	virtual BOOL AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
												 BOOL DoPreTriggerEdit);
protected:
	// Version 2 native streaming.  Children written automatically (the default).
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter*);

	// Version 2 web streaming.  These overrides prevent the node or its children
	// from being written to web files, so as to exclude information used strictly
	// for editing, ie. unreferenced set names and set properties.
	virtual BOOL CanWriteChildrenWeb(BaseCamelotFilter*);
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter*);
	virtual BOOL WriteBeginChildRecordsWeb(BaseCamelotFilter*);
	virtual BOOL WriteEndChildRecordsWeb(BaseCamelotFilter*);

	// Copying.
	virtual UINT32 GetNodeSize() const;
	virtual Node* SimpleCopy();

public:
	// Debugging.
#if DEBUG_TREE	
	virtual void GetDebugDetails(StringBase* pStr);
	virtual void ShowDebugTreeDetails() const;
#endif
};

#endif	/* !NGSENTRY_H */
