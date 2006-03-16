// $Id: nodemold.h 662 2006-03-14 21:31:49Z alex $
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
// The NodeMould renderable class

#ifndef INC_NODEMOULD
#define INC_NODEMOULD


#include "ink.h"
#include "rndrgn.h"
//#include "selop.h"
//#include "moldshap.h"
#include "group.h"
#include "becomea.h"
#include "doccoord.h"
#include "objchge.h"

#define OPTOKEN_NODEMOULD _T("MouldObject")
#define MOULD_EPS_VERSION 100			  // v 1.00

//class BecomeA;
class NodeMoulder;
class NodeMouldGroup;
class NodeMouldPath;
class MouldGeometry;
class BaseCamelotFilter;

/***********************************************************************************************

>	enum MouldSpace

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/94
	Purpose:	defines the types of mould vector spaces that exist

***********************************************************************************************/

typedef enum MouldSpace {
	MOULDSPACE_UNDEFINED,
	MOULDSPACE_ENVELOPE,
	MOULDSPACE_PERSPECTIVE,
	MOULDSPACE_ENVELOPE2X2
};

#define REC_REBUILD 	1
#define REC_PATHARRAYS	2
#define REC_BLOBS		4
#define REC_GEOMETRY	8
#define REC_GEOMCONTEXT	16

#define REC_TYPE		INT32

/***********************************************************************************************

>	class NodeMould : public NodeGroup

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Purpose:	This class defines the structure of a mould node object. This object is the
				parent of a MouldGroup node and a Moulder node. Objects being moulded are 
				placed inside MouldGroup and all resulting moulded objects are placed inside
				the Moulder node.

***********************************************************************************************/

class NodeMould: public NodeGroup
{
	friend class RecordChangeCodesAction;

	CC_DECLARE_DYNAMIC( NodeMould );    

public: 
	NodeMould(); 
	NodeMould(Node* ContextNode,  
			  AttachNodeDirection Direction,    
			  BOOL Locked=FALSE, 
			  BOOL Mangled=FALSE,  
			  BOOL Marked=FALSE, 
			  BOOL Selected=FALSE    
		     ); 
	~NodeMould();

			void	InitialiseVars();

	// Virtual rendering functions	     
	virtual void 	PreExportRender( RenderRegion* pRender ); 
	virtual BOOL	ExportRender( RenderRegion* pRender ); 
	virtual BOOL	PostImport();

	virtual void 	RenderObjectBlobs(RenderRegion* pRender);
	virtual void 	RenderTinyBlobs(RenderRegion* pRender);
	virtual DocRect GetBlobBoundingRect();
	virtual void 	Transform( TransformBase& Trans );

	// Other virtual functions
	virtual String	Describe(BOOL Plural, BOOL Verbose); 
	virtual Node*	SimpleCopy();  
    virtual UINT32	GetNodeSize() const; 			// Returns size of node in bytes
	virtual BOOL	IsANodeMould() const;

	// Functions to deal with messages
	virtual BOOL 	OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
			BOOL 	OnMouseMove( DocCoord, Spread*, ClickModifiers, INT32*, INT32* );

	// Functions to handle changes to moulds
	virtual ChangeCode OnChildChange(ObjChangeParam* pParam);

	// Specify whats editable inside ourselves
	virtual Node* 	HasEditableChild(CCRuntimeClass *ChildClass, Node* pPrevEditable);
	virtual BOOL 	OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu);

 	// Replace DoBecomeA() so that the mould node can turn itself into a group
	virtual BOOL 	CanBecomeA(BecomeA* pBecomeA);
	virtual BOOL 	DoBecomeA(BecomeA* pBecomeA);

	virtual BOOL	AllowSelectInside() const { return FALSE; }

	#ifdef _DEBUG
			void	ShowDebugTreeDetails() const;					// Debugging function
	#endif
			void	GetDebugDetails( StringBase* Str );

	// CAM/AW EPS output functions
			void	PreExportCAMEPS(RenderRegion* pRegion);
			BOOL	PostExportCAMEPS(RenderRegion* pRegion);
			void	PreExportAWEPS(RenderRegion* pRegion);
			BOOL	PostExportAWEPS(RenderRegion* pRegion);

			void	ToggleDetachFlag();
			BOOL	IsDetached() const { return DetachedGeometry; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	// temporary function!
	static	void	RemovePerspectiveFills(Node* pParentNode, UndoableOperation* pUndoOp);

	// Karim MacDonald 26/05/2000
	// Extend functions. Maybe these functions should really originate in NodeRenderable?
	// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
	// also provide complete implementations of ValidateExtend and Extend,
	// which should usually check/extend its children.
public:
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);

	// Karim 20/06/2000 - find the extend bounding rect of this node, for use with Extension.
	virtual DocRect GetExtendTargetBounds(const ExtendParams& ExtParams);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// the following functions are node mould specific

