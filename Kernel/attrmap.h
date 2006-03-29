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
// Created by Markn 11/1/96
// Defines a class that will encapsulate lists of attributes.
//
// NOTE: It is derived from the MFC class CMapPtrToPtr.  This breaks rules to do with winoil/kernel cross-over
// but it doesn't make the old system any worse, which insidently, uses CMapPtrToPtr directly.
// This new class will allow common functions to be put in one place, and will also aid reimplementation
// if this becomes necessary.

#ifndef INC_ATTRMAP
#define INC_ATTRMAP

//#include "rndrgn.h"

class NodeRenderableInk;
class TransformBase;
class RenderRegion;


/********************************************************************************************

>	class CCAttrMap: public CMapPtrToPtr

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Purpose:	Class that encapsulates lists of attributes in Camelot

********************************************************************************************/

class CCAttrMap : public CCObject
{
	CC_DECLARE_DYNAMIC(CCAttrMap)

public:
	CCAttrMap(size_t nBlockSize = 10) { attrMapCreator = NULL; }

	// CMapPtrToPtr emulation
	typedef CMapPtrToPtr::iterator	iterator;
	inline iterator GetStartPosition()
	{
		return m_mapAttr.begin();
	}
	inline iterator GetEndPosition()
	{
		return m_mapAttr.end();
	}

	inline void GetNextAssoc( iterator &iter, CCRuntimeClass *&pClass, void *&pValue ) const
	{
		pClass	= (CCRuntimeClass *)iter->first;
		pValue	= iter->second;
		++iter;
	}

	inline bool Lookup( CCRuntimeClass *pKey, void *&pVal )
	{
		iterator	iter = m_mapAttr.find( pKey );
		if( m_mapAttr.end() == iter )
			return false;

		pVal = iter->second;
		return true;
	}

	bool RemoveKey( CCRuntimeClass *pKey )
	{
		iterator	iter = m_mapAttr.find( pKey );
		if( m_mapAttr.end() == iter )
			return false;

		m_mapAttr.erase( iter );
		return true;
	}

	void SetAt( CCRuntimeClass *pKey, void *pValue )
	{
		m_mapAttr[pKey] = pValue;
	}

	size_t GetCount() const
	{
		return m_mapAttr.size();
	}

	void RemoveAll()
	{
		return m_mapAttr.clear();
	}

	void DeleteAttributes();
	CCAttrMap* Copy();

	static CCAttrMap* MakeAppliedAttrMap(NodeRenderableInk* pInkNode,
										 BOOL ExcludeIndirectlyAppliedGLAs = TRUE,
										 BOOL bStrictEffectStatus = TRUE);

	// DMc 9/8/99
	void Render(RenderRegion* pRegion, BOOL RenderOffscreenAttributes = TRUE);

	void BuildListOfAttributes(List* pList);

	void Transform(TransformBase& Trans);

	// find me the attribute of type Class please (and return it)
	NodeAttribute* FindAttribute (NodeAttribute* pTypeAttr);

	// replaces an attribute reference in the map
	// returns the old attribute
	NodeAttribute* ReplaceAttribute(NodeAttribute* pNewAttr);

	// Removes and deletes 
	void RemoveAttribute(CCRuntimeClass* pAttrClass);

	//transformation function used for blends on paths and brushes
	void TransformForBrush(TransformBase& Trans);

	// special fn. for brushes - transforms the attributes not transformed in TransformForBrush
	void TransformBrushFills(TransformBase& Trans);

	BOOL WillScaleLineWidthToZero(TransformBase& Trans);

	// makes an attribute map from a render region
	static CCAttrMap* MakeAttrMapFromRenderRegion(RenderRegion* pRegion);

	// applies all attributes in the map to the node
	// NOTE doesn't normalise !
	void ApplyAttributesToNode(NodeRenderableInk* pNode);

	// Ilan 4/5/00
	void PostBlendInit(Path* pPath, CCRuntimeClass* pCreatorClass);
	void PostBlendDeinit();
	void AttachGeometryLinkedAttrs(Node* pContext);
	void RemoveGeometryLinkedAttrs();

	BOOL IsSeeThrough();

	Node* attrMapCreator;			// (test code):  whoever created the attrmap

private:
	CMapPtrToPtr		m_mapAttr;
};

#endif  // INC_ATTRMAP
