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

#ifndef INC_XPFCAPS
#define INC_XPFCAPS

#include "nodershp.h"
#include "nodecont.h"
#include "ncntrcnt.h"
#include "nbevcont.h"
#include "nodeblnd.h"
#include "nodemold.h"
#include "nodebmp.h"
#include "nodetxts.h"
#include "ndclpcnt.h"
#include "nodeliveeffect.h"
#include "layer.h"

class XPFRenderRegion;

typedef enum XPFConvertType
{
	XPFCONVTYPE_ERROR = -1,
	XPFCONVTYPE_UNKNOWN = 0,
	XPFCONVTYPE_NATIVE,
	XPFCONVTYPE_SIMPLE,
	XPFCONVTYPE_STROKED,
	XPFCONVTYPE_BITMAP,
	XPFCONVTYPE_BITMAPFILL,
	XPFCONVTYPE_BITMAPTRANS,
	XPFCONVTYPE_BITMAPFILLTRANS,
	XPFCONVTYPE_BITMAPSPAN,
	XPFCONVTYPE_REFORMAT,
	XPFCONVTYPE_REMOVE,
	XPFCONVTYPE_SIMPLERGB,
	XPFCONVTYPE_RGB,
	XPFCONVTYPE_CMYK
} XPFConvertType;

typedef enum XPFBOOL
{
	XPFB_UNKNOWN = -1,
	XPFB_FALSE = 0,
	XPFB_TRUE = 1
} XPFBOOL;

typedef INT32 XPFProp;

#define XPFP_CONTENTONLY_TEXT 1
#define XPFP_CONTENTONLY_PLAINTEXT 2

#define XPFP_UNKNOWN -1
#define XPFP_SHADOWTYPE_WALL 1
#define XPFP_SHADOWTYPE_FLOOR 2
#define XPFP_SHADOWTYPE_GLOW 3
#define XPFP_SHADOWTYPE_FEATHER 4

#define XPFP_BEVELTYPE_FLAT 0
#define XPFP_BEVELTYPE_ROUND 1
#define XPFP_BEVELTYPE_HALFROUND 2
#define XPFP_BEVELTYPE_FRAME 3
#define XPFP_BEVELTYPE_MESA_1 4
#define XPFP_BEVELTYPE_MESA_2 5
#define XPFP_BEVELTYPE_SMOOTH_1 6
#define XPFP_BEVELTYPE_SMOOTH_2 7
#define XPFP_BEVELTYPE_POINT_1 8
#define XPFP_BEVELTYPE_POINT_2a 9
#define XPFP_BEVELTYPE_POINT_2b 10
#define XPFP_BEVELTYPE_RUFFLE_2a 11
#define XPFP_BEVELTYPE_RUFFLE_2b 12
#define XPFP_BEVELTYPE_RUFFLE_3a 13
#define XPFP_BEVELTYPE_RUFFLE_3b 14

#define XPFP_BEVELSIDE_INNER 0
#define XPFP_BEVELSIDE_OUTER 1

#define XPFP_COLOUREFFECT_FADE 0
#define XPFP_COLOUREFFECT_RAINBOW 1
#define XPFP_COLOUREFFECT_ALTRAINBOW 2

#define XPFP_MOULDTYPE_ENVELOPE 1
#define XPFP_MOULDTYPE_PERSPECTIVE 2

#define XPFP_LINECAP_BUTT 0
#define XPFP_LINECAP_ROUND 1
#define XPFP_LINECAP_SQUARE 2

#define XPFP_LINEJOIN_MITRE 0
#define XPFP_LINEJOIN_ROUND 1
#define XPFP_LINEJOIN_BEVEL 2

#define XPFP_FILLREPEAT_SIMPLE 1
#define XPFP_FILLREPEAT_REPEAT 2
#define XPFP_FILLREPEAT_REPEATINV 3
#define XPFP_FILLREPEAT_REPEATEXTRA 4

#define XPFP_TRANSTYPE_NONE 0
#define XPFP_TRANSTYPE_MIX 1

typedef struct PropMapEntry
{
	TCHAR* pName;
	XPFProp Value;
} PropMapEntry;