public:
	MouldSpace		DescribeGeometry();
	MouldGeometry*	GetGeometry() const { return pMouldGeometry; }
	NodeMouldPath*	GetPathShape();
	Path* 			GetPath();
	DocRect			GetRenderBounds();
	DocRect			GetChildrensBounds();

	void			DisableBlobs() { RenderBlobs=FALSE;	}
	void			EnableBlobs()  { RenderBlobs=TRUE; }

	BOOL			CreateGeometry(MouldSpace mSpace);
	NodeMoulder*	CreateNewMoulder(UndoableOperation*);
	NodeMouldPath*	CreateNewMouldShape(Path* pPath, DocRect* const pDestin, UndoableOperation* pOp);
	NodeMouldGroup* CreateNewMouldGroup(UndoableOperation* pOp);
	BOOL 			CreateAllMoulderObjects(UndoableOperation* pOp);

	NodeMoulder*	AddNewMoulder(Node*, AttachNodeDirection, UndoableOperation*);
	NodeMouldPath*	AddNewMouldShape(Path* pPath, DocRect* const pDestin, UndoableOperation* pOp);
	NodeMouldGroup*	AddNewMouldGroup(UndoableOperation* pOp);

	BOOL 			FillMouldGroup(NodeMouldGroup*, List*,	UndoableOperation*);

	NodeMoulder* 	FindFirstMoulder(BOOL errorcheck=TRUE);
	NodeMoulder*	FindNextMoulder(NodeMoulder* pNodeMoulder);
	NodeMouldGroup* FindMouldGroup(BOOL errorcheck=TRUE);
	ChangeCode 		RemouldAll(UndoableOperation* pUndoOp);
	ChangeCode		StartSaveContext(UndoableOperation* pUndoOp, REC_TYPE);
	ChangeCode		EndSaveContext(UndoableOperation* pUndoOp, REC_TYPE);
	BOOL 			CreateAttrSetFromChildren(AttrTypeSet& AttSet);

	// used only for undo (and probably temporary)
	BOOL			SetGeometry(MouldGeometry*);
	INT32 			CountMoulders();

//	virtual BOOL IsValidEffectAttr(CCRuntimeClass* pAttrClass) const {return FALSE;}
	virtual BOOL IsValidEffectAttr(NodeAttribute* pAttr) const {return FALSE;}
	virtual BOOL GroupCanTransformCached(TransformBase& Trans) const {return TRUE;}

private:
	void 			RedrawMould();
	void 			CopyNodeContents(NodeMould* pCopyOfNode);
	ChangeCode		HandleMouldShapeChange(ObjChangePathEdit* pParam);
	void			IncludeChildrensBoundingRects(DocRect& BoundingRect);
	BOOL 			OverMouldEndPoint(DocCoord coord, Spread* pSpread);
	ChangeCode		RecordChangeCodes(UndoableOperation* pOp);
	ChangeCode		ConvertCode(ActionCode Act);
	
	String_32		MouldName; // The name of the mould 

	MouldGeometry*	pMouldGeometry;
	BOOL			DetachedGeometry;

private:
	// Temorary private vars until v2.0
	INT32 			OnCC_CRC;
	INT32			OnCC_Width;
	INT32			OnCC_Height;
	BOOL			RenderBlobs;
};



/********************************************************************************************

	class RecordChangeCodesAction : public Action

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Purpose:	This class records undo/redo information for the Perspective shape object.
	
********************************************************************************************/

class RecordChangeCodesAction : public Action
{
	CC_DECLARE_DYNCREATE(RecordChangeCodesAction)

	public:
		 RecordChangeCodesAction();
		~RecordChangeCodesAction();
		virtual ActionCode Execute();
		static ActionCode Init( Operation* pOp,
								ActionList* pActionList,
								NodeMould* pMould,
								Action** NewAction);
	private:
		NodeMould* pCRCMould;
		INT32  CRCcode;
		INT32 CRCWidth;
		INT32 CRCHeight;
};


#endif
