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

//This file contains a definition of the TextStory attributes, both the Attribute
//value classes and the Node classes

// ** Important **
//Here are some naming conventions for you to break, there a bit odd but at least they conform
//to the way other attribs have been named. 

//All attribute values have the form Txt<AttribName>Attribute
//All attribute nodes have the form AttrTxt<AttribName>
  
// Here is a list of the attributes defined in this file

//Value	Class						Node Class					New file format export class				  

//TxtFontTypefaceAttribute			AttrTxtFontTypeface			CXaraFileTxtFontTypeface
//TxtBoldAttribute					AttrTxtBold					CXaraFileTxtBold
//TxtItalicAttribute				AttrTxtItalic				CXaraFileTxtItalic
//TxtUnderlineAttribute				AttrTxtUnderline			CXaraFileTxtUnderline
//TxtAspectRatioAttribute			AttrTxtAspectRatio			CXaraFileTxtAspectRatio
//TxtJustificationAttribute			AttrTxtJustification		CXaraFileTxtJustification
//TxtTrackingAttribute				AttrTxtTracking				CXaraFileTxtTracking
//TxtFontSizeAttribute				AttrTxtFontSize				CXaraFileTxtFontSize
//TxtScriptAttribute				AttrTxtScript				CXaraFileTxtScript
//TxtBaseLineAttribute				AttrTxtBaseLine				CXaraFileTxtBaseLine
//TxtLineSpaceAttribute				AttrTxtLineSpace			CXaraFileTxtLineSpace

// The export code for the new file format has been placed in separate classes, in another
// file, to avoid the monster rebuilds generated when this file gets changed.

/*
*/

#ifndef INC_TXTATTR
#define INC_TXTATTR

#include "node.h"
#include "attrval.h"
#include "nodeattr.h"
#include "trans2d.h"
#include "fixed16.h"

#include <list>

enum Justification { JLEFT, JRIGHT, JCENTRE, JFULL }; 
enum TxtTabType { LeftTab, RightTab, CentreTab, DecimalTab };
	
// Attribute Value classes

/********************************************************************************************

>	class TxtBaseClassAttribute : public AttributeValue
	   		
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	The base class from which all text attributes are derived
	SeeAlso:	AttrTxtFontTypeface

********************************************************************************************/

class TxtBaseClassAttribute : public AttributeValue
{
	CC_DECLARE_DYNAMIC(TxtBaseClassAttribute)
public:
	// Constructor
	TxtBaseClassAttribute() {};

	virtual void Restore(RenderRegion *, BOOL) = 0;
};

/********************************************************************************************

>	class TxtFontTypefaceAttribute : public TxtBaseClassAttribute
	   		
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	Specifies a FontTypeface. Note that only a handle to the Typeface is
				stored in the attribute, not the full string description.
	SeeAlso:	AttrTxtFontTypeface

********************************************************************************************/

class TxtFontTypefaceAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtFontTypefaceAttribute)
public:
	// Constructors
	TxtFontTypefaceAttribute();
	TxtFontTypefaceAttribute(UINT32 TypefaceHandle);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtFontTypefaceAttribute& operator=(TxtFontTypefaceAttribute& Attrib);
	INT32 operator==(const TxtFontTypefaceAttribute& Attrib);
 

	// The typeface handle
	WORD HTypeface;
	BOOL IsBold;
	BOOL IsItalic;

	static BOOL Init();
};	

/********************************************************************************************

>	class TxtFontSizeAttribute : public TxtBaseClassAttribute
	   		
	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	Specifies the size of a font
	SeeAlso:	AttrTxtFontTypeface

********************************************************************************************/

class TxtFontSizeAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtFontSizeAttribute)
public:
	// Constructors
	TxtFontSizeAttribute();
	TxtFontSizeAttribute(MILLIPOINT FontSize);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtFontSizeAttribute& operator=(TxtFontSizeAttribute& Attrib);
	INT32 operator==(const TxtFontSizeAttribute& Attrib);

	// The typeface handle
	MILLIPOINT FontSize;

	static BOOL Init();
};	



/********************************************************************************************

>	class TxtBoldAttribute : public TxtBaseClassAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Purpose:	This text attribute specifies the BOLD state (on/off)
	SeeAlso:	-

********************************************************************************************/


class TxtBoldAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtBoldAttribute)
public:
	// Constructors
	TxtBoldAttribute();
	TxtBoldAttribute(BOOL boldOn);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtBoldAttribute& operator=(TxtBoldAttribute& Attrib); 
	INT32 operator==(const TxtBoldAttribute& Attrib);


	BOOL BoldOn;

	static BOOL Init();
};

/********************************************************************************************

>	class TxtItalicAttribute : public TxtBaseClassAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Purpose:	This text attribute specifies the ITALIC state (on/off)
	SeeAlso:	-

********************************************************************************************/


class TxtItalicAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtItalicAttribute)
public:
	// Constructors
	TxtItalicAttribute();
	TxtItalicAttribute(BOOL italicOn);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtItalicAttribute& operator=(TxtItalicAttribute& Attrib); 
	INT32 operator==(const TxtItalicAttribute& Attrib);


	// The typeface handle
	BOOL ItalicOn;

	static BOOL Init();
};	

/********************************************************************************************

>	class TxtUnderlineAttribute : public TxtBaseClassAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Purpose:	This text attribute specifies if text is underlined or not
	SeeAlso:	-

********************************************************************************************/


class TxtUnderlineAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtUnderlineAttribute)
public:												  
	TxtUnderlineAttribute();
	TxtUnderlineAttribute(BOOL underlined);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtUnderlineAttribute& operator=(TxtUnderlineAttribute& Attrib); 
	INT32 operator==(const TxtUnderlineAttribute& Attrib);

	BOOL Underlined;

	static BOOL Init();
};	

/********************************************************************************************

>	class TxtJustificationAttribute : public TxtBaseClassAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Purpose:	This text attribute specifies the current text justification
	SeeAlso:	-

********************************************************************************************/


class TxtJustificationAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtJustificationAttribute)
public:
	// Constructors
	TxtJustificationAttribute();
	TxtJustificationAttribute(Justification Value);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtJustificationAttribute& operator=(TxtJustificationAttribute& Attrib);
	INT32 operator==(const TxtJustificationAttribute& Attrib);

	static BOOL Init();

	Justification justification;  // JLEFT, JRIGHT, JCENTRE or JFULL
};	


	
/********************************************************************************************

>	class TxtTrackingAttribute: public TxtBaseClassAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Purpose:	This text attribute specifies a spacing between characters, it can be
				positive or negative.
	SeeAlso:	-

********************************************************************************************/


class TxtTrackingAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtTrackingAttribute)
public:
	// Constructors
	TxtTrackingAttribute();
	TxtTrackingAttribute(MILLIPOINT tracking);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtTrackingAttribute& operator=(TxtTrackingAttribute& Attrib);
	INT32 operator==(const TxtTrackingAttribute& Attrib);
 

	static BOOL Init();

	MILLIPOINT Tracking; 
};	

/********************************************************************************************

>	class TxtAspectRatioAttribute: public TxtBaseClassAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Purpose:	This text attribute specifies the aspect ratio of characters
	SeeAlso:	-

********************************************************************************************/


class TxtAspectRatioAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtAspectRatioAttribute)
public:
	// Constructors
	TxtAspectRatioAttribute();
	TxtAspectRatioAttribute(FIXED16 aspect);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtAspectRatioAttribute& operator=(TxtAspectRatioAttribute& Attrib);
	INT32 operator==(const TxtAspectRatioAttribute& Attrib);
 
	static BOOL Init();

	FIXED16 AspectRatio;
};	

/********************************************************************************************

>	class TxtScriptAttribute: public TxtBaseClassAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Purpose:	This text attribute specifies the superscript/subscript value of a char
	SeeAlso:	-

********************************************************************************************/


class TxtScriptAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtScriptAttribute)
public:
	// Constructors
	TxtScriptAttribute();
	TxtScriptAttribute(FIXED16 offset, FIXED16 size);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtScriptAttribute& operator=(TxtScriptAttribute& Attrib);
	INT32 operator==(const TxtScriptAttribute& Attrib);
 
	static BOOL Init();

	FIXED16 Offset;	// Relative to height of char +/-
	FIXED16 Size; 	// Ralative to font size 
};	

/********************************************************************************************

>	class TxtBaseLineAttribute: public TxtBaseClassAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Purpose:	This text attribute specifies the aspect ratio of characters
	SeeAlso:	-

********************************************************************************************/


class TxtBaseLineAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtBaseLineAttribute)
public:
	// Constructors
	TxtBaseLineAttribute();
	TxtBaseLineAttribute(MILLIPOINT value);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtBaseLineAttribute& operator=(TxtBaseLineAttribute& Attrib);
	INT32 operator==(const TxtBaseLineAttribute& Attrib);
 
	static BOOL Init();

	MILLIPOINT Value;  // Absolute offset from base line of line
};	

/********************************************************************************************

>	class TxtLineSpaceAttribute: public TxtBaseClassAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Purpose:	This text attribute specifies the aspect ratio of characters
	SeeAlso:	-

********************************************************************************************/


class TxtLineSpaceAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtLineSpaceAttribute)
public:
	// Constructors
	TxtLineSpaceAttribute();
	TxtLineSpaceAttribute(MILLIPOINT Value);
	TxtLineSpaceAttribute(FIXED16 Ratio);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtLineSpaceAttribute& operator=(TxtLineSpaceAttribute& Attrib);
	INT32 operator==(const TxtLineSpaceAttribute& Attrib);
 
	static BOOL Init();
	
	BOOL IsARatio() const;


	BOOL IsRatio:1;

	MILLIPOINT Value; // Absolute distance between lines
	FIXED16 Ratio;
	
};

/********************************************************************************************

>	class TxtTabStop

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/6/06
	Purpose:	This type represents a tab stop (part of a ruler attribute)

********************************************************************************************/

class TxtTabStop
{
public:
	// standard constructor - fills in decimal point and tab chars automatically
	TxtTabStop(TxtTabType TheType, MILLIPOINT ThePosition);
	// full constructor
	TxtTabStop(TxtTabType TheType, MILLIPOINT ThePosition, WCHAR DecimalPointChar, WCHAR TabFillerChar);

	// accessor functions
	inline TxtTabType GetType() const { return mType; }
	void SetType(TxtTabType TheType);
	inline MILLIPOINT GetPosition() const { return mPosition; }
	inline void SetPosition(MILLIPOINT ThePosition) { mPosition = ThePosition; }
	inline WCHAR GetDecimalPointChar() const { return mDecimalPointChar; }
	inline void SetDecimalPointChar(WCHAR DecimalPointChar) { mDecimalPointChar = DecimalPointChar; }
	inline WCHAR GetTabFillerChar() const { return mTabFillerChar; }
	inline void SetTabFillerChar(WCHAR TheChar) { mTabFillerChar = TheChar; }

 private:
	TxtTabType mType;
	MILLIPOINT mPosition;
	WCHAR mDecimalPointChar;  // the character to align to (for decimal tabs only)
	WCHAR mTabFillerChar;     // the tab filler character (0 for none)
};

// comparison operators
BOOL operator==(const TxtTabStop& t1, const TxtTabStop& t2);
BOOL operator!=(const TxtTabStop& t1, const TxtTabStop& t2);

typedef list<TxtTabStop> TxtRuler;

// iterators for the tab stop list used by TxtRulerAttribute
typedef list<TxtTabStop>::iterator TxtTabStopIterator;
typedef list<TxtTabStop>::const_iterator const_TxtTabStopIterator;

/********************************************************************************************

>	class TxtRulerAttribute: public TxtBaseClassAttribute

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/6/06
	Purpose:	This text attribute specifies the ruler (i.e., tab stops)
				Conceptually, a ruler is just a pointer to a sorted list of tab stops.				
	SeeAlso:	-

********************************************************************************************/

class TxtRulerAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtRulerAttribute)
public:
	// Constructor
	TxtRulerAttribute();
	~TxtRulerAttribute();

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtRulerAttribute& operator=(TxtRulerAttribute& Attrib);
	INT32 operator==(const TxtRulerAttribute& Attrib);

	// access functions for the tab stop list
	void AddTabStop(TxtTabType Type, MILLIPOINT Position);
	void AddTabStop(TxtTabType Type, MILLIPOINT Position, WCHAR DecimalPointChar, WCHAR TabFillerChar);
	void AddTabStop(TxtTabStop TabStop);
	void FindTabStop (MILLIPOINT MinPos, TxtTabType* pType, MILLIPOINT* pPos) const;
	const_TxtTabStopIterator begin() const { return Value->begin(); }
	const_TxtTabStopIterator end() const { return Value->end(); }
	TxtTabStopIterator begin() { return Value->begin(); }
	TxtTabStopIterator end() { return Value->end(); }
	INT32 NumTabStops() const { return Value->size(); }

	static BOOL Init();
	static void FindTabStopInRuler(const TxtRuler* pRuler, MILLIPOINT MinPos,
								   TxtTabType* pType, MILLIPOINT* pPos);
	// the actual value is a pointer to a list of tab stops
	// this means that we will have to take care when destructing/copying
	// objects of this class
	TxtRuler* Value;