/********************************************************************************************

>	class XPFCapability : public CC_CLASS_MEMDUMP

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the baseclass for the various nodes in the capability tree

********************************************************************************************/

class XPFCapability : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCapability);

public:
	XPFCapability(XPFConvertType ConvertType = XPFCONVTYPE_UNKNOWN)
	{
		m_pNext = NULL;
		m_pChild = NULL;
		m_ConvertType = ConvertType;
	}
	virtual ~XPFCapability()
	{
		XPFCapability* pNext = NULL;
		XPFCapability* pCap = m_pChild;
		while (pCap)
		{
			pNext = pCap->GetNext();
			delete pCap;
			pCap = pNext;
		}
	}

	XPFCapability* GetNext() const { return(m_pNext); }
	XPFCapability* GetChild() const { return(m_pChild); }
	XPFConvertType GetConvertType() const { return(m_ConvertType); }

	void SetNext(XPFCapability* pNext) { m_pNext = pNext; }
	void SetChild(XPFCapability* pChild) { m_pChild = pChild; }

	XPFConvertType GetConvertTypeForNode(Node* pNode);
	XPFConvertType GetConvertTypeForAttrs(RenderRegion* pRegion);

public:

protected:
	virtual BOOL DoesNodeMatch(Node* pNode) { return(FALSE); }
	virtual BOOL DoAttributesMatch(RenderRegion* pRegion) { return(FALSE); }
	virtual BOOL DoesColourMatch(DocColour* pColour) { return(FALSE); }

	BOOL AreAllChildrenText(Node* pRootNode, BOOL bPlain);
	BOOL IsTextStoryPlain(Node* pNode);

protected:
	XPFCapability* m_pNext;
	XPFCapability* m_pChild;
	XPFConvertType m_ConvertType;
};



/********************************************************************************************

>	class XPFCComplexClass : public XPFCapability

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is a general class that just uses the runtime class of the node
				It can be used directly to handle those complex objects that just support
				a simple on/off style of conversion or it can be used as the baseclass 
				for more complex classes

********************************************************************************************/

class XPFCComplexClass : public XPFCapability
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCComplexClass);

public:
	XPFCComplexClass(CCRuntimeClass* pClass = NULL, XPFConvertType m_ConvertType = XPFCONVTYPE_UNKNOWN)
		: XPFCapability(m_ConvertType)
	{
		m_pClass = pClass;
	}
	virtual ~XPFCComplexClass()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);

protected:
	CCRuntimeClass* m_pClass;
};


/********************************************************************************************

>	class XPFCLayer : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for layer capabilities

********************************************************************************************/

class XPFCLayer : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCLayer);

public:
	XPFCLayer(XPFConvertType ConvertType, 
				XPFBOOL bVisible, 
				XPFBOOL bLocked, 
				XPFBOOL bPrintable, 
				XPFBOOL bActive, 
				XPFBOOL bBackground, 
				XPFBOOL bGuide,
				XPFProp ContentOnly)
		: XPFCComplexClass(CC_RUNTIME_CLASS(Layer), ConvertType)
	{
		m_bVisible = bVisible;
		m_bLocked = bLocked;
		m_bPrintable = bPrintable;
		m_bActive = bActive;
		m_bBackground = bBackground;
		m_bGuide = bGuide;
		m_ContentOnly = ContentOnly;
	}
	virtual ~XPFCLayer()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);

protected:
	XPFBOOL m_bVisible;
	XPFBOOL m_bLocked;
	XPFBOOL m_bPrintable;
	XPFBOOL m_bActive;
	XPFBOOL m_bBackground;
	XPFBOOL m_bGuide;
	XPFProp m_ContentOnly;
};


/********************************************************************************************

>	class XPFCContour : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Contour capabilities

********************************************************************************************/

class XPFCContour : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCContour);

public:
	XPFCContour(XPFConvertType ConvertType)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeContourController), ConvertType)
	{
	}
	virtual ~XPFCContour()
	{
	}

protected:
//	virtual BOOL DoesNodeMatch(Node* pNode);	// Just uses the ComplexClass version

