// $Id: nodepath.h 662 2006-03-14 21:31:49Z alex $
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
// The Node Renderable Path Class

#ifndef INC_NODEPATH
#define INC_NODEPATH


#include "paths.h"
#include "ink.h"
#include "rndrgn.h"

#define OPTOKEN_NODEPATH	_T("PathObject")

class BecomeA;
class CamelotFileRecord;
class RenderRegion;
class Matrix;
class CCAttrMap;
class BlendPath;

/***********************************************************************************************

>	class NodePath : public NodeRenderableInk

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/5/93
	Purpose:	Puts path objects into the tree
	SeeAlso:	Path

***********************************************************************************************/

class NodePath : public NodeRenderableInk
{
// Give my name out in memory dumps
CC_DECLARE_DYNCREATE( NodePath )

public:
	Path	InkPath;									// Public path
	
public:
	NodePath(); 
	NodePath(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked = FALSE, 
					BOOL Mangled = FALSE,  
					BOOL Marked = FALSE, 
					BOOL Selected = FALSE    
			  );      
			      
	BOOL SetUpPath(INT32 RequiredSize = 12, INT32 BlockSize = 12); 
	virtual Node* SimpleCopy();						// Copies a node

	// virtual Functions to deal with bounding boxes
	virtual DocRect	GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE);
	virtual DocRect GetBlobBoundingRect();

	// Rendering Functions
	virtual void Render( RenderRegion* pRender );
	virtual void RenderEorDrag( RenderRegion* );

	// Selection functions.
	virtual void RenderObjectBlobs(RenderRegion* pRender);
	virtual void RenderTinyBlobs(RenderRegion* pRender);
	virtual void RenderPenBlobs(RenderRegion* pRender);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsNative(BaseCamelotFilter* pFilter);

	virtual UINT32 ChooseTagValue();
	virtual BOOL  WritePathRecord(BaseCamelotFilter* pFilter);
	virtual BOOL  WritePathRefRecord(BaseCamelotFilter* pFilter,UINT32 SrcPathRecNum,Matrix* pTransform);
	virtual INT32  CalcPathRecordSize();
	virtual BOOL  WritePathToRecord(CamelotFileRecord* pRecord);

	// 'Make shapes' functions
	virtual BOOL CanBecomeA(BecomeA* pBecomeA); 
	virtual BOOL DoBecomeA(BecomeA* pBecomeA); 

	void ClearSubSelection(Spread* pSpread);

    // Responce to clicks from tools
	virtual BOOL OnClick( DocCoord, ClickType, ClickModifiers, Spread * );
	virtual BOOL OnBlobPopUp(Spread*, DocCoord, ContextMenu*);
	virtual BOOL OnNodePopUp(Spread*, DocCoord, ContextMenu*);

	// Other virtual functions
	virtual void Transform( TransformBase& );				// Transforms this object
	
	virtual String Describe(BOOL Plural, BOOL Verbose); 

	#ifdef _DEBUG
		void	ShowDebugTreeDetails() const;		// Debugging function
	#endif
	void GetDebugDetails( StringBase* Str );
	
	virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes
	virtual BOOL IsNodePath() const;
	virtual BOOL IsPathAllowable();

	// The natty Snap functions
	virtual BOOL Snap(DocCoord* pDocCoord);
	virtual BOOL Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord);
	virtual BOOL SnapToCoords(DocCoord* pDocCoord);

	virtual double GetRotationAngle();
	virtual void DeSelect(BOOL ReDraw, BOOL bDeselectChildren = FALSE);
	virtual BOOL NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly = FALSE,
							   BOOL CheckSelected = FALSE);
	virtual BOOL AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState = TRUE,
												 BOOL DoPreTriggerEdit = TRUE);
												
	virtual BOOL IsANodeBlendPath();

	virtual ChangeCode 	OnChildChange(ObjChangeParam* pParam);

	// Converts this path & all its attributes to a closed path and returns it.
	NodePath * MakeNodePathFromAttributes(	double Flatness = 750.0,
											CCAttrMap* pAttrMap = NULL,
											BOOL bIncludeClosedPaths = FALSE,
											BOOL IncludeWidth = TRUE);
	
	virtual NodePath* GetVariableWidthStrokePath();
	virtual NodePath* GetSmoothVariableWidthStrokePath();

	virtual INT32 EstimateNodeComplexity (OpParam* details);

	BOOL RetroSmoothMe(double Smoothness);

	virtual BOOL GetAreaDetails(XLONG* pxlArea, XLONG* pXLPerimeter);

public:
	virtual BOOL IsDifferent(Node *pOther);
		// Determine if 2 nodes are considered to be equal.
		// NOTE: This may not be implemented fully for all derived classes, so for certain
		// types of node, the comparison will only be to see if they are of the same class!
		// (This was originally added for the StrokeComponent to be able to compare 2
		// subtrees to see if they are equal - a simple base-class implementation plus
		// overrides in NodeRenderable & NopdePath suffices to distinguish most Stroke clipart)

	// Karim MacDonald 25/11/1999
	// Extend functions.
	// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
	// also provide complete implementations of ValidateExtend and Extend,
	// which should usually check/extend its children.
public:
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);

	// virtual override of NodeRenderable::TransformTranslateObject.
	void TransformTranslateObject(const ExtendParams& ExtParams);

	// virtual override of NodeRenderableBounded::GetExtendTargetBounds().
	virtual DocRect GetExtendTargetBounds(const ExtendParams& ExtParams);

public:
	static BOOL CreateFromPath(NodePath** ppNewPath,	Path* pSrcPath = NULL,
													Operation* pOp = NULL);
	Path* GetPathCopy();

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);
	
protected:
	void CopyNodeContents( NodePath* NodeCopy );		// Copies the contents of the path

private:
	void HandleBlobClick(DocCoord*, PathFlags*, INT32, INT32, BOOL, BOOL);

protected:
	double CurrentRotation;

protected:
};

#endif	// INC_NODEPATH


