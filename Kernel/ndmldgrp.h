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
// The NodeMouldGroup renderable class

#ifndef INC_NODEMOULDGROUP
#define INC_NODEMOULDGROUP


#include "ink.h"
#include "rndrgn.h"
//#include "selop.h"
#include "moldshap.h"
#include "group.h"
#include "becomea.h"

#define OPTOKEN_NODEMOULDGROUP _T("MouldGroupObject")

class ExtendParams;


/***********************************************************************************************

>	Union MouldedObjectUnion
	enum MouldedTypeEnum

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Purpose:	These structures describe the type of object held within the mould list item

***********************************************************************************************/

enum MouldedType {
	MOULD_UNDEFINED,
	MOULD_PATH,
	MOULD_BITMAP
};




/***********************************************************************************************

>	class NodeMouldGroup : public NodeGroup

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Purpose:	defines the structure of a mould group object. This object lives inside a 
				NodeMould and keeps track of all mould source objects. 

***********************************************************************************************/

class NodeMouldGroup: public NodeGroup
{

	CC_DECLARE_DYNAMIC( NodeMouldGroup );    

	public: 
		NodeMouldGroup(); 
		NodeMouldGroup(Node* ContextNode,  
				  AttachNodeDirection Direction,    
				  BOOL Locked=FALSE, 
				  BOOL Mangled=FALSE,  
				  BOOL Marked=FALSE, 
				  BOOL Selected=FALSE    
			     ); 

		~NodeMouldGroup();

		virtual void 	PreExportRender(RenderRegion* pRender);
		virtual BOOL 	ExportRender(RenderRegion* pRender);
		virtual SubtreeRenderState RenderSubtree(RenderRegion *pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
		virtual BOOL 	NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly = FALSE,
									  BOOL CheckSelected = FALSE);

		// eor-drag rendering functions.
		// we do none, so they both do very little :o)
		virtual void RenderEorDrag(RenderRegion* pRender) {}
		virtual void RenderEorDragChildren(RenderRegion* pRender) {}

		// We don't want the hidden ink-nodes within a NodeMouldGroup to be accounted for
		// when the bounds for a drag are calculated.
		virtual DocRect GetEorDragBoundingRect() { return DocRect(0, 0, 0, 0); }

		virtual DocRect GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE);
		virtual DocRect GetBlobBoundingRect();
				DocRect GetChildrensBounds(BOOL DontUseAttrs = FALSE);
		virtual void 	Transform(TransformBase&);
		virtual BOOL ChildrenAreEorDragRenderedByMe() { return TRUE; }

		// Other virtual functions
		virtual String Describe(BOOL Plural, BOOL Verbose); 
		virtual Node* SimpleCopy();  
	    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes
		virtual BOOL ShouldBeRendered() const;

		// Functions to deal with messages
		virtual BOOL OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
		virtual BOOL HidingNode();
		virtual BOOL ShowingNode();

		// Fuctions that allows ops and other nodes to inform this node of events on it's children
		virtual ChangeCode OnChildChange(ObjChangeParam* pParam);

		// Version 2 file format functions
		virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
		virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	// Karim MacDonald 25/11/1999
	// Extend functions.
	// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
	// also provide complete implementations of ValidateExtend and Extend,
	// which should usually check/extend its children.
	// A MouldGroup node acts like a hidden node, as Mould extension is handled solely
	// by the relevant NodeMouldPath.
	public:
		virtual BOOL IsTypeExtendible() const { return FALSE; }
		virtual DocRect ValidateExtend(const ExtendParams& ExtParams)
		{
			return DocRect(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);
		}
		virtual void Extend(const ExtendParams& ExtParams) {}

		virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

//	virtual BOOL IsValidEffectAttr(CCRuntimeClass* pAttrClass) const {return FALSE;}
	virtual BOOL IsValidEffectAttr(NodeAttribute* pAttr) const {return FALSE;}

	private:
		void CopyNodeContents(NodeMouldGroup* pCopyOfNode);

		// CAM/AW EPS output functions
		void PreExportNATEPS(RenderRegion* pRegion);
		BOOL PostExportNATEPS(RenderRegion* pRegion);
		void PreExportAWEPS(RenderRegion* pRegion);
		BOOL PostExportAWEPS(RenderRegion* pRegion);

	private:
		String_32 MouldGroupName; // The name of the mould group

};


#endif
