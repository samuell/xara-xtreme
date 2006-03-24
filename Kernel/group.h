// $Id: group.h 662 2006-03-14 21:31:49Z alex $
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
// Group and OpEditGroup definitions

#ifndef	INC_GROUP
#define	INC_GROUP

#include "ink.h"
#include "nodecomp.h"


class RenderRegion;
class String_32;
class NodeRect; 
class BecomeA;

/********************************************************************************************

>	class NodeGroup: public NodeCompound

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Purpose:	Group Node

********************************************************************************************/

class NodeGroup: public NodeCompound
{
public: 
	
	CC_DECLARE_DYNAMIC( NodeGroup )    
	
	NodeGroup(); 
	
	NodeGroup(Node* ContextNode,  
			  AttachNodeDirection Direction,    
			  BOOL Locked=FALSE, 
			  BOOL Mangled=FALSE,  
			  BOOL Marked=FALSE, 
			  BOOL Selected=FALSE    
		     ); 
		     
	// virtual ID fn for a group node.
	virtual BOOL IsAGroup() const { return TRUE; }

	// virtual Functions to deal with bounding boxes
	virtual DocRect	GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE);
	virtual DocRect GetBlobBoundingRect();
	virtual DocRect GetEorDragBoundingRect();
	virtual void Transform(TransformBase&);

	// Virtual rendering functions
//	virtual void Render(RenderRegion* pRRegion);
	virtual void PreExportRender( RenderRegion* pRender ); 
	virtual BOOL ExportRender( RenderRegion* pRender ); 
	virtual SubtreeRenderState RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
	virtual void RenderAfterSubtree(RenderRegion* pRender);

	// Selection Functions
	virtual void RenderObjectBlobs(RenderRegion* pRender);
	virtual void RenderTinyBlobs(RenderRegion* pRender);

	// Other virtual functions
	virtual String Describe(BOOL Plural, BOOL Verbose); 
	virtual Node* SimpleCopy();
	void CopyNodeContents(NodeGroup* NodeCopy);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	// Fuctions that allows ops and other nodes to inform this node of events on it's children
	virtual ChangeCode OnChildChange(ObjChangeParam* pParam);

	// Functions to deal with messages
	virtual BOOL OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
 
	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBoundsRecord(BaseCamelotFilter* pFilter);
	virtual BOOL WillWriteBounds(BaseCamelotFilter* pFilter);

	// Progressive rendering & the new file format
	virtual	BOOL AreYouSafeToRender();

	// Effect Attribute functions
	virtual BOOL RenderTight(RenderRegion* pRender);
	virtual BOOL CaptureTight(RenderRegion* pRender);
	virtual double GetTightGroupPixelsPerInch(RenderRegion* pRender=NULL) const;
	virtual double GetTightGroupPixelWidth(RenderRegion* pRender=NULL) const {return 72000.0/GetTightGroupPixelsPerInch(pRender);}
	virtual void TransformTight(TransformBase& Trans, double dTestPixelWidth);

//	virtual BOOL IsValidEffectAttr(CCRuntimeClass* pAttrClass) const;
	virtual BOOL IsValidEffectAttr(NodeAttribute* pAttr) const;
	virtual BOOL GroupCanTransformCached(TransformBase& Trans) const {return TRUE;}

protected:
//	BOOL CopyEffectAttrsFrom(NodeGroup* pSrcNode);
};

/********************************************************************************************

>	class NodeListItemWithDocPtr : public NodeListItem

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/7/2000
	Purpose:	This is used to allows blends of compounds to manage memory correctly.
				The class stores a pointer to the owning document - so that when the
				document dies, we only kill nodes associated with that document.
	SeeAlso:	Range::MakeListOfNodes

********************************************************************************************/

class NodeListItemWithDocPtr : public NodeListItem
{
public:
	CC_DECLARE_DYNAMIC( NodeListItemWithDocPtr )
	NodeListItemWithDocPtr();					// will contain a NULL pointer
	NodeListItemWithDocPtr(Node* WhichNode);	// initialise pNode to be WhichNode
	~NodeListItemWithDocPtr() {}				// Nothing to destruct
	
	Node* pNode;					// pointer to a node
	Document* pOwner;				// ptr to the owning doc
	BOOL isForPrinting;				// was the node generated for use in printing?
};



/********************************************************************************************

>	class OpEditGroup : public SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/94
	Purpose:	This operation handles the resizing of a group. It is usually
				started when the Group detects a drag on one of its selected blobs.

********************************************************************************************/
#if 0
class OpEditGroup : public SelOperation
{
CC_DECLARE_DYNCREATE(OpEditGroup);

public:
	// Construction/Destruction
	OpEditGroup();

	// The all important Do functions
	//void DoDrag(DocCoord Anchor, Spread*, NodeRect *pRect, INT32 FixedCorner);
	void DoWithParam(OpDescriptor* OpDesc, OpParam* pOpParam); 

	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL Success, BOOL bSolidDrag);

	// Some Render functions to will draw the EORed group
	virtual void RenderDragBlobs(DocRect, Spread*, BOOL bSolidDrag);

	// These functions required for the OpDescriptor class
	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

private:
	// Function to deal with all the undo stuff
	BOOL CompleteOperation();

	// Function to calculate the position of the rectangle after one of the corners moves
	void RebuildParallelogram(DocCoord PointerPos);
	DocRect GetDragBlobRect();

// Vars
protected:
	// Information we need to draw the EORed rectangle
	DocCoord  Parallel[4];			// The coords of the parallelogram
	INT32 	  FixedCorner;			// The corner of the rectangle that is not moving

	// Infomation about what we are editing
	NodeRect* 		OldRect;

	BOOL			UseRect;
	NodeRect*		RectPath;
	Spread*   		StartSpread;

	// Mouse Positions
	DocCoord  		StartPoint;
	DocCoord  		LastMousePosition;
};
#endif
 
#endif
