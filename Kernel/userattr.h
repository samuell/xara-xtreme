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

#ifndef INC_USERATTR
#define INC_USERATTR

class BlendAttrParam;
class BaseCamelotFilter;
class Style;

/********************************************************************************************

>	class UserAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
					(Actually, it was more likely to be Graham, cos Tim quit a few months before)

	Created:	15/06/94
					(More like mid '96 if you ask me)

	Purpose:	Represent a line width of a path.  This may be a simple stroking operator
				such as a constant line width, or a complex one such as variable line
				widths or an airbrush stroke operator.
					(I'd say it stores a "key" string and an associated "value" string
					for implementation of URLs and other such stuff, but if you say it
					stores a line width, who am I to argue? ;-)

********************************************************************************************/

class UserAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(UserAttribute)
public:
	static BOOL Init();

	UserAttribute();
	UserAttribute& operator=(const UserAttribute& other);

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual NodeAttribute *MakeNode();
	BOOL IsDifferent(AttributeValue *pAttr);
	virtual void SimpleCopy(AttributeValue *);

	// This blend the attribute value with the one provided by pBlendParam
	virtual BOOL Blend(BlendAttrParam* pBlendParam);

	// Extra virtual function for stroke type attributes.
	virtual BOOL RenderStroke(RenderRegion *, Path *) { return FALSE; }

	StringBase	 Key;
	StringBase	 LongKey;
	StringBase	 Value;
	const Style* m_pStyle;		// used by StyleReferenceAttribute
};


/***********************************************************************************************

>	class AttrUser : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93

	Purpose:	User Attribute Node

***********************************************************************************************/

class AttrUser : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrUser)

public:
	AttrUser(); 
	AttrUser(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 
	AttrUser(const StringBase& NewKey, const StringBase& NewSubKey, const StringBase& NewValue);
	AttrUser(const StringBase& NewLongKey, const StringBase& NewValue);

	virtual void Render( RenderRegion* pRender );

	virtual Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 
	virtual UINT32 GetAttrNameID(void); 
	virtual BOOL IsUserAttribute() const		{ return TRUE; }

	void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void Transform( TransformBase& );

	// Yes, user attributes can be applied more than once to the same object
	virtual BOOL CanBeMultiplyApplied()		{ return TRUE; }
	virtual BOOL ShouldBeOptimized();
	virtual BOOL EffectsParentBounds()		{ return TRUE; }

	virtual BOOL Blend(BlendAttrParam* pBlendParam);

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrUser); }
	virtual AttributeIdentifier	GetAttributeClassID() const;
	virtual AttrIndex GetAttributeIndex () { return ATTR_USERATTRIBUTE; }

	virtual VisibleAttribute* CreateVisibleAttribute() const;

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	BOOL ShouldBecomeCurrent();

	StringBase& GetWebAddress();

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

protected:
	void CopyNodeContents( AttrUser* NodeCopy );

	static const TCHAR s_KeyDelimiter;

public:
	UserAttribute Value;
};



#endif // INC_USERATTR