protected:
};


/********************************************************************************************

>	class XPFCShadow : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Shadow capabilities

********************************************************************************************/

class XPFCShadow : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCShadow);

public:
	XPFCShadow(XPFConvertType ConvertType, XPFProp Type)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeShadowController), ConvertType)
	{
		m_Type = Type;
	}
	virtual ~XPFCShadow()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);

protected:
	XPFProp m_Type;
};


/********************************************************************************************

>	class XPFCBevel : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Bevel capabilities

********************************************************************************************/

class XPFCBevel : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCBevel);

public:
	XPFCBevel(XPFConvertType ConvertType, XPFProp Type, XPFProp Side)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeBevelController), ConvertType)
	{
		m_Type = Type;
		m_Side = Side;
	}
	virtual ~XPFCBevel()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);

protected:
	XPFProp m_Type;
	XPFProp m_Side;
};


/********************************************************************************************

>	class XPFCBlend : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Blend capabilities

********************************************************************************************/

class XPFCBlend : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCBlend);

public:
	XPFCBlend(XPFConvertType ConvertType, 
				XPFProp Effect,
				XPFBOOL bOnCurve,
				XPFProp bObjProfile,
				XPFProp bAttrProfile)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeBlend), ConvertType)
	{
		m_Effect = Effect;
		m_bOnCurve = bOnCurve;
		m_bObjProfile = bObjProfile;
		m_bAttrProfile = bAttrProfile;
	}
	virtual ~XPFCBlend()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);

protected:
	XPFProp m_Effect;
	XPFBOOL m_bOnCurve;
	XPFProp m_bObjProfile;
	XPFProp m_bAttrProfile;
};


/********************************************************************************************

>	class XPFCMould : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Mould capabilities

********************************************************************************************/

class XPFCMould : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCMould);

public:
	XPFCMould(XPFConvertType ConvertType, XPFProp Type, XPFBOOL bGradFill)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeMould), ConvertType)
	{
		m_Type = Type;
		m_bGradFill = bGradFill;
	}
	virtual ~XPFCMould()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);
	BOOL HasGraduatedFill(Node* pNode);

protected:
	XPFProp m_Type;
	XPFBOOL m_bGradFill;
};


/********************************************************************************************

>	class XPFCRectangle : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Rectangle capabilities

********************************************************************************************/

class XPFCRectangle : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCRectangle);

public:
	XPFCRectangle(XPFConvertType ConvertType, XPFBOOL bComplex, XPFBOOL bRounded, XPFBOOL bStellated, XPFBOOL bReformed)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeRegularShape), ConvertType)
	{
		m_bComplex = bComplex;
		m_bRounded = bRounded;
		m_bStellated = bStellated;
		m_bReformed = bReformed;
	}
	virtual ~XPFCRectangle()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);

protected:
	XPFBOOL m_bComplex;
	XPFBOOL m_bRounded;
	XPFBOOL m_bStellated;
	XPFBOOL m_bReformed;
};


/********************************************************************************************

>	class XPFCEllipse : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Ellipse capabilities

********************************************************************************************/

class XPFCEllipse : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCEllipse);

public:
	XPFCEllipse(XPFConvertType ConvertType, XPFBOOL bComplex)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeRegularShape), ConvertType)
	{
		m_bComplex = bComplex;
	}
	virtual ~XPFCEllipse()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);

protected:
	XPFBOOL m_bComplex;
};


/********************************************************************************************

>	class XPFCPolygon : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Polygon capabilities

********************************************************************************************/

class XPFCPolygon : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCPolygon);

public:
	XPFCPolygon(XPFConvertType ConvertType, XPFBOOL bRounded, XPFBOOL bStellated, XPFBOOL bReformed)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeRegularShape), ConvertType)
	{
		m_bRounded = bRounded;
		m_bStellated = bStellated;
		m_bReformed = bReformed;
	}
	virtual ~XPFCPolygon()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);

protected:
	XPFBOOL m_bRounded;
	XPFBOOL m_bStellated;
	XPFBOOL m_bReformed;
};


