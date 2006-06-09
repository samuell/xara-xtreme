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
// Declaration of ClipRegionAttribute.
//

#ifndef INC_CLIPATTR
#define INC_CLIPATTR

//#include "attrval.h"	// derivation from AttributeValue. - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h"		// for Path member variable. - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "clipres.h"

/********************************************************************************************

>	class ClipRegionAttribute : public AttributeValue

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20 March 2000
	Purpose:	

	See also:	

********************************************************************************************/
class ClipRegionAttribute : public AttributeValue
{
// Runtime class information.
CC_DECLARE_DYNCREATE(ClipRegionAttribute)

public:
	// construction/destruction
	ClipRegionAttribute();
	~ClipRegionAttribute();

	// methods required for AttributeValue implementation
	virtual void Render(RenderRegion* pRender, BOOL Temp = FALSE);
	virtual void Restore(RenderRegion* pRender, BOOL Temp);
	virtual void GoingOutOfScope(RenderRegion* pRender);
	virtual void SimpleCopy(AttributeValue* pSource);
	virtual NodeAttribute* MakeNode();
	virtual BOOL IsDifferent(AttributeValue* pOther);

	// static initialisation function, to register this attribute with Camelot.
	static BOOL Init();

	// member access functions.
	Path*	GetClipPath() const { return m_pClipPath; }
	void	SetClipPath(Path* pClipPath) { m_pClipPath = pClipPath; }

//	not really sure what this is for...
//	virtual INT32 operator=(AttributeValue& Attrib) { return TRUE; }

	void	SetResponsibleForGrouping  (BOOL starts) { m_bResponsibleForGrouping = starts; }
	BOOL	IsResponsibleForGrouping () { return m_bResponsibleForGrouping; }

// member variables.
private:
	// the path which this attribute should cause output to be clipped to.
	Path*	m_pClipPath;
	BOOL	m_bResponsibleForGrouping;
};

class AttrClipView : public NodeAttribute
{
CC_DECLARE_DYNAMIC(AttrClipView)

public:
	AttrClipView();
	~AttrClipView();
	AttrClipView(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE);

	// Traditional Node functionality
	virtual void Render( RenderRegion* pRender );

#ifdef _DEBUG
	void GetDebugDetails(StringBase* Str);
#endif

	// Traditional NodeAttribute functionality
	virtual AttributeValue* GetAttributeValue() { return &Value; }
	Node* SimpleCopy();
	virtual UINT32 GetAttrNameID() { return _R(IDS_CLIPVIEW_ATTR_ID); }
	virtual AttrIndex GetAttributeIndex () { return ATTR_CLIPVIEW; }
	virtual UINT32 GetNodeSize() { return sizeof(AttrClipView); }
	virtual INT32 operator==(const NodeAttribute& NodeAttrib);

	// Node charateristics tests
	virtual BOOL NeedsTransparency() const { return FALSE; }

	// prevent feathering in incompatible circumstances
	virtual SubtreeRenderState RenderSubtree(RenderRegion *pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE) {return SUBTREE_ROOTONLY; }
	virtual BOOL IsAClipViewAttr() const { return TRUE; }

	// support layering and remove redundant attrs
	virtual BOOL CanBeMultiplyApplied()	{ return TRUE; }
	virtual BOOL ShouldBeOptimized() { return FALSE; }

	// dynamic attribute functions and flags
	virtual void Transform(TransformBase& Trans);
	virtual BOOL Blend(BlendAttrParam* pBlendParam);
	virtual BOOL ContainsAttributeValue(AttributeValue* pVal);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	// New Function local to this attribute only!
	// It`s mainly used for turning the attribute off when exporting AI EPS
	void EnableRendering(BOOL DoEnableRendering = TRUE) { m_DoRender = DoEnableRendering; }
	
	// Small function which lets the caller find out if the attribute is disabled or not.
	BOOL GetRenderStatus() { return m_DoRender; }

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents(NodeAttribute *pCopy);
	BOOL m_DoRender;

public:
	ClipRegionAttribute Value;
};


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



#endif	// INC_CLIPATTR