private:
	// prevent accidental copy construction
	TxtRulerAttribute(const TxtRulerAttribute&);
};

/********************************************************************************************

>	class TxtLeftMarginAttribute: public TxtBaseClassAttribute

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/6/06
	Purpose:	This text attribute specifies the left margin
	SeeAlso:	-

********************************************************************************************/

class TxtLeftMarginAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtLeftMarginAttribute)
public:
	// Constructors
	TxtLeftMarginAttribute();
	TxtLeftMarginAttribute(MILLIPOINT value);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtLeftMarginAttribute& operator=(TxtLeftMarginAttribute& Attrib);
	INT32 operator==(const TxtLeftMarginAttribute& Attrib);

	static BOOL Init();

	MILLIPOINT Value;
};

/********************************************************************************************

>	class TxtRightMarginAttribute: public TxtBaseClassAttribute

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/6/06
	Purpose:	This text attribute specifies the right margin
	SeeAlso:	-

********************************************************************************************/

class TxtRightMarginAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtRightMarginAttribute)
public:
	// Constructors
	TxtRightMarginAttribute();
	TxtRightMarginAttribute(MILLIPOINT value);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtRightMarginAttribute& operator=(TxtRightMarginAttribute& Attrib);
	INT32 operator==(const TxtRightMarginAttribute& Attrib);

	static BOOL Init();

	MILLIPOINT Value;
};

/********************************************************************************************

>	class TxtFirstIndentAttribute: public TxtBaseClassAttribute

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/6/06
	Purpose:	This text attribute specifies the first line indent
	SeeAlso:	-

********************************************************************************************/

class TxtFirstIndentAttribute : public TxtBaseClassAttribute
{
	CC_DECLARE_DYNCREATE(TxtFirstIndentAttribute)
public:
	// Constructors
	TxtFirstIndentAttribute();
	TxtFirstIndentAttribute(MILLIPOINT value);

	virtual void Restore (RenderRegion *, BOOL);
	virtual void Render(RenderRegion *pRegion, BOOL Temp = FALSE);

	virtual void SimpleCopy(AttributeValue *);
	virtual NodeAttribute *MakeNode();

	BOOL IsDifferent(AttributeValue *pAttr);

	virtual TxtFirstIndentAttribute& operator=(TxtFirstIndentAttribute& Attrib);
	INT32 operator==(const TxtFirstIndentAttribute& Attrib);

	static BOOL Init();

	MILLIPOINT Value;
};

// ---------------------------------------------------------------------------------------------
// Attribute Node classes

/***********************************************************************************************

>	class AttrTxtBase : public NodeAttribute

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtBase is the base class from which all text attribute nodes are derived

***********************************************************************************************/

class AttrTxtBase : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrTxtBase)

public:
	// Constructors
	AttrTxtBase();
	AttrTxtBase(Node* ContextNode,  
			  	AttachNodeDirection Direction,    
		      	BOOL Locked=FALSE, 
		      	BOOL Mangled=FALSE,  
		      	BOOL Marked=FALSE, 
			  	BOOL Selected=FALSE);

	virtual BOOL IsKindOfTextAttribute() const { return TRUE; }

	virtual BOOL EffectsParentBounds() { return TRUE; };
	virtual void BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect) { return; }

};

/***********************************************************************************************

>	class AttrTxtFontTypeface : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtFontTypeface specifies a font typeface

***********************************************************************************************/

class AttrTxtFontTypeface : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtFontTypeface)

public:
	// Constructors
	AttrTxtFontTypeface(); 
	AttrTxtFontTypeface(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTFONTTYPEFACE; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	void SetBold(BOOL State)	{ Value.IsBold = State; }
	void SetItalic(BOOL State)	{ Value.IsItalic = State; }
	BOOL IsBold()				{ return Value.IsBold; }
	BOOL IsItalic()				{ return Value.IsItalic; }
	BOOL IsATypeface() const	{ return TRUE; }

	// needs to be called _everytime_ objects of this type get created or updated.
	void UpdateCachedData();
	
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtFontTypeface* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtFontTypefaceAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtBold : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtBold specifies if text is bold or not

***********************************************************************************************/

class AttrTxtBold : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtBold)

