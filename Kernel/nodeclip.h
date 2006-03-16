// $Id: nodeclip.h 662 2006-03-14 21:31:49Z alex $
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
// Declaration of NodeClipView.
//

#ifndef INC_NODECLIP
#define INC_NODECLIP

#include "node.h"			// derivation from NodeRenderableInk.
#include "clipattr.h"		// for our ClipRegionAttribute member.

/********************************************************************************************

>	class NodeClipView : public NodeRenderableInk

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10 February 2000
	Purpose:	ClipView node.
				This node works in tandem with NodeClipViewController to provide clipview 
				functionality. It sits underneath a NodeClipViewController, between the 
				keyhole node and the clipped nodes, so that it is rendered after the keyhole 
				node. Its Render() method saves GDraw's current clipping region and applies a 
				new region, derived from the keyhole node's outline.
	See also:	

********************************************************************************************/
class NodeClipView : public NodeRenderableInk
{
// Give my name out in memory dumps.
CC_DECLARE_DYNCREATE(NodeClipView);

// we don't mind NodeClipViewController accessing our protected methods.
friend class NodeClipViewController;
//friend class ClipRegionAttribute;

public:
	// construction/destruction and initialisation.
	NodeClipView();
	NodeClipView(	Node* pContextNode,  
					AttachNodeDirection Direction,
					BOOL Locked		= FALSE,
					BOOL Mangled	= FALSE,
					BOOL Marked		= FALSE,
					BOOL Selected	= FALSE );
	~NodeClipView();

	// virtual ID function.
	virtual BOOL IsANodeClipView() const { return TRUE; }

	// bounding information.
	virtual DocRect GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE);
	virtual DocRect GetBlobBoundingRect();

	// render this node.
	virtual SubtreeRenderState RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
	virtual void Render(RenderRegion* pRender);
	void RenderClipAttr(RenderRegion* pRender);
	virtual void RenderEorDrag( RenderRegion* pRender );

	// we don't want to ever have any children, so we reject all attributes out of hand.
	virtual BOOL RequiresAttrib(CCRuntimeClass * , BOOL )		{ return FALSE; }
	virtual BOOL RequiresAttrib(NodeAttribute * , BOOL )		{ return FALSE; }
	virtual BOOL CanAttrBeAppliedToMe(CCRuntimeClass* AttrType)	{ return FALSE; }

	// return a description, for use in menus, infobar etc.
	virtual String Describe(BOOL Plural, BOOL Verbose);

	// get the size of this node.
	virtual UINT32 GetNodeSize() const;

	// transform this node.
	void Transform(TransformBase &Trans);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	// called on nodes after a document is imported.
	virtual BOOL PostImport();

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

protected:
	/////////////////////////////////////////////////////////////////////////////////////////
	//
	// ClipView-specific methods.
	//

	// set GDraw'c clipping region to, and restore it back over, the path of our keyhole node.
	void SetGDrawClippingRegion(RenderRegion* pRender);
	BOOL RestoreGDrawClippingRegion(BOOL fFirstInRenderLoop = FALSE);

	// this allows our NCVC to give us a path to clip to.
	void SetClipPath(Path* pClipPath) { m_ClipRegionAttribute.SetClipPath(pClipPath); }

	// copy methods. (Q: is there a reason why CopyNodeContents() is not virtual?)
	// Phil 19/12/2003 A: Yes. Because it takes a typed input parameter
	//						However, I have just added PolyCopyNodeContents which takes a NodeRenderable*
	virtual Node* SimpleCopy();
	void CopyNodeContents(NodeClipView* pNodeCopy);

/////////////////////////////////////////////////////////////////////////////////////////////
//
// Member variables
//

private:
	BOOL				m_bRenderingForward;
	BYTE *				m_pRegion;
	GCONTEXT *			m_pContext;
	CONST REGION *		m_pSavedRegion;
	BOOL				m_bGDrawClipRegionSet;
	RECT				m_SavedClipRect;

private:
	ClipRegionAttribute		m_ClipRegionAttribute;

};



#endif	// INC_NODECLIP
