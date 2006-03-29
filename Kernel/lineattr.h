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

// Line Attribute definitions

#ifndef INC_LINEATTR
#define INC_LINEATTR

#include "nodeattr.h"
#include "doccolor.h"
#include "attr.h"
#include "fillattr.h"
#include "rndrgn.h"

class BlendAttrParam;
class BaseCamelotFilter;

/*Remember:: (Bodge)

The NodeSize fns for those attributes which contain pointers will need changing 
when we have decided what their format is to be. 

Same for the operator== fns


Simon
*/


/***********************************************************************************************

>	class AttrLineWidth : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Line Width Attribute class.

***********************************************************************************************/

class AttrLineWidth : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrLineWidth)

public:
	AttrLineWidth(); 
	AttrLineWidth(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 
	virtual UINT32 GetAttrNameID(void); 

	void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void Transform( TransformBase& );
	virtual BOOL EffectsParentBounds() { return TRUE; }

	virtual BOOL Blend(BlendAttrParam* pBlendParam);

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrLineWidth); }
	virtual AttrIndex GetAttributeIndex () { return ATTR_LINEWIDTH; }

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);
	virtual BOOL OtherAttrIsAppliedSelectively() const { return TRUE; }
	virtual BOOL IsALineWidthAttr() const;
	virtual BOOL NeedsToRenderAtEachBrushStroke() const;

private:
	void CopyNodeContents( AttrLineWidth* NodeCopy );

public:
	LineWidthAttribute Value;
};

/********************************************************************************************

>	class AttrStrokeColour : public AttrFillGeometry

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Purpose:	Stroke Colour Attribute class.

********************************************************************************************/

class AttrStrokeColour : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrStrokeColour)
	
public:
	AttrStrokeColour(); 
	AttrStrokeColour(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 
	virtual UINT32 GetAttrNameID(void);

	virtual DocRect GetBlobBoundingRect() { return DocRect(0,0,0,0); }
	virtual BOOL EffectsParentBounds();

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrStrokeColour); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual AttrIndex GetAttributeIndex () { return ATTR_STROKECOLOUR; }

	virtual AttrFillGeometry* ChangeAttrValue(AttrValueChange*);

	virtual BOOL IsAStrokeColour() const { return TRUE; } 

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);


#ifdef _DEBUG
	void ShowDebugTreeDetails() const;			// Debugging function
#endif

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

private:
	void CopyNodeContents( AttrStrokeColour* NodeCopy );

public:
	StrokeColourAttribute Value;
};

/********************************************************************************************

>	class AttrStrokeTransp : public AttrFillGeometry

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/94
	Purpose:	Stroke Transparency Attribute class.

********************************************************************************************/

class AttrStrokeTransp : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrStrokeTransp)
	
public:
	AttrStrokeTransp(); 
	AttrStrokeTransp(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 
	virtual UINT32 GetAttrNameID(void);

	virtual DocRect GetBlobBoundingRect() { return DocRect(0,0,0,0); }

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrStrokeTransp); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual AttrIndex GetAttributeIndex () { return ATTR_STROKETRANSP; }

	virtual AttrFillGeometry* ChangeAttrValue(AttrValueChange*);

	virtual BOOL IsAStrokeTransp() const { return TRUE; }

#ifdef _DEBUG
	void ShowDebugTreeDetails() const;			// Debugging function
#endif

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	// Wasn't present before 19/11/97 Neville
	virtual BOOL NeedsTransparency() const;

private:
	void CopyNodeContents( AttrStrokeTransp* NodeCopy );

public:
	StrokeTranspAttribute Value;
};

/***********************************************************************************************

>	class AttrStrokeColourChange : public AttrValueChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Stroke colour Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the colours
				of an existing Stroke.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrStrokeColourChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrStrokeColourChange)
	
public:
	//AttrStrokeColourChange() : AttrValueChange() {}

	AttrStrokeColourChange(BOOL bLineWidth=FALSE) : AttrValueChange() { m_bLineWidth = bLineWidth;}

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrStrokeColour); }

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	// This routine returns any secondary attribute that needs to be changed,
	// when this attribute is changed.
	virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);

	virtual UINT32 GetAttrNameID(void);

	virtual BOOL EffectsParentBounds();

protected:
	StrokeColourAttribute Value;

private:
	// Olivier 17/03/97
	// This boolean is used with the line width.
	// When "none" is selected in the line width combo-box, 
	// it will display "Undo line width change" rather than 
	// "Undo line colour change" in the undo list.
	BOOL m_bLineWidth;
};

/***********************************************************************************************

>	class AttrStrokeTranspChange : public AttrValueChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Stroke transparency Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the transp
				of an existing Stroke.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrStrokeTranspChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrStrokeTranspChange)
	
public:
	AttrStrokeTranspChange() : AttrValueChange() {}

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrStrokeTransp); }

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	virtual UINT32 GetAttrNameID(void);

protected:
	StrokeTranspAttribute Value;
};

/***********************************************************************************************

>	class AttrStrokeTranspTypeChange : public AttrValueChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Stroke transparency Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the transp
				of an existing Stroke.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrStrokeTranspTypeChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrStrokeTranspTypeChange)
	
public:
	AttrStrokeTranspTypeChange() : AttrValueChange() {}

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrStrokeTransp); }

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	virtual UINT32 GetAttrNameID(void);

protected:
	StrokeTranspAttribute Value;
};

/***********************************************************************************************

>	class AttrStartArrow : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/93
	Purpose:	Start Arrow Attribute class.
	SeeAlso:	ArrowRec

***********************************************************************************************/

class AttrStartArrow : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrStartArrow)
	