public:
	// Constructors
	AttrTxtBold(); 
	AttrTxtBold(Node* ContextNode,  
			  	AttachNodeDirection Direction,    
		      	BOOL Locked=FALSE, 
		      	BOOL Mangled=FALSE,  
		      	BOOL Marked=FALSE, 
			  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTBOLD; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtBold* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtBoldAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtItalic : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtFontTypeface specifies if text is italic or not

***********************************************************************************************/

class AttrTxtItalic : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtItalic)

public:
	// Constructors
	AttrTxtItalic(); 
	AttrTxtItalic(Node* ContextNode,  
				  	AttachNodeDirection Direction,    
			      	BOOL Locked=FALSE, 
			      	BOOL Mangled=FALSE,  
			      	BOOL Marked=FALSE, 
				  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTITALIC; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtItalic* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtItalicAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtUnderline : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtUnderline specifies if text is underlined or not

***********************************************************************************************/

class AttrTxtUnderline : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtUnderline)

public:
	// Constructors
	AttrTxtUnderline(); 
	AttrTxtUnderline(Node* ContextNode,  
				  	AttachNodeDirection Direction,    
			      	BOOL Locked=FALSE, 
			      	BOOL Mangled=FALSE,  
			      	BOOL Marked=FALSE, 
				  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTUNDERLINE; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtUnderline* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtUnderlineAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtAspectRatio : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtAspectRatio specifies the aspect ratio of text

***********************************************************************************************/

class AttrTxtAspectRatio : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtAspectRatio)

public:
	// Constructors
	AttrTxtAspectRatio(); 
	AttrTxtAspectRatio(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTASPECTRATIO; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual void BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtAspectRatio* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtAspectRatioAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtJustification : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtJustification specifies how text is justified

***********************************************************************************************/

class AttrTxtJustification : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtJustification)

public:
	// Constructors
	AttrTxtJustification(); 
	AttrTxtJustification(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTJUSTIFICATION; }

	virtual BOOL IsALineLevelAttrib() {return TRUE;};

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtJustification* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtJustificationAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtTracking : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtTracking specifies text tracking

***********************************************************************************************/

class AttrTxtTracking : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtTracking)

public:
	// Constructors
	AttrTxtTracking(); 
	AttrTxtTracking(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTTRACKING; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtTracking* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtTrackingAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtFontSize : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtFontSize: specifies the size of a font (height)

***********************************************************************************************/

class AttrTxtFontSize : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtFontSize)

public:
	// Constructors
	AttrTxtFontSize(); 
	AttrTxtFontSize(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTFONTSIZE; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual void BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtFontSize* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtFontSizeAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtScript : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtScript: Specifies sub/super script

***********************************************************************************************/

class AttrTxtScript : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtScript)

public:
	// Constructors
	AttrTxtScript(); 
	AttrTxtScript(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTSCRIPT; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtScript* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtScriptAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtBaseLine : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtBaseLine: specifies an absolute offset from the base-line

***********************************************************************************************/

class AttrTxtBaseLine : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtBaseLine)

public:
	// Constructors
	AttrTxtBaseLine(); 
	AttrTxtBaseLine(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTBASELINE; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual void BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtBaseLine* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtBaseLineAttribute Value;
};

/***********************************************************************************************

>	class AttrTxtLeftMargin : public AttrTxtBase

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/06/06
	Purpose:	AttrTxtLeftMargin: specifies the left margin

***********************************************************************************************/

class AttrTxtLeftMargin : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtLeftMargin)

public:
	// Constructors
	AttrTxtLeftMargin() {}
	AttrTxtLeftMargin(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTLEFTMARGIN; }

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual BOOL IsALineLevelAttrib() { return TRUE; }

	virtual void BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtLeftMargin* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtLeftMarginAttribute Value;
};

/***********************************************************************************************

>	class AttrTxtRightMargin : public AttrTxtBase

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/06/06
	Purpose:	AttrTxtRightMargin: specifies the right margin

***********************************************************************************************/

class AttrTxtRightMargin : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtRightMargin)

public:
	// Constructors
	AttrTxtRightMargin() {}
	AttrTxtRightMargin(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTRIGHTMARGIN; }

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual BOOL IsALineLevelAttrib() { return TRUE; }

	virtual void BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtRightMargin* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtRightMarginAttribute Value;
};

