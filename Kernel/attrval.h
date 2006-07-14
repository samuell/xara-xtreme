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

// Defines AttributeValue and some derived classes 

#ifndef INC_ATTRVAL
#define INC_ATTRVAL

#include "doccoord.h"
#include "doccolor.h"
#include "ensure.h"
#include "attr.h"
#include "arrows.h"
#include "node.h"

class Path;
class RenderRegion;
class Node;
class NodeAttribute;
class BlendAttrParam;
class PathStrokerVector;

/********************************************************************************************

>	class AttributeValue : public CCObject

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/94
	Purpose:	To encapsulate rendering attributes.  Derivations of NodeAttribute will
				hold an instance of this class (or derivation of it) to store the
				attribute information - this instance field should always be called 'Value'.
				An AttributeValue object is capable of being pushed onto and popped off the
				render context stack, and being rendered.  This is accomplished by using
				virtual functions to perform these operations.
	SeeAlso:	NodeAttribute; RenderStack

********************************************************************************************/

class AttributeValue : public CCObject
{
	CC_DECLARE_DYNCREATE(AttributeValue)
public:
	AttributeValue() {};
	virtual ~AttributeValue() {};

	virtual void Render(RenderRegion *, BOOL Temp = FALSE) = 0;
			// Called to "render" an attribute (makes it current for the first time)

	virtual void Restore(RenderRegion *, BOOL Temp) = 0;
			// Called to "restore" a stacked attribute (makes it current once more)

	virtual void GoingOutOfScope(RenderRegion *pRender) {};
			// Called when a current attribute goes out of scope
			// (only called once, when the attribute is "popped" from being current)
			// Used to allow us to remove any PathProcessor we may have added to the RndRgn

	virtual void SimpleCopy(AttributeValue *) = 0;
	virtual NodeAttribute *MakeNode();
	virtual NodeAttribute *MakeNode(Node* pContextNode, AttachNodeDirection Direction);
	virtual BOOL IsDifferent(AttributeValue*);

	// This blends the attribute value with the one provided by pBlendParam
	virtual BOOL Blend(BlendAttrParam* pBlendParam);

	virtual INT32 operator=(AttributeValue& Attrib) { return TRUE; }

public:
	virtual AttributeValue *MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale = 1.0);
			// Moulds the attribute into the given Stroke envelope. This translates fill
			// handles, and scales transparency values and line widths (etc). The base class
			// does nothing.

	// There is only one case to date where AttributeValue are rendered directly ie without calling the render function of their
	// associated/containing NodeAttribute class. This occurs in RenderRegion::InitDevice().
	// This function is to determine if the AttributeValue can be rendered independently of the NodeAttribute.
	// It is only used in RR::InitDevice() at present but I am defining it in order to highlight this new characteristic of
	// nodes which are geometry dependent, in order to ensure that this becomes an acknowledged attribute design consideration
	// (ie so that these special cases get handled correctly by future code which may need to render attributes directly)
	virtual BOOL CanBeRenderedDirectly();
};

/********************************************************************************************

>	class LineWidthAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/06/94
	Purpose:	Represent a line width of a path.  This may be a simple stroking operator
				such as a constant line width, or a complex one such as variable line
				widths or an airbrush stroke operator.
	SeeAlso:	FillColourAttribute; StrokeColourAttribute;
				StartArrowAttribute; EndArrowAttribute; StartCapAttribute; EndCapAttribute;
				JoinTypeAttribute; MitreLimitAttribute; WindingRuleAttribute;
				DashPatternAttribute

********************************************************************************************/

class LineWidthAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(LineWidthAttribute)
public:
	LineWidthAttribute();
	LineWidthAttribute(MILLIPOINT NewLineWidth) { LineWidth = NewLineWidth; }
	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual NodeAttribute *MakeNode();
	BOOL IsDifferent(AttributeValue *pAttr);
	virtual void SimpleCopy(AttributeValue *);

	// This blend the attribute value with the one provided by pBlendParam
	virtual BOOL Blend(BlendAttrParam* pBlendParam);

	// Extra virtual function for stroke type attributes.
	virtual BOOL RenderStroke(RenderRegion *, Path *) { return FALSE; }

	// Simplified colour for render regions that can only handle simple line widths.
	MILLIPOINT LineWidth;

	static BOOL Init();

public:
	virtual AttributeValue *MouldIntoStroke(PathStrokerVector *pMoulder, double TransScale = 1.0);
			// Moulds the attribute into the given Stroke envelope. This translates fill
			// handles, and scales transparency values and line widths (etc). The base class
			// does nothing.
};

/********************************************************************************************

>	class StartArrowAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/94
	Purpose:	Attribute for start arrow type of paths.
	SeeAlso:	FillColourAttribute; StrokeColourAttribute; LineWidthAttribute;
				EndArrowAttribute; StartCapAttribute; EndCapAttribute;
				JoinTypeAttribute; MitreLimitAttribute; WindingRuleAttribute;
				DashPatternAttribute

********************************************************************************************/

class StartArrowAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(StartArrowAttribute)
public:
	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();
	BOOL IsDifferent(AttributeValue *pAttr);

	// The arrow style
	ArrowRec StartArrow;

	static BOOL Init();
};

/********************************************************************************************

>	class EndArrowAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/94
	Purpose:	Attribute for end arrow type of paths.
	SeeAlso:	FillColourAttribute; StrokeColourAttribute; LineWidthAttribute;
				StartArrowAttribute; StartCapAttribute; EndCapAttribute;
				JoinTypeAttribute; MitreLimitAttribute; WindingRuleAttribute;
				DashPatternAttribute

********************************************************************************************/

class EndArrowAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(EndArrowAttribute)
public:
	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();
	BOOL IsDifferent(AttributeValue *pAttr);

	// The arrow style
	ArrowRec EndArrow;

	static BOOL Init();
};

/********************************************************************************************

>	class StartCapAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/94
	Purpose:	Attribute for start cap style of paths.
	SeeAlso:	FillColourAttribute; StrokeColourAttribute; LineWidthAttribute;
				StartArrowAttribute; EndArrowAttribute; EndCapAttribute;
				JoinTypeAttribute; MitreLimitAttribute; WindingRuleAttribute;
				DashPatternAttribute

********************************************************************************************/

class StartCapAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(StartCapAttribute)
public:
	StartCapAttribute();

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();
	virtual BOOL IsDifferent(AttributeValue*);

	// The cap style
	LineCapType StartCap;

	static BOOL Init();
};

/********************************************************************************************

>	class JoinTypeAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/94
	Purpose:	Attribute for join types of paths.
	SeeAlso:	FillColourAttribute; StrokeColourAttribute; LineWidthAttribute;
				StartArrowAttribute; EndArrowAttribute; StartCapAttribute; EndCapAttribute;
				MitreLimitAttribute; WindingRuleAttribute; DashPatternAttribute

********************************************************************************************/

class JoinTypeAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(JoinTypeAttribute)
public:
	JoinTypeAttribute();

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();
	virtual BOOL IsDifferent(AttributeValue*);

	// The join type
	JointType JoinType;

	static BOOL Init();
};

/********************************************************************************************

>	class MitreLimitAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/94
	Purpose:	Attribute for mitre limits of paths.
	SeeAlso:	FillColourAttribute; StrokeColourAttribute; LineWidthAttribute;
				StartArrowAttribute; EndArrowAttribute; StartCapAttribute; EndCapAttribute;
				JoinTypeAttribute; WindingRuleAttribute; DashPatternAttribute

********************************************************************************************/

class MitreLimitAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(MitreLimitAttribute)
public:
	MitreLimitAttribute();

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();
	BOOL IsDifferent(AttributeValue *pAttr);

	// The mitre limit
	MILLIPOINT MitreLimit;

	static BOOL Init();
};

/********************************************************************************************

>	class WindingRuleAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/94
	Purpose:	Attribute for the winding rule of a path.
	SeeAlso:	FillColourAttribute; StrokeColourAttribute; LineWidthAttribute;
				StartArrowAttribute; EndArrowAttribute; StartCapAttribute; EndCapAttribute;
				JoinTypeAttribute; MitreLimitAttribute; DashPatternAttribute

********************************************************************************************/

class WindingRuleAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(WindingRuleAttribute)
public:
	WindingRuleAttribute();
	WindingRuleAttribute(WindingType NewRule) { WindingRule = NewRule; }

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();
	virtual BOOL IsDifferent(AttributeValue*);

	// Winding rule
	WindingType WindingRule;

	static BOOL Init();
};

/********************************************************************************************

>	class DashPatternAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/94
	Purpose:	Attribute for dash pattern style of paths.
	SeeAlso:	FillColourAttribute; StrokeColourAttribute; LineWidthAttribute;
				StartArrowAttribute; EndArrowAttribute; StartCapAttribute; EndCapAttribute;
				JoinTypeAttribute; MitreLimitAttribute; WindingRuleAttribute

********************************************************************************************/

class DashPatternAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(DashPatternAttribute)
public:
	DashPatternAttribute();
	~DashPatternAttribute();

	BOOL SetDashPattern(DashRec& NewDash);
	BOOL SetStockDashPattern(StockDash NewDash);
	BOOL SetDeviceDashPattern(DashRec& NewDash, INT32 PixelSize);
	BOOL SetDeviceStockDashPattern(StockDash NewDash, INT32 PixelSize);
	INT32 operator==(DashPatternAttribute&);

	virtual void Restore(RenderRegion*, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue*);
	virtual NodeAttribute* MakeNode();
	BOOL IsDifferent(AttributeValue *pAttr);

	// Dash pattern
	DashRec DashPattern;

	static BOOL Init();
};

/********************************************************************************************

>	class DrawingModeAttribute : public AttributeValue

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/94
	Purpose:	Attribute for drawing mode - EOR, COPY etc.
	SeeAlso:	FillColourAttribute; StrokeColourAttribute; LineWidthAttribute;
				StartArrowAttribute; EndArrowAttribute; StartCapAttribute; EndCapAttribute;
				JoinTypeAttribute; MitreLimitAttribute; WindingRuleAttribute

********************************************************************************************/

class DrawingModeAttribute : public AttributeValue
{
	CC_DECLARE_DYNCREATE(DrawingModeAttribute)
public:
	DrawingModeAttribute() {}
	DrawingModeAttribute(DrawModeType NewDrawingMode) { DrawingMode = NewDrawingMode; }

	virtual void Restore(RenderRegion *, BOOL);
	virtual void Render(RenderRegion *, BOOL Temp = FALSE);
	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();
	BOOL IsDifferent(AttributeValue *pAttr);

	// Drawing Mode
	DrawModeType DrawingMode;
	static UINT32 ID;
};



#endif // INC_ATTRVAL