public:
	
	AttrStartArrow(); 
	AttrStartArrow(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	virtual BOOL EffectsParentBounds();
	virtual DocRect GetAttrBoundingRect(NodeRenderableInk* pParent = NULL, CCAttrMap* pAttribMap = NULL);

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrStartArrow); }
	virtual AttrIndex GetAttributeIndex () { return ATTR_STARTARROW; }

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib);
	virtual UINT32 GetAttrNameID(void); 


#ifdef _DEBUG
	void ShowDebugTreeDetails() const;			// Debugging function
#endif
	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	void CopyNodeContents( AttrStartArrow* NodeCopy );

public:
	StartArrowAttribute Value;
};

/***********************************************************************************************

>	class AttrEndArrow : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/93
	Purpose:	End Arrow Attribute class.
	SeeAlso:	ArrowRec

***********************************************************************************************/

class AttrEndArrow : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrEndArrow)
	
public:
	AttrEndArrow(); 
	AttrEndArrow(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	virtual BOOL EffectsParentBounds();
	virtual DocRect GetAttrBoundingRect(NodeRenderableInk* pParent = NULL, CCAttrMap* pAttribMap = NULL);

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrEndArrow); }
	virtual AttrIndex GetAttributeIndex () { return ATTR_ENDARROW; }

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib);
	virtual UINT32 GetAttrNameID(void); 


#ifdef _DEBUG
	void ShowDebugTreeDetails() const;			// Debugging function
#endif
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	void CopyNodeContents( AttrEndArrow* NodeCopy );

public:
	EndArrowAttribute Value;
};

/***********************************************************************************************

>	class AttrStartCap : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/93
	Purpose:	Start Cap Attribute class.
	SeeAlso:	LineCapType

***********************************************************************************************/

class AttrStartCap : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrStartCap)
	
public:
	AttrStartCap(); 
	AttrStartCap(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib);
	virtual UINT32 GetAttrNameID(void); 

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrStartCap); }
	virtual AttrIndex GetAttributeIndex () { return ATTR_STARTCAP; }

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual BOOL EffectsParentBounds() { return TRUE; }

#ifdef _DEBUG
	void ShowDebugTreeDetails() const;			// Debugging function
#endif
	void GetDebugDetails(StringBase* Str);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	void CopyNodeContents( AttrStartCap* NodeCopy );

public:
	StartCapAttribute Value;
};

/***********************************************************************************************

>	class AttrJoinType : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/93
	Purpose:	JoinType Attribute class.
	SeeAlso:	JointType

***********************************************************************************************/

class AttrJoinType : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrJoinType)
	
public:
	AttrJoinType(); 
	AttrJoinType(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 
	virtual UINT32 GetAttrNameID(void);

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrJoinType); }
	virtual AttrIndex GetAttributeIndex () { return ATTR_JOINTYPE; }

	virtual BOOL EffectsParentBounds() { return TRUE; }

#ifdef _DEBUG
	void ShowDebugTreeDetails() const;			// Debugging function
#endif
	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	void CopyNodeContents( AttrJoinType* NodeCopy );

public:
	JoinTypeAttribute Value;
};

/***********************************************************************************************

>	class AttrMitreLimit : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/93
	Purpose:	Mitre Limit Attribute class.

***********************************************************************************************/

class AttrMitreLimit : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrMitreLimit)
	
public:
	AttrMitreLimit(); 
	AttrMitreLimit(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib);
	virtual UINT32 GetAttrNameID(void); 

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrMitreLimit); }
	virtual AttrIndex GetAttributeIndex () { return ATTR_MITRELIMIT; }

	virtual BOOL EffectsParentBounds() { return TRUE; }

#ifdef _DEBUG
	void ShowDebugTreeDetails() const;			// Debugging function
#endif
	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	void CopyNodeContents( AttrMitreLimit* NodeCopy );

public:
	MitreLimitAttribute Value;
};

/***********************************************************************************************

>	class AttrWindingRule : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/93
	Purpose:	Winding Rule Attribute class.
	SeeAlso:	WindingType

***********************************************************************************************/

class AttrWindingRule : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrWindingRule)
	
public:
	AttrWindingRule(); 
	AttrWindingRule(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 
	virtual UINT32 GetAttrNameID(void);

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrWindingRule); }
	virtual AttrIndex GetAttributeIndex () { return ATTR_WINDINGRULE; }

#ifdef _DEBUG
	void ShowDebugTreeDetails() const;			// Debugging function
#endif
	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	void CopyNodeContents( AttrWindingRule* NodeCopy );

public:
	WindingRuleAttribute Value;
};

/***********************************************************************************************

>	class AttrDashPattern : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/93
	Purpose:	Dash Pattern Attribute class.
	SeeAlso:	DashRec

***********************************************************************************************/

class AttrDashPattern : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrDashPattern)
	
public:
	AttrDashPattern(); 
	AttrDashPattern(Node* ContextNode,  
					  AttachNodeDirection Direction,    
				      BOOL Locked=FALSE, 
				      BOOL Mangled=FALSE,  
				      BOOL Marked=FALSE, 
					  BOOL Selected=FALSE); 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 
	virtual UINT32 GetAttrNameID(void);

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrDashPattern); }

	virtual AttrIndex GetAttributeIndex () { return ATTR_DASHPATTERN; }

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

#ifdef _DEBUG
	void ShowDebugTreeDetails() const;			// Debugging function
#endif
	void GetDebugDetails(StringBase* Str);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	void CopyNodeContents( AttrDashPattern* NodeCopy );

public:
	DashPatternAttribute Value;
};

#endif


