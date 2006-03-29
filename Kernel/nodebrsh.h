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

// Nodebrush class

#ifndef INC_NODEBRSH
#define INC_NODEBRSH

#include "group.h"

class NodeBrushMaker;
class NodeBrushPath;



/********************************************************************************************

>	class NodeBrush: public NodeGroup

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/99
	Purpose:	The NodeBrush class

********************************************************************************************/

class NodeBrush: public NodeGroup
{
	// Give my name out in memory dumps
	CC_DECLARE_DYNCREATE(NodeBrush);

public:
	NodeBrush();
	NodeBrush(Node* ContextNode,  
			  AttachNodeDirection Direction,    
			  BOOL Locked=FALSE, 
			  BOOL Mangled=FALSE,  
			  BOOL Marked=FALSE, 
			  BOOL Selected=FALSE    
		     ); 
	
	~NodeBrush();

	// get the bounding rect 
	virtual DocRect GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE);
	virtual DocRect GetBlobBoundingRect();

	// the main render function
	virtual void Render(RenderRegion* pRender);
	virtual SubtreeRenderState RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip = TRUE);

	// blob rendering
	void RenderObjectBlobs(RenderRegion* pRender);
	void RenderTinyBlobs(RenderRegion* pRender);

	//  require transparency ?
//	virtual BOOL NeedsTransparency() const;


	virtual String Describe(BOOL Plural, BOOL Verbose = TRUE);

	// transformation function
	virtual void Transform( TransformBase& Trans ); 


	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	
	virtual Node* 	HasEditableChild(CCRuntimeClass *ChildClass, Node* pPrevEditable);

		// Fuctions that allows ops and other nodes to inform this node of events on it's children
	virtual ChangeCode 	OnChildChange(ObjChangeParam* pParam);
	virtual BOOL 		AllowOp(ObjChangeParam* pParam,BOOL SetOpPermissionState = TRUE);
	
	
	
	// access functions
	NodeBrushPath* GetNodeBrushPath();
	void SetNodeBrushPath(NodeBrushPath * pPath);
	NodeBrushMaker* GetNodeBrushMaker();
	void SetNodeBrushMaker(NodeBrushMaker* pMaker);
	MILLIPOINT GetBrushSpacing();
	void SetBrushSpacing(MILLIPOINT Value);

	NodeRenderableInk* GetInkNode();
	BOOL ResetBrushPath();
	BOOL ReInitialiseInk();
	
	BOOL ChangeNodeBrushPath(NodeBrushPath* pNewPath);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

//	virtual BOOL IsValidEffectAttr(CCRuntimeClass* pAttrClass) const {return FALSE;}
	virtual BOOL IsValidEffectAttr(NodeAttribute* pAttr) const {return FALSE;}
	virtual BOOL GroupCanTransformCached(TransformBase& Trans) const {return FALSE;}

protected:
// return a copy of this node
	virtual Node* SimpleCopy();

	void CopyNodeContents(NodeBrush* pCopyOfNode);

	String_32 m_BrushName;
	NodeBrushPath* m_pNodeBrushPath;
	NodeBrushMaker* m_pNodeBrushMaker;
private:
	
	
	
};

#endif