/********************************************************************************************

>	class XPFCBitmap : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Bitmap capabilities

********************************************************************************************/

class XPFCBitmap : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCBitmap);

public:
	XPFCBitmap(XPFConvertType ConvertType, XPFBOOL bComplex, XPFBOOL bContone)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeBitmap), ConvertType)
	{
		m_bComplex = bComplex;
		m_bContone = bContone;
	}
	virtual ~XPFCBitmap()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);

protected:
	XPFBOOL m_bComplex;
	XPFBOOL m_bContone;
};


/********************************************************************************************

>	class XPFCText : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Text capabilities

********************************************************************************************/

class XPFCText : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCText);

public:
	XPFCText(XPFConvertType ConvertType, XPFBOOL bOnPath, XPFBOOL bComplex, XPFBOOL bPlain, XPFBOOL bAutoKern, XPFBOOL bJustified)
		: XPFCComplexClass(CC_RUNTIME_CLASS(TextStory), ConvertType)
	{
		m_bOnPath = bOnPath;
		m_bComplex = bComplex;
		m_bPlain = bPlain;
		m_bAutoKern = bAutoKern;
		m_bJustified = bJustified;
	}
	virtual ~XPFCText()
	{
	}

protected:
	virtual BOOL DoesNodeMatch(Node* pNode);
	BOOL IsNodeJustified(Node* pNode);

protected:
	XPFBOOL m_bOnPath;
	XPFBOOL m_bComplex;
	XPFBOOL m_bPlain;
	XPFBOOL m_bAutoKern;
	XPFBOOL m_bJustified;
};



/********************************************************************************************

>	class XPFCClipView : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for ClipView capabilities

********************************************************************************************/

class XPFCClipView : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCClipView);

public:
	XPFCClipView(XPFConvertType ConvertType)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeClipViewController), ConvertType)
	{
	}
	virtual ~XPFCClipView()
	{
	}

protected:
//	virtual BOOL DoesNodeMatch(Node* pNode);	// Just uses the ComplexClass version

protected:
};


/********************************************************************************************

>	class XPFCBitmapEffect : public XPFCComplexClass

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for BitmapEffect capabilities

********************************************************************************************/

class XPFCBitmapEffect : public XPFCComplexClass
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCBitmapEffect);

public:
	XPFCBitmapEffect(XPFConvertType ConvertType)
		: XPFCComplexClass(CC_RUNTIME_CLASS(NodeBitmapEffect), ConvertType)
	{
	}
	virtual ~XPFCBitmapEffect()
	{
	}

protected:
//	virtual BOOL DoesNodeMatch(Node* pNode);	// Just uses the ComplexClass version for now

protected:
};



/********************************************************************************************

>	class XPFCFill : public XPFCapability

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Fill capabilities

********************************************************************************************/

class XPFCFill : public XPFCapability
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCFill);

public:
	XPFCFill(XPFConvertType ConvertType, XPFProp Shape, XPFProp Repeat, XPFBOOL bMultistage, XPFProp Effect, XPFBOOL bProfile, XPFBOOL bContone)
		: XPFCapability(ConvertType)
	{
		m_Shape = Shape;
		m_Repeat = Repeat;
		m_bMultistage = bMultistage;
		m_Effect = Effect;
		m_bProfile = bProfile;
		m_bContone = bContone;
	}
	virtual ~XPFCFill()
	{
	}


protected:
	virtual BOOL DoAttributesMatch(RenderRegion* pRegion);

protected:
	XPFProp m_Shape;
	XPFProp m_Repeat;
	XPFBOOL m_bMultistage;
	XPFProp m_Effect;
	XPFBOOL m_bProfile;
	XPFBOOL m_bContone;
};


/********************************************************************************************

>	class XPFCFillTrans : public XPFCapability

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for FillTrans capabilities

********************************************************************************************/

class XPFCFillTrans : public XPFCapability
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCFillTrans);

public:
	XPFCFillTrans(XPFConvertType ConvertType, XPFProp Shape, XPFProp Type, XPFProp Repeat, XPFBOOL bProfile)
		: XPFCapability(ConvertType)
	{
		m_Shape = Shape;
		m_Type = Type;
		m_Repeat = Repeat;
		m_bProfile = bProfile;
	}
	virtual ~XPFCFillTrans()
	{
	}