/***********************************************************************************************

>	class AttrTxtFirstIndent : public AttrTxtBase

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/06/06
	Purpose:	AttrTxtFirstIndent: specifies the first line indent

***********************************************************************************************/

class AttrTxtFirstIndent : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtFirstIndent)

public:
	// Constructors
	AttrTxtFirstIndent() {}
	AttrTxtFirstIndent(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTFIRSTINDENT; }

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual BOOL IsALineLevelAttrib() { return TRUE; }

	virtual void BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtFirstIndent* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtFirstIndentAttribute Value;
};

/***********************************************************************************************

>	class AttrTxtRuler : public AttrTxtBase

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/06/06
	Purpose:	AttrTxtRuler: specifies the ruler

***********************************************************************************************/

class AttrTxtRuler : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtRuler)

public:
	// Constructors
	AttrTxtRuler() {}
	AttrTxtRuler(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTRULER; }

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual BOOL IsALineLevelAttrib() { return TRUE; }

	virtual void BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtRuler* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtRulerAttribute Value;
};


/***********************************************************************************************

>	class AttrTxtLineSpace : public AttrTxtBase

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/03/95
	Purpose:	AttrTxtLineSpace: specifies the size of a font (height)

***********************************************************************************************/

class AttrTxtLineSpace : public AttrTxtBase
{
	CC_DECLARE_DYNCREATE(AttrTxtLineSpace)

public:
	// Constructors
	AttrTxtLineSpace();
	AttrTxtLineSpace(MILLIPOINT Value);
	AttrTxtLineSpace(FIXED16 Ratio);
 
	AttrTxtLineSpace(Node* ContextNode,  
					  	AttachNodeDirection Direction,    
				      	BOOL Locked=FALSE, 
				      	BOOL Mangled=FALSE,  
				      	BOOL Marked=FALSE, 
					  	BOOL Selected=FALSE);
			 

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node

	virtual INT32 operator==(const NodeAttribute& NodeAttrib); 

	virtual UINT32 GetAttrNameID(void); 
	virtual AttrIndex GetAttributeIndex () { return ATTR_TXTLINESPACE; }

	//void ShowDebugTreeDetails() const;			// Debugging function

	void GetDebugDetails(StringBase* Str);

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual BOOL IsALineLevelAttrib() {return TRUE;};

	BOOL IsARatio() { return Value.IsARatio(); }

	virtual void BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect);

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	void CopyNodeContents( AttrTxtLineSpace* NodeCopy );

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

public:
	TxtLineSpaceAttribute Value;
};

/********************************************************************************************

>	class CharDescription : public CCObject

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/95
	Purpose:	The CharDescription structure is used to identify a character within the
				FontCache.
	SeeAlso:	FontCache

********************************************************************************************/

/********************************************************************************************

>	WCHAR CharDescription::GetCharCode()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	UNICODE char code
	Purpose:	To extract the CharCode from a CharDescription
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/********************************************************************************************

>	WORD CharDescription:: GetTypefaceHandle()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	The typeface handle
	Purpose:	To extract the Typeface Handle from a CharDescription
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


/********************************************************************************************

>	BOOL CharDescription:: GetBold()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	The bold state
	Purpose:	To extract the Bold state from a CharDescription
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


/********************************************************************************************

>	BOOL CharDescription:: GetItalic()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	The italic state
	Purpose:	To extract the italic state from a CharDescription
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

class CharDescription:public CCObject
{
	CC_DECLARE_DYNCREATE(CharDescription)

	// Needs sorting out
	CharDescription() {};
	CharDescription(CharDescription& that);
	CharDescription(WCHAR CharCode, WORD TypefaceHandle, BOOL Bold, BOOL Italic);

	WCHAR GetCharCode() 	  { return (WCHAR)(charHandle >> 16); }
	WORD GetTypefaceHandle()  { return (WORD)(charHandle & 0x00003FFF); }
	BOOL GetBold()			  { return ((charHandle & 0x00008000) != 0); }
	BOOL GetItalic()		  { return ((charHandle & 0x00004000) != 0); }

public:
	UINT32 charHandle;	  // The char handle used in GCache 

	CharDescription& operator=(CharDescription& that) { this->charHandle =that.charHandle; return *this; }
	BOOL operator==(CharDescription& that) const { return this->charHandle==that.charHandle; }
	BOOL operator!=(CharDescription& that) const { return this->charHandle!=that.charHandle; }
};

#endif