protected:
	virtual BOOL DoAttributesMatch(RenderRegion* pRegion);

protected:
	XPFProp m_Shape;
	XPFProp m_Type;
	XPFProp m_Repeat;
	XPFBOOL m_bProfile;
};


/********************************************************************************************

>	class XPFCLine : public XPFCapability

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Line capabilities

********************************************************************************************/

class XPFCLine : public XPFCapability
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCLine);

public:
	XPFCLine(XPFConvertType ConvertType, XPFBOOL bDash, XPFBOOL bArrowhead, XPFProp Cap, XPFProp Join, XPFBOOL bStroke, XPFBOOL bBrush)
		: XPFCapability(ConvertType)
	{
		m_bDash = bDash;
		m_bArrowhead = bArrowhead;
		m_Cap = Cap;
		m_Join = Join;
		m_bStroke = bStroke;
		m_bBrush = bBrush;
	}
	virtual ~XPFCLine()
	{
	}


protected:
	virtual BOOL DoAttributesMatch(RenderRegion* pRegion);

protected:
	XPFBOOL m_bDash;
	XPFBOOL m_bArrowhead;
	XPFProp m_Cap;
	XPFProp m_Join;
	XPFBOOL m_bStroke;
	XPFBOOL m_bBrush;
};


/********************************************************************************************

>	class XPFCLineTrans : public XPFCapability

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for LineTrans capabilities

********************************************************************************************/

class XPFCLineTrans : public XPFCapability
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCLineTrans);

public:
	XPFCLineTrans(XPFConvertType ConvertType, XPFProp Type)
		: XPFCapability(ConvertType)
	{
		m_Type = Type;
	}
	virtual ~XPFCLineTrans()
	{
	}


protected:
	virtual BOOL DoAttributesMatch(RenderRegion* pRegion);

protected:
	XPFProp m_Type;
};


/********************************************************************************************

>	class XPFCFeather : public XPFCapability

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Feather capabilities

********************************************************************************************/

class XPFCFeather : public XPFCapability
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCFeather);

public:
	XPFCFeather(XPFConvertType ConvertType)
		: XPFCapability(ConvertType)
	{
	}
	virtual ~XPFCFeather()
	{
	}


protected:
	virtual BOOL DoesNodeMatch(Node* pNode);
	virtual BOOL DoAttributesMatch(RenderRegion* pRegion);

protected:
};



/********************************************************************************************

>	class XPFCColour : public XPFCapability

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class for Colour capabilities

********************************************************************************************/

class XPFCColour : public XPFCapability
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(XPFCColour);

public:
	XPFCColour(XPFConvertType ConvertType)
		: XPFCapability(ConvertType)
	{
	}
	virtual ~XPFCColour()
	{
	}


protected:
	virtual BOOL DoesColourMatch(DocColour* pColour) { return(TRUE); }

protected:
};





/********************************************************************************************

>	class CapabilityTree : public CC_CLASS_MEMDUMP

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/05
	Purpose:	This is the class representing the wntire capability tree

********************************************************************************************/

class CapabilityTree : public CC_CLASS_MEMDUMP
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP(CapabilityTree);

public:
	CapabilityTree()
	{
		// Set some sensible defaults (these will result in no conversions at all)
		m_BoundsLevel = BWL_NONE;
		m_bSelection = FALSE;
		m_RasteriseDPI = 96.0;
		m_bRasteriseAlpha = TRUE;
		m_BitmapCompression = 200;
		m_SpreadType = XPFCONVTYPE_NATIVE;
		m_pObjects = NULL;
		m_ObjectsType = XPFCONVTYPE_NATIVE;
		m_pAttributes = NULL;
		m_AttributesType = XPFCONVTYPE_NATIVE;
		m_pColours = NULL;
		m_ColoursType = XPFCONVTYPE_NATIVE;
	}
	virtual ~CapabilityTree()
	{
		// Clean up the capability trees
		XPFCapability* pNext = NULL;
		XPFCapability* pCap = m_pObjects;
		while (pCap)
		{
			pNext = pCap->GetNext();
			delete pCap;
			pCap = pNext;
		}

		pCap = m_pAttributes;
		while (pCap)
		{
			pNext = pCap->GetNext();
			delete pCap;
			pCap = pNext;
		}

		pCap = m_pColours;
		while (pCap)
		{
			pNext = pCap->GetNext();
			delete pCap;
			pCap = pNext;
		}
	}

	XPFConvertType GetConvertTypePass1(Node* pNode, XPFRenderRegion* pRegion);
	XPFConvertType GetConvertTypePass2(Node* pNode, XPFRenderRegion* pRegion);
	void GetConvertTypePass3(Node* pNode, XPFRenderRegion* pRegion, BOOL* pbFill, BOOL* pbTrans, BOOL* pbFillTrans);
	XPFConvertType GetConvertTypePass4(Node* pNode, XPFRenderRegion* pRegion);
	XPFConvertType GetConvertTypePass5(Node* pNode, XPFRenderRegion* pRegion);

	void SetSelection(BOOL bSelection) { m_bSelection = bSelection; }
	void SetBoundsLevel(BoundsWriteLevel Level) { m_BoundsLevel = Level; }
	
	void SetSpreadType(XPFConvertType Type) { m_SpreadType = Type; }

	void SetRasterise(double DPI, BOOL bAlpha, INT32 Compression, const String_256& CommonTrans)
	{
		m_RasteriseDPI = DPI;
		m_bRasteriseAlpha = bAlpha;
		m_BitmapCompression = Compression;
		m_CommonTrans = CommonTrans;
	}

	void SetObjectsTree(XPFCapability* pObjects, XPFConvertType ObjectsType)
	{
		ERROR3IF(m_pObjects, "Already have an Objects tree");
		m_pObjects = pObjects;
		m_ObjectsType = ObjectsType;
	}

	void SetAttributesTree(XPFCapability* pAttributes, XPFConvertType AttributesType)
	{
		ERROR3IF(m_pAttributes, "Already have an Attributes tree");
		m_pAttributes = pAttributes;
		m_AttributesType = AttributesType;
	}

	void SetColoursTree(XPFCapability* pColours, XPFConvertType ColoursType)
	{
		ERROR3IF(m_pColours, "Already have a Colours tree");
		m_pColours = pColours;
		m_ColoursType = ColoursType;
	}

	BOOL GetSelection() { return(m_bSelection); }
	BoundsWriteLevel GetBoundsLevel() { return(m_BoundsLevel); }

	double GetRasteriseDPI() { return(m_RasteriseDPI); }
	BOOL GetRasteriseAlpha() { return(m_bRasteriseAlpha); }
	BOOL GetBitmapCompression() { return(m_BitmapCompression); }
	BOOL HasRasteriseCommonTrans() { return(!m_CommonTrans.IsEmpty()); }
	// Move this into cpp file when implemented
	BOOL IsRasteriseCommonTrans(UINT32 Type);

	XPFCapability* GetObjects(void) const { return(m_pObjects); }
	XPFCapability* GetAttributes(void) const { return(m_pAttributes); }
	XPFCapability* GetColours(void) const { return(m_pColours); }

	XPFConvertType GetObjectsType(void) const { return(m_ObjectsType); }
	XPFConvertType GetAttributesType(void) const { return(m_AttributesType); }
	XPFConvertType GetColoursType(void) const { return(m_ColoursType); }


protected:
	BoundsWriteLevel m_BoundsLevel;
	BOOL m_bSelection;

	double m_RasteriseDPI;
	BOOL m_bRasteriseAlpha;
	INT32 m_BitmapCompression;
	String_256 m_CommonTrans;

	XPFConvertType m_SpreadType;
	
	XPFCapability* m_pObjects;
	XPFConvertType m_ObjectsType;
	XPFCapability* m_pAttributes;
	XPFConvertType m_AttributesType;
	XPFCapability* m_pColours;
	XPFConvertType m_ColoursType;
};


#endif  // INC_XPFCAPS
