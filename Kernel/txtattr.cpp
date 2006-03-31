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

//This file contains the implementation of the TextStory attributes, both the Attribute
//value classes and the Node classes

// ** Important **
//Here are some naming conventions for you to break, there a bit odd but at least they conform
//to the way other attribs have been named. 

//All attribute values have the form Txt<AttribName>Attribute
//All attribute nodes have the form AttrTxt<AttribName>
  
// Here is a list of the attributes defined in this file
// The attribute values are defined first, followed by the NodeAttributes

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

// FontDataItemHandle

#include "camtypes.h"
#include "txtattr.h"
//#include "simon.h"
#include "rndrgn.h"
#include "attrmgr.h"
#include "ink.h"
//#include "mario.h"
//#include "peter.h"
#include "nodetxts.h"
#include "app.h"
#include "fontman.h"
#include "nodetext.h"
#include "cxftext.h"

DECLARE_SOURCE("$Revision$");


// All IMPLEMENTS here
CC_IMPLEMENT_DYNAMIC(TxtBaseClassAttribute,  	  AttributeValue)
CC_IMPLEMENT_DYNCREATE(TxtFontTypefaceAttribute,  TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtBoldAttribute, 		  TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtItalicAttribute, 		  TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtUnderlineAttribute, 	  TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtAspectRatioAttribute,   TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtJustificationAttribute, TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtTrackingAttribute, 	  TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtFontSizeAttribute, 	  TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtScriptAttribute, 		  TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtBaseLineAttribute, 	  TxtBaseClassAttribute)
CC_IMPLEMENT_DYNCREATE(TxtLineSpaceAttribute, 	  TxtBaseClassAttribute)


CC_IMPLEMENT_DYNCREATE(AttrTxtBase,  	  		  NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrTxtFontTypeface,  	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtBold, 		  	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtItalic, 		  	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtUnderline, 	 	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtAspectRatio,   	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtJustification, 	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtTracking, 	  	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtFontSize, 	  	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtScript,	 	  	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtBaseLine, 	  	  AttrTxtBase)
CC_IMPLEMENT_DYNCREATE(AttrTxtLineSpace, 	  	  AttrTxtBase)


CC_IMPLEMENT_DYNCREATE(CharDescription, 	  	  CCObject)


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// -------------------------------------------------------------------------------------------
// AttrTxtBase methods

AttrTxtBase::AttrTxtBase() { }
AttrTxtBase::AttrTxtBase(Node* ContextNode,  
						 AttachNodeDirection Direction,  
						 BOOL Locked, 
						 BOOL Mangled,  
  						 BOOL Marked, 
						 BOOL Selected    
			): NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

// -----------------------------------------------------------------------------------------
// TxtFontTypeface methods
 
/********************************************************************************************

>	TxtFontTypefaceAttribute::TxtFontTypefaceAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtFontTypefaceAttribute
				The default typeface handle is 0 which represents the default font
			
	SeeAlso:

********************************************************************************************/

TxtFontTypefaceAttribute::TxtFontTypefaceAttribute()
{
	HTypeface = 0; // Default font
	IsBold = FALSE;
	IsItalic = FALSE;
} 


/********************************************************************************************

>	TxtFontTypefaceAttribute::TxtFontTypefaceAttribute(UINT32 TypefaceHandle) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		TypeFaceHandle: A typeface handle value
	Outputs:	-
	Returns:	-
	Purpose:	TextFontTypefaceAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtFontTypefaceAttribute::TxtFontTypefaceAttribute(UINT32 TypefaceHandle) 
{ 
	HTypeface = (WORD)TypefaceHandle;
	IsBold = FALSE;
	IsItalic = FALSE;
}



/********************************************************************************************

>	void TxtFontTypefaceAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtFontTypeface attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtFontTypefaceAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtFontTypefaceAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtFontTypefaceAttribute::Restore called\n") );
	pRegion->SetTxtFontTypeface(this, Temp);
}

/********************************************************************************************

>	void TxtFontTypefaceAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtFontTypefaceAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtFontTypefaceAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtFontTypefaceAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtFontTypefaceAttribute::Restore called\n") );
	pRegion->RestoreTxtFontTypeface(this, Temp);
}

/********************************************************************************************

>	TxtFontTypefaceAttribute& TxtFontTypefaceAttribute::operator=(TxtFontTypefaceAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtFontTypefaceAttribute& TxtFontTypefaceAttribute::operator=(TxtFontTypefaceAttribute& Attrib)
{	 
	HTypeface = Attrib.HTypeface;
	IsBold = Attrib.IsBold;
	IsItalic = Attrib.IsItalic;

	return *this;
}

/********************************************************************************************

>	INT32 TxtFontTypefaceAttribute::operator==(const TxtFontTypefaceAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtFontTypefaceAttribute::operator==(const TxtFontTypefaceAttribute& Attrib)
{
	return (Attrib.HTypeface == HTypeface &&
			Attrib.IsBold == IsBold	  &&
			Attrib.IsItalic == IsItalic); 
}

/********************************************************************************************

>	void TxtFontTypefaceAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtFontTypefaceAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtFontTypefaceAttribute), 
		"Invalid Attribute value passed to TxtFontTypefaceAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtFontTypefaceAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtFontTypefaceAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtFontTypefaceAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtFontTypefaceAttribute)), 
		   		"Different attribute types in TxtFontTypefaceAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtFontTypefaceAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtFontTypefaceAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtFontTypefaceAttribute::Init()
{
	TxtFontTypefaceAttribute *pAttr = new TxtFontTypefaceAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtFontTypefaceAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtFontTypefaceAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTextFontTypeface node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtFontTypefaceAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtFontTypeface*  pAttr = new AttrTxtFontTypeface();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}


// -----------------------------------------------------------------------------------------
// TxtFontSizeAttribute methods
 
/********************************************************************************************

>	TxtFontSizeAttribute::TxtFontSizeAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtFontSizeAttribute
				The default FontSize attribute value is 16000mp (16pt)
			
	SeeAlso:

********************************************************************************************/

TxtFontSizeAttribute::TxtFontSizeAttribute()
{
	FontSize = 16000; // Default font
} 


/********************************************************************************************

>	TxtFontSizeAttribute::TxtFontSizeAttribute(MILLIPOINT FontSize) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		FontSize: The font size in mp
	Outputs:	-
	Returns:	-
	Purpose:	TextFontTypefaceAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtFontSizeAttribute::TxtFontSizeAttribute(MILLIPOINT fontSize) 
{ 
	FontSize = fontSize; 
}



/********************************************************************************************

>	void TxtFontSizeAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtFontSizeAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtFontSizeAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtFontSizeAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtFontSizeAttribute::Restore called\n") );
	pRegion->SetTxtFontSize(this, Temp);
}

/********************************************************************************************

>	void TxtFontSizeAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtFontSizeAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtFontSizeAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtFontSizeAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtFontSizeAttribute::Restore called\n") );
	pRegion->RestoreTxtFontSize(this, Temp);
}

/********************************************************************************************

>	TxtFontSizeAttribute& TxtFontSizeAttribute::operator=(TxtFontSizeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtFontSizeAttribute& TxtFontSizeAttribute::operator=(TxtFontSizeAttribute& Attrib)
{	 
	FontSize = Attrib.FontSize;
	return *this;
}

/********************************************************************************************

>	INT32 TxtFontSizeAttribute::operator==(const TxtFontSizeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtFontSizeAttribute::operator==(const TxtFontSizeAttribute& Attrib)
{
	return (Attrib.FontSize == FontSize); 
}

/********************************************************************************************

>	void TxtFontSizeAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtFontSizeAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtFontSizeAttribute), 
		"Invalid Attribute value passed to TxtFontSizeAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtFontSizeAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtFontSizeAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtFontSizeAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtFontSizeAttribute)), 
		   		"Different attribute types in TxtFontSizeAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtFontSizeAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtFontSizeAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtFontSizeAttribute::Init()
{
	TxtFontSizeAttribute *pAttr = new TxtFontSizeAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtFontSizeAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtFontSizeAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTextFontTypeface node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtFontSizeAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtFontSize*  pAttr = new AttrTxtFontSize();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}



// -----------------------------------------------------------------------------------------
// TxtBoldAttribute methods
 
/********************************************************************************************

>	TxtBoldAttribute::TxtBoldAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtBoldAttribute
				The Bold state is set to FALSE
			
	SeeAlso:

********************************************************************************************/

TxtBoldAttribute::TxtBoldAttribute()
{
	BoldOn = FALSE;
} 


/********************************************************************************************

>	TxtBoldAttribute::TxtBoldAttribute(BOOL boldOn) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		boldOn: BOOL value indicating the initial bold state
	Outputs:	-
	Returns:	-
	Purpose:	TxtBoldAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtBoldAttribute::TxtBoldAttribute(BOOL boldOn) 
{ 
	BoldOn = boldOn; 
}



/********************************************************************************************

>	void TxtBoldAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtBoldAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtBoldAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtBoldAttribute::Render - do nothing" )
//	TRACE( _T("Warning - TxtBoldAttribute::Render called\n") );
	pRegion->SetTxtBold(this, Temp);
}

/********************************************************************************************

>	void TxtBoldAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtBoldAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtBoldAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtBoldAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtBoldAttribute::Restore called\n") );
	pRegion->RestoreTxtBold(this, Temp);
}

/********************************************************************************************

>	TxtBoldAttribute& TxtBoldAttribute::operator=(TxtBoldAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtBoldAttribute& TxtBoldAttribute::operator=(TxtBoldAttribute& Attrib)
{	 
	BoldOn = Attrib.BoldOn;
	return *this;
}

/********************************************************************************************

>	INT32 TxtBoldAttribute::operator==(const TxtBoldAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtBoldAttribute::operator==(const TxtBoldAttribute& Attrib)
{
	return (Attrib.BoldOn == BoldOn); 
}

/********************************************************************************************

>	void TxtBoldAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtBoldAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtBoldAttribute), 
		"Invalid Attribute value passed to TxtBoldAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtBoldAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtBoldAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtBoldAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtBoldAttribute)), 
		   		"Different attribute types in TxtBoldAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtBoldAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtBoldAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtBoldAttribute::Init()
{
	TxtBoldAttribute *pAttr = new TxtBoldAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtBoldAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtBoldAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTextFontTypeface node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtBoldAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtBold*  pAttr = new AttrTxtBold();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}


// -----------------------------------------------------------------------------------------
// TxtItalicAttribute methods
 
/********************************************************************************************

>	TxtItalicAttribute::TxtItalicAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtItalicAttribute
				The default Italic state is FALSE
			
	SeeAlso:

********************************************************************************************/

TxtItalicAttribute::TxtItalicAttribute()
{
	ItalicOn = FALSE; 
} 


/********************************************************************************************

>	TxtItalicAttribute::TxtItalicAttribute(BOOL italicOn) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		italicOn: TRUE = Italic on, FALSE = Italic off
	Outputs:	-
	Returns:	-
	Purpose:	TextFontTypefaceAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtItalicAttribute::TxtItalicAttribute(BOOL italicOn) 
{ 
	ItalicOn = italicOn; 
}



/********************************************************************************************

>	void TxtItalicAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtFontTypeface attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtItalicAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtItalicAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtItalicAttribute::Restore called\n") );
	pRegion->SetTxtItalic(this, Temp);
}

/********************************************************************************************

>	void TxtItalicAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtItalicAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtItalicAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtItalicAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtItalicAttribute::Restore called\n") );
	pRegion->RestoreTxtItalic(this, Temp);
}

/********************************************************************************************

>	TxtItalicAttribute& TxtItalicAttribute::operator=(TxtItalicAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtItalicAttribute& TxtItalicAttribute::operator=(TxtItalicAttribute& Attrib)
{	 
	ItalicOn = Attrib.ItalicOn;
	return *this;
}

/********************************************************************************************

>	INT32 TxtItalicAttribute::operator==(const TxtItalicAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtItalicAttribute::operator==(const TxtItalicAttribute& Attrib)
{
	return (Attrib.ItalicOn == ItalicOn); 
}

/********************************************************************************************

>	void TxtItalicAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtItalicAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtItalicAttribute), 
		"Invalid Attribute value passed to TxtItalicAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtItalicAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtItalicAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtItalicAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtItalicAttribute)), 
		   		"Different attribute types in TxtItalicAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtItalicAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtItalicAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtItalicAttribute::Init()
{
	TxtItalicAttribute *pAttr = new TxtItalicAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtItalicAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtItalicAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTxtItalic node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtItalicAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtItalic*  pAttr = new AttrTxtItalic();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}


// -----------------------------------------------------------------------------------------
// TxtUnderlineAttribute methods
 
/********************************************************************************************

>	TxtUnderlineAttribute::TxtUnderlineAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtUnderlineAttribute
				The default Underline state is FALSE
			
	SeeAlso:

********************************************************************************************/

TxtUnderlineAttribute::TxtUnderlineAttribute()
{
	Underlined = FALSE; 
} 


/********************************************************************************************

>	TxtUnderlineAttribute::TxtUnderlineAttribute(BOOL underlined) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		underlined: The underlined state
	Outputs:	-
	Returns:	-
	Purpose:	TxtUnderlineAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtUnderlineAttribute::TxtUnderlineAttribute(BOOL underlined) 
{ 
	Underlined = underlined; 
}



/********************************************************************************************

>	void TxtUnderlineAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtUnderlineAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtUnderlineAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtUnderlineAttribute::Render - do nothing" )
//	TRACE( _T("Warning - TxtUnderlineAttribute::Render called\n") );
	pRegion->SetTxtUnderline(this, Temp);
}

/********************************************************************************************

>	void TxtUnderlineAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtUnderlineAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtUnderlineAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtUnderlineAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtUnderlineAttribute::Restore called\n") );
	pRegion->RestoreTxtUnderline(this, Temp);
}

/********************************************************************************************

>	TxtUnderlineAttribute& TxtUnderlineAttribute::operator=(TxtUnderlineAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtUnderlineAttribute& TxtUnderlineAttribute::operator=(TxtUnderlineAttribute& Attrib)
{	 
	Underlined = Attrib.Underlined;
	return *this;
}

/********************************************************************************************

>	INT32 TxtUnderlineAttribute::operator==(const TxtUnderlineAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/03/95
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtUnderlineAttribute::operator==(const TxtUnderlineAttribute& Attrib)
{
	return (Attrib.Underlined == Underlined); 
}

/********************************************************************************************

>	void TxtUnderlineAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtUnderlineAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtUnderlineAttribute), 
		"Invalid Attribute value passed to TxtUnderlineAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtUnderlineAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtUnderlineAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtUnderlineAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtUnderlineAttribute)), 
		   		"Different attribute types in TxtUnderlineAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtUnderlineAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtUnderlineAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtUnderlineAttribute::Init()
{
	TxtUnderlineAttribute *pAttr = new TxtUnderlineAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtUnderlineAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtUnderlineAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTxtUnderline node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtUnderlineAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtUnderline*  pAttr = new AttrTxtUnderline();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}




 // -----------------------------------------------------------------------------------------
// TxtAspectRatioAttribute methods
 
/********************************************************************************************

>	TxtAspectRatioAttribute::TxtAspectRatioAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtAspectRatioAttribute
				A zero aspect ratio is set

********************************************************************************************/

TxtAspectRatioAttribute::TxtAspectRatioAttribute()
{
	AspectRatio = 1;
} 


/********************************************************************************************

>	TxtAspectRatioAttribute::TxtAspectRatioAttribute(FIXED16 aspectRatio) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		aspectRatio: The aspectRatio value
	Purpose:	TxtAspectRatioAttribute constructor

********************************************************************************************/

TxtAspectRatioAttribute::TxtAspectRatioAttribute(FIXED16 aspect) 
{ 
	AspectRatio = aspect; 
}


/********************************************************************************************

>	void TxtAspectRatioAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtFontTypeface attribute for the given render region. 

********************************************************************************************/

void TxtAspectRatioAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtAspectRatioAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtAspectRatioAttribute::Restore called\n") );
	pRegion->SetTxtAspectRatio(this, Temp);
}


/********************************************************************************************

>	void TxtAspectRatioAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtAspectRatioAttribute attribute for the given render region. 

********************************************************************************************/

void TxtAspectRatioAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtAspectRatioAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtAspectRatioAttribute::Restore called\n") );
	pRegion->RestoreTxtAspectRatio(this, Temp);
}

/********************************************************************************************

>	TxtAspectRatioAttribute& TxtAspectRatioAttribute::operator=(TxtAspectRatioAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtAspectRatioAttribute& TxtAspectRatioAttribute::operator=(TxtAspectRatioAttribute& Attrib)
{	 
	AspectRatio = Attrib.AspectRatio;
	return *this;
}


/********************************************************************************************

>	INT32 TxtAspectRatioAttribute::operator==(const TxtAspectRatioAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtAspectRatioAttribute::operator==(const TxtAspectRatioAttribute& Attrib)
{
	return (Attrib.AspectRatio == AspectRatio); 
}


/********************************************************************************************

>	void TxtAspectRatioAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtAspectRatioAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtAspectRatioAttribute), 
		"Invalid Attribute value passed to TxtAspectRatioAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtAspectRatioAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtAspectRatioAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtAspectRatioAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtAspectRatioAttribute)), 
		   		"Different attribute types in TxtAspectRatioAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtAspectRatioAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtAspectRatioAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtAspectRatioAttribute::Init()
{
	TxtAspectRatioAttribute *pAttr = new TxtAspectRatioAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtAspectRatioAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtAspectRatioAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTextFontTypeface node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtAspectRatioAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtAspectRatio*  pAttr = new AttrTxtAspectRatio();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}





 // -----------------------------------------------------------------------------------------
// TxtJustificationAttribute methods
 
/********************************************************************************************

>	TxtJustificationAttribute::TxtJustificationAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtJustificationAttribute
				It defaults to LEFT justify
			
	SeeAlso:

********************************************************************************************/

TxtJustificationAttribute::TxtJustificationAttribute()
{
	justification = JLEFT;
} 


/********************************************************************************************

>	TxtJustificationAttribute::TxtJustificationAttribute(Justification Justify) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		Justify:The type of justification (JLEFT, JRIGHT, JCENTRE, or JFULL)
	Outputs:	-
	Returns:	-
	Purpose:	TxtJustificationAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtJustificationAttribute::TxtJustificationAttribute(Justification Justify) 
{ 
	justification = Justify; 
}



/********************************************************************************************

>	void TxtJustificationAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtJustificationAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtJustificationAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtJustificationAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtJustificationAttribute::Restore called\n") );
	pRegion->SetTxtJustification(this, Temp);
}

/********************************************************************************************

>	void TxtJustificationAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtJustificationAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtJustificationAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtJustificationAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtJustificationAttribute::Restore called\n") );
	pRegion->RestoreTxtJustification(this, Temp);
}

/********************************************************************************************

>	TxtJustificationAttribute& TxtJustificationAttribute::operator=(TxtJustificationAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtJustificationAttribute& TxtJustificationAttribute::operator=(TxtJustificationAttribute& Attrib)
{	 
	justification = Attrib.justification;
	return *this;
}

/********************************************************************************************

>	INT32 TxtJustificationAttribute::operator==(const TxtJustificationAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtJustificationAttribute::operator==(const TxtJustificationAttribute& Attrib)
{
	return (Attrib.justification == justification); 
}

/********************************************************************************************

>	void TxtJustificationAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtJustificationAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtJustificationAttribute), 
		"Invalid Attribute value passed to TxtJustificationAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtJustificationAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtJustificationAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtJustificationAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtJustificationAttribute)), 
		   		"Different attribute types in TxtJustificationAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtJustificationAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtJustificationAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtJustificationAttribute::Init()
{
	TxtJustificationAttribute *pAttr = new TxtJustificationAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtJustificationAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtJustificationAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTxtJustification node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtJustificationAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtJustification*  pAttr = new AttrTxtJustification();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}





 // -----------------------------------------------------------------------------------------
// TxtTrackingAttribute methods
 
/********************************************************************************************

>	TxtTrackingAttribute::TxtTrackingAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtTrackingAttribute
				The function sets the Tracking value to 0
			
	SeeAlso:

********************************************************************************************/

TxtTrackingAttribute::TxtTrackingAttribute()
{
	Tracking = 0; 
} 


/********************************************************************************************

>	TxtTrackingAttribute::TxtTrackingAttribute(MILLIPOINT Tracking)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		Tracking: The tracking value
	Outputs:	-
	Returns:	-
	Purpose:	TxtTrackingAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtTrackingAttribute::TxtTrackingAttribute(MILLIPOINT tracking) 
{ 
	Tracking = tracking;
}



/********************************************************************************************

>	void TxtTrackingAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtTrackingAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtTrackingAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtTrackingAttribute::Render - do nothing" )
//	TRACE( _T("Warning - TxtTrackingAttribute::Render called\n") );
	pRegion->SetTxtTracking(this, Temp);
}

/********************************************************************************************

>	void TxtTrackingAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtTrackingAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtTrackingAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtTrackingAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtTrackingAttribute::Restore called\n") );
	pRegion->RestoreTxtTracking(this, Temp);
}

/********************************************************************************************

>	TxtTrackingAttribute& TxtTrackingAttribute::operator=(TxtTrackingAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtTrackingAttribute& TxtTrackingAttribute::operator=(TxtTrackingAttribute& Attrib)
{	 
	Tracking = Attrib.Tracking;
	return *this;
}

/********************************************************************************************

>	INT32 TxtTrackingAttribute::operator==(const TxtTrackingAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtTrackingAttribute::operator==(const TxtTrackingAttribute& Attrib)
{
	return (Attrib.Tracking == Tracking); 
}

/********************************************************************************************

>	void TxtTrackingAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtTrackingAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtTrackingAttribute), 
		"Invalid Attribute value passed to TxtTrackingAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtTrackingAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtTrackingAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtTrackingAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtTrackingAttribute)), 
		   		"Different attribute types in TxtTrackingAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtTrackingAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtTrackingAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtTrackingAttribute::Init()
{
	TxtTrackingAttribute *pAttr = new TxtTrackingAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtTrackingAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtTrackingAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTxtTracking node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtTrackingAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtTracking*  pAttr = new AttrTxtTracking();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}



 // -----------------------------------------------------------------------------------------
// TxtScriptAttribute methods
 
/********************************************************************************************

>	TxtScriptAttribute::TxtScriptAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtScriptAttribute
				The function sets the Offset to 0, and the Size to 1
			
	SeeAlso:

********************************************************************************************/

TxtScriptAttribute::TxtScriptAttribute()
{
	Offset = 0;
	Size = 1;

} 


/********************************************************************************************

>	TxtScriptAttribute::TxtScriptAttribute(FIXED16 Offset, FIXED16 Size)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		Tracking: The tracking value
	Outputs:	-
	Returns:	-
	Purpose:	TxtScriptAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtScriptAttribute::TxtScriptAttribute(FIXED16 offset, FIXED16 size) 
{ 
	Offset = offset;
	Size = size;
}



/********************************************************************************************

>	void TxtScriptAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtScriptAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtScriptAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtScriptAttribute::Render - do nothing" )
//	TRACE( _T("Warning - TxtScriptAttribute::Render called\n") );
	pRegion->SetTxtScript(this, Temp);
}

/********************************************************************************************

>	void TxtScriptAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtScriptAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtScriptAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtScriptAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtScriptAttribute::Restore called\n") );
	pRegion->RestoreTxtScript(this, Temp);
}

/********************************************************************************************

>	TxtScriptAttribute& TxtScriptAttribute::operator=(TxtScriptAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtScriptAttribute& TxtScriptAttribute::operator=(TxtScriptAttribute& Attrib)
{	 
	Offset = Attrib.Offset;
	Size = Attrib.Size;
	return *this;
}

/********************************************************************************************

>	INT32 TxtScriptAttribute::operator==(const TxtScriptAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtScriptAttribute::operator==(const TxtScriptAttribute& Attrib)
{
	return ((Attrib.Offset == Offset) && (Attrib.Size == Size)); 
}

/********************************************************************************************

>	void TxtScriptAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtScriptAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtScriptAttribute), 
		"Invalid Attribute value passed to TxtScriptAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtScriptAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtScriptAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtScriptAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtScriptAttribute)), 
		   		"Different attribute types in TxtScriptAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtScriptAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtScriptAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtScriptAttribute::Init()
{
	TxtScriptAttribute *pAttr = new TxtScriptAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtScriptAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtScriptAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTxtTracking node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtScriptAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtScript*  pAttr = new AttrTxtScript();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}


// -----------------------------------------------------------------------------------------
// TxtBaseLineAttribute methods
 
/********************************************************************************************

>	TxtBaseLineAttribute::TxtBaseLineAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtBaseLineAttribute
				The function sets the Tracking value to 0
			
	SeeAlso:

********************************************************************************************/

TxtBaseLineAttribute::TxtBaseLineAttribute()
{
	Value = 0; 
} 


/********************************************************************************************

>	TxtBaseLineAttribute::TxtBaseLineAttribute(MILLIPOINT Value)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		Value: The absolute offset from base line
	Outputs:	-
	Returns:	-
	Purpose:	TxtBaseLineAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtBaseLineAttribute::TxtBaseLineAttribute(MILLIPOINT value) 
{ 
	Value = value;
}



/********************************************************************************************

>	void TxtBaseLineAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtBaseLineAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtBaseLineAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtBaseLineAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtBaseLineAttribute::Restore called\n") );
	pRegion->SetTxtBaseLine(this, Temp);
}

/********************************************************************************************

>	void TxtBaseLineAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtBaseLineAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtBaseLineAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtBaseLineAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtBaseLineAttribute::Restore called\n") );
	pRegion->RestoreTxtBaseLine(this, Temp);
}

/********************************************************************************************

>	TxtBaseLineAttribute& TxtBaseLineAttribute::operator=(TxtBaseLineAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtBaseLineAttribute& TxtBaseLineAttribute::operator=(TxtBaseLineAttribute& Attrib)
{	 
	Value = Attrib.Value;
	return *this;
}

/********************************************************************************************

>	INT32 TxtBaseLineAttribute::operator==(const TxtBaseLineAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtBaseLineAttribute::operator==(const TxtBaseLineAttribute& Attrib)
{
	return (Attrib.Value == Value); 
}

/********************************************************************************************

>	void TxtBaseLineAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtBaseLineAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtBaseLineAttribute), 
		"Invalid Attribute value passed to TxtBaseLineAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtBaseLineAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtBaseLineAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtBaseLineAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtBaseLineAttribute)), 
		   		"Different attribute types in TxtBaseLineAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtBaseLineAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtBaseLineAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtBaseLineAttribute::Init()
{
	TxtBaseLineAttribute *pAttr = new TxtBaseLineAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtBaseLineAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtBaseLineAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTxtTracking node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtBaseLineAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtBaseLine*  pAttr = new AttrTxtBaseLine();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}




 // -----------------------------------------------------------------------------------------
// TxtLineSpaceAttribute methods
 
/********************************************************************************************

>	TxtLineSpaceAttribute::TxtLineSpaceAttribute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default Constuctor for TxtLineSpaceAttribute
				The function sets the Value to 19.2pt - 120% of 16pt
			
	SeeAlso:

********************************************************************************************/

TxtLineSpaceAttribute::TxtLineSpaceAttribute()
{
	IsRatio = TRUE;
	Value = 0;
	Ratio = FIXED16(1.0);
} 


/********************************************************************************************

>	TxtLineSpaceAttribute::TxtLineSpaceAttribute(MILLIPOINT Value)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		Value: The tracking value
	Outputs:	-
	Returns:	-
	Purpose:	TxtLineSpaceAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtLineSpaceAttribute::TxtLineSpaceAttribute(MILLIPOINT value) 
{ 
	IsRatio = FALSE;
	Value = value;
	Ratio = FIXED16(0);
}

/********************************************************************************************

>	TxtLineSpaceAttribute::TxtLineSpaceAttribute(FIXED16 Ratio)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		Ratio: The line space ratio
	Outputs:	-
	Returns:	-
	Purpose:	TxtLineSpaceAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TxtLineSpaceAttribute::TxtLineSpaceAttribute(FIXED16 ratio) 
{ 
	IsRatio = TRUE;
	Ratio = ratio;
	Value = 0;
}

/********************************************************************************************

>	TxtLineSpaceAttribute::IsARatio()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95
	Inputs:		Ratio: The line space ratio
	Outputs:	-
	Returns:	TRUE if the line space is a ratio value
	Purpose:	TxtLineSpaceAttribute constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL TxtLineSpaceAttribute::IsARatio()const 
{ 
	return (IsRatio);
}



/********************************************************************************************

>	void TxtLineSpaceAttribute::Render(RenderRegion *pRegion)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to render this attribute into.
	Purpose:	Sets the TxtLineSpaceAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtLineSpaceAttribute::Render(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtLineSpaceAttribute::Render - do nothing" )
//	TRACE( _T("Warning - TxtLineSpaceAttribute::Render called\n") );
	pRegion->SetTxtLineSpace(this, Temp);
}

/********************************************************************************************

>	void TxtLineSpaceAttribute::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).
	Purpose:	Restores the TxtLineSpaceAttribute attribute for the given render region. 
	SeeAlso:	-

********************************************************************************************/

void TxtLineSpaceAttribute::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "TxtLineSpaceAttribute::Restore - do nothing" )
//	TRACE( _T("Warning - TxtLineSpaceAttribute::Restore called\n") );
	pRegion->RestoreTxtLineSpace(this, Temp);
}

/********************************************************************************************

>	TxtLineSpaceAttribute& TxtLineSpaceAttribute::operator=(TxtLineSpaceAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Attrib - the attribute to copy

	Returns:	Usual semantics for equality.
	Purpose:	Make the Attribute the same as the other. 

********************************************************************************************/

TxtLineSpaceAttribute& TxtLineSpaceAttribute::operator=(TxtLineSpaceAttribute& Attrib)
{	
	Value = Attrib.Value;
	Ratio = Attrib.Ratio;
	IsRatio = Attrib.IsARatio();
	return *this;
}

/********************************************************************************************

>	INT32 TxtLineSpaceAttribute::operator==(const TxtLineSpaceAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare this attribute with
	Returns:	Usual semantics for equality.
	Purpose:	Comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 TxtLineSpaceAttribute::operator==(const TxtLineSpaceAttribute& Attrib)
{
	if (Attrib.IsARatio() == IsARatio())
	{
		if (IsRatio)
		{
			return (Attrib.Ratio == Ratio);
		}
		else
		{
			return (Attrib.Value == Value);
		}
	}
	else return FALSE;  
}

/********************************************************************************************

>	void TxtLineSpaceAttribute ::SimpleCopy(AttributeValue *pValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		pAttr - pointer to the AttributeValue to copy.
	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void TxtLineSpaceAttribute::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, TxtLineSpaceAttribute), 
		"Invalid Attribute value passed to TxtLineSpaceAttribute::SimpleCopy");
	// We may as well just use our assignment operator.
	*this = *((TxtLineSpaceAttribute*)pValue);
}


/********************************************************************************************

>	BOOL TxtLineSpaceAttribute::IsDifferent(AttributeValue *pAttr)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	See base class version.
	Errors:		The two attributes are not of the same type.
	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL TxtLineSpaceAttribute::IsDifferent(AttributeValue *pAttr)
{
	// This must be at least a FillGeometryAttribute...
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(TxtLineSpaceAttribute)), 
		   		"Different attribute types in TxtLineSpaceAttribute::IsDifferent()");

	// Check they are NOT the same.
	return ( !(*((TxtLineSpaceAttribute *)pAttr) == *this) ); 
}

/********************************************************************************************

>	BOOL TxtLineSpaceAttribute::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	TRUE - initialised ok; FALSE if not.
	Purpose:	Registers default attribute
	Errors:		Out of memory.
	SeeAlso:	AttributeManager

********************************************************************************************/

BOOL TxtLineSpaceAttribute::Init()
{
	TxtLineSpaceAttribute *pAttr = new TxtLineSpaceAttribute;
	if (pAttr==NULL)
		// error message has already been set by new
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(BaseTextClass), 
														 pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising TxtLineSpaceAttribute");

	return TRUE;
}

/********************************************************************************************

>	NodeAttribute *TxtLineSpaceAttribute::MakeNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Returns:	Pointer to the new node, or NULL if out of memory.
	Purpose:	Make a AttrTxtTracking node, see base class
	Errors:		Out of memory
	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *TxtLineSpaceAttribute::MakeNode()
{
	// Create new attribute node
	AttrTxtLineSpace*  pAttr = new AttrTxtLineSpace();
	if (pAttr==NULL)	  
		// error message has already been set by new
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	// Return the new node
	return pAttr;
}



// -----------------------------------------------------------------------------------------
// Node Attributes
// -----------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------
// AttrTxtFontTypeface methods

/********************************************************************************************

>	AttrTxtFontTypeface::AttrTxtFontTypeface(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtFontTypeface Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtFontTypeface::AttrTxtFontTypeface(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtFontTypeface::AttrTxtFontTypeface()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtFontTypeface class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtFontTypeface::AttrTxtFontTypeface()
{
}


/********************************************************************************************

>	void AttrTxtFontTypeface::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtFontTypeface::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtFontTypeface::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtFontTypeface::Render called\n") );
	pRegion->SetTxtFontTypeface(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtFontTypeface::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/

Node* AttrTxtFontTypeface::SimpleCopy()
{
	AttrTxtFontTypeface* NodeCopy = new AttrTxtFontTypeface();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
}

/********************************************************************************************

>	INT32 AttrTxtFontTypeface::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrTxtFontTypeface 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtFontTypeface 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtFontTypeface::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtFontTypeface)), 
		"Trying to compare two objects with different types"); 
	AttrTxtFontTypeface* Attr = (AttrTxtFontTypeface*) &Attrib;
	return (Attr->Value.HTypeface == Value.HTypeface &&
			Attr->Value.IsBold == Value.IsBold &&
			Attr->Value.IsItalic == Value.IsItalic  ); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtFontTypeface::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtFontTypeface::GetAttrNameID(void)  
{
	return (_R(IDS_FONTTYPEFACE)); 
}  


/***********************************************************************************************
> ` void AttrTxtFontTypeface::CopyNodeContents( AttrTxtFontTypeface* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtFontTypeface::CopyNodeContents( AttrTxtFontTypeface* NodeCopy)
{
	// Let the base class do its bit
	AttrTxtBase::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.HTypeface = Value.HTypeface;
	NodeCopy->Value.IsBold = Value.IsBold;
	NodeCopy->Value.IsItalic = Value.IsItalic;
} 


/***********************************************************************************************
>   void AttrTxtFontTypeface::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtFontTypeface::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtFontTypeface), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtFontTypeface))
		CopyNodeContents((AttrTxtFontTypeface*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtFontTypeface::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtFontTypeface::GetNodeSize() const 
{     
	return (sizeof(AttrTxtFontTypeface)); 
}


/********************************************************************************************

>	virtual void AttrTxtFontTypeface::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtFontTypeface::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );
	String_64 FontName;
	if (FONTMANAGER->GetFontName(Value.HTypeface, FontName))
	{
		FontClass Class = FONTMANAGER->GetFontClass(Value.HTypeface);

		TCHAR			buf[255];
		
		tsprintf( buf, 255, _T("\r\nFontName = %s\r\n"), (TCHAR*)(FontName) );
		(*Str) += buf;

		tsprintf( buf, 256, _T("\r\nFont Handle = %ld\r\n"), Value.HTypeface );
		(*Str) += buf;
		
		switch (Class)
		{
		case FC_ATM:
			tsprintf( buf, 255, _T("Type = ATM Type1\r\n") ); 
			break;
		case FC_TRUETYPE: 
			tsprintf( buf, 255, _T("Type = TrueType\r\n") );
			break;
		default:
			tsprintf( buf, 255, _T("Type = Unknown\r\n") ); 
			break;
		}
		
		(*Str) += buf;
		tsprintf( buf, 255, _T("Bold = %s\r\n"), Value.IsBold ? "TRUE":"FALSE");
		(*Str) += buf;
		tsprintf( buf, 255, _T("Italic = %s\r\n"), Value.IsItalic ? "TRUE":"FALSE");
		(*Str) += buf;	
		tsprintf( buf, 255, _T("Replaced = %s\r\n"), FONTMANAGER->IsFontReplaced(&FontName, Class) ? "TRUE":"FALSE");
		(*Str) += buf;	
	}
}

/********************************************************************************************

>	BOOL AttrTxtFontTypeface::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtFontTypeface::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text font typeface attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtFontTypeface::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");

	return CXaraFileTxtFontTypeface::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtFontTypeface::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.");

	return CXaraFileTxtFontTypeface::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// -----------------------------------------------------------------------------------------
// AttrTxtBold methods																		 

/********************************************************************************************

>	AttrTxtBold::AttrTxtBold(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtBold Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtBold::AttrTxtBold(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtBold::AttrTxtBold()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtBold class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtBold::AttrTxtBold()
{
}


/********************************************************************************************

>	void AttrTxtBold::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtBold::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtBold::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtBold::Render called\n") );
	pRegion->SetTxtBold(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtBold::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtBold::SimpleCopy()
{
	AttrTxtBold* NodeCopy = new AttrTxtBold();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtBold::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrTxtBold 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtBold 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtBold::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtBold)), 
		"Trying to compare two objects with different types"); 
	AttrTxtBold* Attr = (AttrTxtBold*) &Attrib;
	return (Attr->Value.BoldOn == Value.BoldOn); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtBold::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtBold::GetAttrNameID(void)  
{
	return (_R(IDS_BOLD)); 
}  


/***********************************************************************************************
> ` void AttrTxtBold::CopyNodeContents( AttrTxtBold* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtBold::CopyNodeContents( AttrTxtBold* NodeCopy)
{
	// Let the base class do its bit
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.BoldOn = Value.BoldOn;
} 
            
                 
/***********************************************************************************************
>   void AttrTxtBold::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtBold::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtBold), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtBold))
		CopyNodeContents((AttrTxtBold*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtBold::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtBold::GetNodeSize() const 
{     
	return (sizeof(AttrTxtBold)); 
}


/********************************************************************************************

>	virtual void AttrTxtBold::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtBold::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	char* p;

	(Value.BoldOn) ? (p = "TRUE") : (p = "FALSE");
	TempStr._MakeMsg( TEXT("\r\nBold=#1%s\r\n"), p);
	(*Str) += TempStr;
}

/********************************************************************************************

>	BOOL AttrTxtBold::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtBold::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text bold attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtBold::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtBold::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtBold::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtBold::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// -----------------------------------------------------------------------------------------
// AttrTxtItalic methods

/********************************************************************************************

>	AttrTxtItalic::AttrTxtItalic(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtItalic Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtItalic::AttrTxtItalic(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtItalic::AttrTxtItalic()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtItalic class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtItalic::AttrTxtItalic()
{
}


/********************************************************************************************

>	void AttrTxtItalic::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtItalic::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtItalic::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtItalic::Render called\n") );
	pRegion->SetTxtItalic(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtItalic::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtItalic::SimpleCopy()
{
	AttrTxtItalic* NodeCopy = new AttrTxtItalic();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtItalic::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrLineWidth 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtItalic 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtItalic::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtItalic)), 
		"Trying to compare two objects with different types"); 
	AttrTxtItalic* Attr = (AttrTxtItalic*) &Attrib;
	return (Attr->Value.ItalicOn == Value.ItalicOn); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtItalic::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtItalic::GetAttrNameID(void)  
{
	return (_R(IDS_ITALIC)); 
}  


/***********************************************************************************************
> ` void AttrTxtItalic::CopyNodeContents( AttrTxtItalic* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtItalic::CopyNodeContents( AttrTxtItalic* NodeCopy)
{
	// Let the base class do its bit
	AttrTxtBase::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.ItalicOn = Value.ItalicOn;
} 
            
                 
/***********************************************************************************************
>   void AttrTxtItalic::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtItalic::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtItalic), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtItalic))
		CopyNodeContents((AttrTxtItalic*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtItalic::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtItalic::GetNodeSize() const 
{     
	return (sizeof(AttrTxtItalic)); 
}


/********************************************************************************************

>	virtual void AttrTxtItalic::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtItalic::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	char* p;

	(Value.ItalicOn) ? (p = "TRUE") : (p = "FALSE");
	TempStr._MakeMsg( TEXT("\r\nItalic=#1%s\r\n"), p);
	(*Str) += TempStr;
}

/********************************************************************************************

>	BOOL AttrTxtItalic::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtItalic::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text font italic attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtItalic::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtItalic::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtItalic::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtItalic::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// -----------------------------------------------------------------------------------------
// AttrTxtFontSize methods


/********************************************************************************************

>	AttrTxtFontSize::AttrTxtFontSize(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtFontSize Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtFontSize::AttrTxtFontSize(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtFontSize::AttrTxtFontSize()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtFontSize class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtFontSize::AttrTxtFontSize()
{
}


/********************************************************************************************

>	void AttrTxtFontSize::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtFontSize::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtFontSize::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtFontSize::Render called\n") );
	pRegion->SetTxtFontSize(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtFontSize::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtFontSize::SimpleCopy()
{
	AttrTxtFontSize* NodeCopy = new AttrTxtFontSize();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtFontSize::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrTxtFontSize 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtFontSize 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtFontSize::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtFontSize)), 
		"Trying to compare two objects with different types"); 
	AttrTxtFontSize* Attr = (AttrTxtFontSize*) &Attrib;
	return (Attr->Value.FontSize == Value.FontSize); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtFontSize::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtFontSize::GetAttrNameID(void)  
{
	return (_R(IDS_FONTSIZE)); 
}  


/***********************************************************************************************
> ` void AttrTxtFontSize::CopyNodeContents( AttrTxtFontSize* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtFontSize::CopyNodeContents( AttrTxtFontSize* NodeCopy)
{
	// Let the base class do its bit
	AttrTxtBase::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.FontSize = Value.FontSize;
} 
            
                 
/***********************************************************************************************
>   void AttrTxtFontSize::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtFontSize::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtFontSize), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtFontSize))
		CopyNodeContents((AttrTxtFontSize*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtFontSize::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtFontSize::GetNodeSize() const 
{     
	return (sizeof(AttrTxtFontSize)); 
}


/********************************************************************************************

>	virtual void AttrTxtFontSize::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtFontSize::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	TempStr._MakeMsg( TEXT("\r\nFontSize=#1%ld\r\n"), Value.FontSize);
	(*Str) += TempStr;
}


/********************************************************************************************
>	void AttrTxtFontSize::BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/95
	Inputs:		Scale  - 
				Aspect -
	Purpose:	Transform the attribute using the baseline relative scale and aspect
********************************************************************************************/

void AttrTxtFontSize::BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect)
{
	TxtFontSizeAttribute* pFontSizeAttrVal=(TxtFontSizeAttribute*)GetAttributeValue();
	pFontSizeAttrVal->FontSize = XLONG(pFontSizeAttrVal->FontSize) * Scale;
}

/********************************************************************************************

>	BOOL AttrTxtFontSize::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtFontSize::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text font size attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtFontSize::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtFontSize::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtFontSize::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtFontSize::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// AttrTxtUnderline methods
// -----------------------------------------------------------------------------------------

/********************************************************************************************

>	AttrTxtUnderline::AttrTxtUnderline(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtUnderline Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtUnderline::AttrTxtUnderline(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtUnderline::AttrTxtUnderline()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtUnderline class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtUnderline::AttrTxtUnderline()
{
}


/********************************************************************************************

>	void AttrTxtUnderline::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtUnderline::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtUnderline::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtUnderline::Render called\n") );
	pRegion->SetTxtUnderline(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtUnderline::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtUnderline::SimpleCopy()
{
	AttrTxtUnderline* NodeCopy = new AttrTxtUnderline();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtUnderline::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrLineWidth 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtUnderline 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtUnderline::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtUnderline)), 
		"Trying to compare two objects with different types"); 
	AttrTxtUnderline* Attr = (AttrTxtUnderline*) &Attrib;
	return (Attr->Value.Underlined == Value.Underlined); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtUnderline::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtUnderline::GetAttrNameID(void)  
{
	return (_R(IDS_UNDERLINE)); 
}  


/***********************************************************************************************
> ` void AttrTxtUnderline::CopyNodeContents( AttrTxtUnderline* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtUnderline::CopyNodeContents( AttrTxtUnderline* NodeCopy)
{
	// Let the base class do its bit
	AttrTxtBase::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.Underlined = Value.Underlined;
} 
            
                 
/***********************************************************************************************
>   void AttrTxtUnderline::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtUnderline::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtUnderline), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtUnderline))
		CopyNodeContents((AttrTxtUnderline*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtUnderline::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtUnderline::GetNodeSize() const 
{     
	return (sizeof(AttrTxtUnderline)); 
}

/********************************************************************************************

>	virtual void AttrTxtUnderline::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtUnderline::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	char* p;
	(Value.Underlined) ? (p="TRUE") : (p="FALSE");
	TempStr._MakeMsg( TEXT("\r\nUnderline=#1%s\r\n"), p);
	(*Str) += TempStr;
}

/********************************************************************************************

>	BOOL AttrTxtUnderline::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtUnderline::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text underline attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtUnderline::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtUnderline::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtUnderline::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtUnderline::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// ------------------------------------------------------------------------------------------
// AttrTxtAspectRatio methods

/********************************************************************************************

>	AttrTxtAspectRatio::AttrTxtAspectRatio(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Creates a AttrTxtAspectRatio Attribute

********************************************************************************************/

AttrTxtAspectRatio::AttrTxtAspectRatio(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtAspectRatio::AttrTxtAspectRatio()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Default constructor for AttrTxtAspectRatio class

********************************************************************************************/

AttrTxtAspectRatio::AttrTxtAspectRatio()
{
}


/********************************************************************************************

>	void AttrTxtAspectRatio::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	'Renders' a Line Width attribute.

********************************************************************************************/

void AttrTxtAspectRatio::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtAspectRatio::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtAspectRatio::Render called\n") );
	pRegion->SetTxtAspectRatio(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtAspectRatio::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtAspectRatio::SimpleCopy()
{
	AttrTxtAspectRatio* NodeCopy = new AttrTxtAspectRatio();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtAspectRatio::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrLineWidth 
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtAspectRatio 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtAspectRatio::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtAspectRatio)), 
		"Trying to compare two objects with different types"); 
	AttrTxtAspectRatio* Attr = (AttrTxtAspectRatio*) &Attrib;
	return (Attr->Value.AspectRatio == Value.AspectRatio); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtAspectRatio::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrTxtAspectRatio::GetAttrNameID(void)  
{
	return (_R(IDS_ASPECTRATIO)); 
}  


/***********************************************************************************************
> ` void AttrTxtAspectRatio::CopyNodeContents( AttrTxtAspectRatio* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtAspectRatio::CopyNodeContents( AttrTxtAspectRatio* NodeCopy)
{
	// Let the base class do its bit
	AttrTxtBase::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.AspectRatio = Value.AspectRatio;
} 
            
                 
/***********************************************************************************************
>   void AttrTxtAspectRatio::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtAspectRatio::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtAspectRatio), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtAspectRatio))
		CopyNodeContents((AttrTxtAspectRatio*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtAspectRatio::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtAspectRatio::GetNodeSize() const 
{     
	return (sizeof(AttrTxtAspectRatio)); 
}


/********************************************************************************************

>	virtual void AttrTxtAspectRatio::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtAspectRatio::GetDebugDetails(StringBase* Str)
{
#if DEBUG_TREE
	NodeAttribute::GetDebugDetails( Str );

	String_256			TempStr;
	TCHAR				floatStr[20];
	tsprintf( floatStr, 20, _T("%f"), Value.AspectRatio.MakeDouble() );
	TempStr._MakeMsg( TEXT("\r\nAspect ratio :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;
#endif
}


/********************************************************************************************
>	void AttrTxtAspectRatio::BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/95
	Inputs:		Scale  - 
				Aspect -
	Purpose:	Transform the attribute using the baseline relative scale and aspect
********************************************************************************************/

void AttrTxtAspectRatio::BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect)
{
	TxtAspectRatioAttribute* pAspectRatioAttrVal=(TxtAspectRatioAttribute*)GetAttributeValue();
	pAspectRatioAttrVal->AspectRatio = pAspectRatioAttrVal->AspectRatio * Aspect;
}

/********************************************************************************************

>	BOOL AttrTxtAspectRatio::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtAspectRatio::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text aspect ratio attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtAspectRatio::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtAspectRatio::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtAspectRatio::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtAspectRatio::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// AttrTxtJustification methods
// -----------------------------------------------------------------------------------------

/********************************************************************************************

>	AttrTxtJustification::AttrTxtJustification(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtJustification Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtJustification::AttrTxtJustification(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtJustification::AttrTxtJustification()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtJustification class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtJustification::AttrTxtJustification()
{
}


/********************************************************************************************

>	void AttrTxtJustification::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtJustification::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtJustification::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtJustification::Render called\n") );
	pRegion->SetTxtJustification(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtJustification::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtJustification::SimpleCopy()
{
	AttrTxtJustification* NodeCopy = new AttrTxtJustification();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtJustification::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrLineWidth 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtJustification 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtJustification::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtJustification)), 
		"Trying to compare two objects with different types"); 
	AttrTxtJustification* Attr = (AttrTxtJustification*) &Attrib;
	return (Attr->Value.justification == Value.justification); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtJustification::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtJustification::GetAttrNameID(void)  
{
	return (_R(IDS_JUSTIFICATION)); 
}  


/***********************************************************************************************
> ` void AttrTxtJustification::CopyNodeContents( AttrTxtJustification* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtJustification::CopyNodeContents( AttrTxtJustification* NodeCopy)
{
	// Let the base class do its bit
	AttrTxtBase::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.justification = Value.justification;
} 
            
                 
/***********************************************************************************************
>   void AttrTxtJustification::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtJustification::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtJustification), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtJustification))
		CopyNodeContents((AttrTxtJustification*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtJustification::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtJustification::GetNodeSize() const 
{     
	return (sizeof(AttrTxtJustification)); 
}


/********************************************************************************************

>	virtual void AttrTxtJustification::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtJustification::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	char* p;

	switch (Value.justification)
	{
		case JLEFT:
			p="Left";
			break;
		case JRIGHT:
			p="Right";
			break;
		case JCENTRE:
			p="Centre";
			break;
		case JFULL:
			p="Fully";
			break;
		default:
			p="(Unknown)";
			break;
	}

	TempStr._MakeMsg(TEXT("\r\nJustification=#1%s"), p);
	(*Str) += TempStr;
	TempStr._MakeMsg(TEXT(" justified\r\n"));
	(*Str) += TempStr;
}

/********************************************************************************************

>	BOOL AttrTxtJustification::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtJustification::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text justification attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtJustification::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtJustification::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtJustification::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtJustification::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// AttrTxtTracking methods
// -----------------------------------------------------------------------------------------

/********************************************************************************************

>	AttrTxtTracking::AttrTxtTracking(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtTracking Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtTracking::AttrTxtTracking(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtTracking::AttrTxtTracking()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtTracking class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtTracking::AttrTxtTracking()
{
}


/********************************************************************************************

>	void AttrTxtTracking::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtTracking::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtTracking::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtTracking::Render called\n") );
	pRegion->SetTxtTracking(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtTracking::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtTracking::SimpleCopy()
{
	AttrTxtTracking* NodeCopy = new AttrTxtTracking();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtTracking::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrLineWidth 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtTracking 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtTracking::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtTracking)), 
		"Trying to compare two objects with different types"); 
	AttrTxtTracking* Attr = (AttrTxtTracking*) &Attrib;
	return (Attr->Value.Tracking == Value.Tracking); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtTracking::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtTracking::GetAttrNameID(void)  
{
	return (_R(IDS_TRACKING)); 
}  


/***********************************************************************************************
> ` void AttrTxtTracking::CopyNodeContents( AttrTxtTracking* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtTracking::CopyNodeContents( AttrTxtTracking* NodeCopy)
{
	// Let the base class do its bit
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.Tracking = Value.Tracking;
} 
            
                 
/***********************************************************************************************
>   void AttrTxtTracking::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtTracking::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtTracking), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtTracking))
		CopyNodeContents((AttrTxtTracking*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtTracking::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtTracking::GetNodeSize() const 
{     
	return (sizeof(AttrTxtTracking)); 
}


/********************************************************************************************

>	virtual void AttrTxtTracking::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtTracking::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	TempStr._MakeMsg( TEXT("\r\nTracking=#1%ld\r\n"), Value.Tracking);
	(*Str) += TempStr;
}

/********************************************************************************************

>	BOOL AttrTxtTracking::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtTracking::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text tracking attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtTracking::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtTracking::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtTracking::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtTracking::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// AttrTxtScript methods
// -----------------------------------------------------------------------------------------


/********************************************************************************************

>	AttrTxtScript::AttrTxtScript(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtScript Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtScript::AttrTxtScript(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtScript::AttrTxtScript()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtScript class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtScript::AttrTxtScript()
{
}


/********************************************************************************************

>	void AttrTxtScript::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtScript::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtScript::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtScript::Render called\n") );
	pRegion->SetTxtScript(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtScript::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtScript::SimpleCopy()
{
	AttrTxtScript* NodeCopy = new AttrTxtScript();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtScript::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrTxtScript
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtScript 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtScript::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtScript)), 
		"Trying to compare two objects with different types"); 
	AttrTxtScript* Attr = (AttrTxtScript*) &Attrib;
	return ((Attr->Value.Size == Value.Size) && (Attr->Value.Offset == Value.Offset)); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtScript::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtScript::GetAttrNameID(void)  
{
	return (_R(IDS_SCRIPT)); 
}  


/***********************************************************************************************
> ` void AttrTxtScript::CopyNodeContents( AttrTxtScript* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtScript::CopyNodeContents( AttrTxtScript* NodeCopy)
{
	// Let the base class do its bit
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.Size = Value.Size;
	NodeCopy->Value.Offset = Value.Offset;
} 
            
                 
/***********************************************************************************************
>   void AttrTxtScript::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtScript::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtScript), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtScript))
		CopyNodeContents((AttrTxtScript*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtScript::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtScript::GetNodeSize() const 
{     
	return (sizeof(AttrTxtScript)); 
}


/********************************************************************************************

>	virtual void AttrTxtScript::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtScript::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );

	String_256			TempStr;
	TempStr._MakeMsg( TEXT("\r\nSize=#1%ld\r\n"), Value.Size.GetRawLong() );
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("Offset=#1%ld\r\n"), Value.Offset.GetRawLong() );
	(*Str) += TempStr;
}

/********************************************************************************************

>	BOOL AttrTxtScript::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtScript::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text script attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtScript::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtScript::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtScript::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtScript::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// AttrTxtBaseLine methods
// -----------------------------------------------------------------------------------------

/********************************************************************************************

>	AttrTxtBaseLine::AttrTxtBaseLine(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtBaseLine Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtBaseLine::AttrTxtBaseLine(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtBaseLine::AttrTxtBaseLine()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtBaseLine class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtBaseLine::AttrTxtBaseLine()
{
}


/********************************************************************************************

>	void AttrTxtBaseLine::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtBaseLine::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtBaseLine::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtBaseLine::Render called\n") );
	pRegion->SetTxtBaseLine(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtBaseLine::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtBaseLine::SimpleCopy()
{
	AttrTxtBaseLine* NodeCopy = new AttrTxtBaseLine();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtBaseLine::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrTxtBaseLine 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtBaseLine 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtBaseLine::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtBaseLine)), 
		"Trying to compare two objects with different types"); 
	AttrTxtBaseLine* Attr = (AttrTxtBaseLine*) &Attrib;
	return (Attr->Value.Value == Value.Value); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtBaseLine::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtBaseLine::GetAttrNameID(void)  
{
	return (_R(IDS_BASELINE)); 
}  


/***********************************************************************************************
> ` void AttrTxtBaseLine::CopyNodeContents( AttrTxtBaseLine* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtBaseLine::CopyNodeContents( AttrTxtBaseLine* NodeCopy)
{
	// Let the base class do its bit
	NodeAttribute::CopyNodeContents( NodeCopy );
	
	//Copy contents specific to derived class here
	NodeCopy->Value.Value = Value.Value;
} 
            
                 
/***********************************************************************************************
>   void AttrTxtBaseLine::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtBaseLine::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtBaseLine), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtBaseLine))
		CopyNodeContents((AttrTxtBaseLine*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtBaseLine::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtBaseLine::GetNodeSize() const 
{     
	return (sizeof(AttrTxtBaseLine)); 
}


/********************************************************************************************

>	virtual void AttrTxtBaseLine::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtBaseLine::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;
	TempStr._MakeMsg( TEXT("\r\nRise=#1%ld\r\n"), Value.Value);
	(*Str) += TempStr;
}


/********************************************************************************************
>	void AttrTxtBaseLine::BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/95
	Inputs:		Scale  - 
				Aspect -
	Purpose:	Transform the attribute using the baseline relative scale and aspect
********************************************************************************************/

void AttrTxtBaseLine::BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect)
{
	TxtBaseLineAttribute* pBaseLineAttrVal=(TxtBaseLineAttribute*)GetAttributeValue();
	pBaseLineAttrVal->Value = XLONG(pBaseLineAttrVal->Value) * Scale;
}


/********************************************************************************************

>	BOOL AttrTxtBaseLine::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtBaseLine::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text baseline attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtBaseLine::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtBaseLine::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtBaseLine::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtBaseLine::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

// AttrTxtLineSpace methods
// -----------------------------------------------------------------------------------------

/********************************************************************************************

>	AttrTxtLineSpace::AttrTxtLineSpace(Node* ContextNode,  
											 AttachNodeDirection Direction,  
											 BOOL Locked, 
											 BOOL Mangled,  
											 BOOL Marked, 
											 BOOL Selected    
										): AttrTxtBase(ContextNode, Direction, Locked, 
														 Mangled, Marked, Selected, TRUE)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates a AttrTxtLineSpace Attribute
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtLineSpace::AttrTxtLineSpace(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
  					BOOL Marked, 
					BOOL Selected    
			): AttrTxtBase(ContextNode, Direction, Locked, Mangled, Marked, Selected)  
{                         
} 

/********************************************************************************************

>	AttrTxtLineSpace::AttrTxtLineSpace()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtLineSpace class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtLineSpace::AttrTxtLineSpace()
{
}


/********************************************************************************************

>	AttrTxtLineSpace::AttrTxtLineSpace(MILLIPOINT Value)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtLineSpace class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtLineSpace::AttrTxtLineSpace(MILLIPOINT value)
{
	Value.Value = value;
	Value.Ratio	= 0;
	Value.IsRatio = FALSE;
}

 
/********************************************************************************************

>	AttrTxtLineSpace::AttrTxtLineSpace(FIXED16 Ratio)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor for AttrTxtLineSpace class
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

AttrTxtLineSpace::AttrTxtLineSpace(FIXED16 ratio)
{
	Value.Ratio = ratio;
	Value.Value = 0;
  	Value.IsRatio = TRUE;

}


/********************************************************************************************

>	void AttrTxtLineSpace::Render( RenderRegion* pRender)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	'Renders' a Line Width attribute.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void AttrTxtLineSpace::Render( RenderRegion* pRegion)
{
//#pragma message( __LOCMSG__ "AttrTxtLineSpace::Render - do nothing" )
//	TRACE( _T("Warning - AttrTxtLineSpace::Render called\n") );
	pRegion->SetTxtLineSpace(&Value, FALSE);
}



/***********************************************************************************************
> Node* AttrTxtLineSpace::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    13/03/95
	
	Inputs:         - 
    Outputs:     
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
   	
	Scope:      protected       
***********************************************************************************************/
     
Node* AttrTxtLineSpace::SimpleCopy()
{
	AttrTxtLineSpace* NodeCopy = new AttrTxtLineSpace();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);
	return NodeCopy;
} 

/********************************************************************************************

>	INT32 AttrTxtLineSpace::operator==(const NodeAttribute& Attrib)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Inputs:		Atrib:	The attribute to compare, which must be an AttrLineWidth 
	Outputs:	-
	Returns:	-
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
 
	Errors:		An ENSURE failure will occur if Attrib does not have a AttrTxtLineSpace 
				runtime class.
				 
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTxtLineSpace::operator==(const NodeAttribute& Attrib)
{
	ENSURE(Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTxtLineSpace)), 
		"Trying to compare two objects with different types"); 
	AttrTxtLineSpace* Attr = (AttrTxtLineSpace*) &Attrib;
	return (Attr->Value == Value); 
} 


/********************************************************************************************

>	virtual UINT32 AttrTxtLineSpace::GetAttrNameID(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		-
	Outputs:	-
	Returns:	Attribute description ID
	Purpose:	Returns back a string resource ID describing the attribute

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 AttrTxtLineSpace::GetAttrNameID(void)  
{
	return (_R(IDS_LINESPACE)); 
}  


/***********************************************************************************************
> ` void AttrTxtLineSpace::CopyNodeContents( AttrTxtLineSpace* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtLineSpace::CopyNodeContents( AttrTxtLineSpace* NodeCopy)
{
	// Let the base class do its bit
	NodeAttribute::CopyNodeContents( NodeCopy );
	NodeCopy->Value = Value;

} 
            
                 
/***********************************************************************************************
>   void AttrTxtLineSpace::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTxtLineSpace::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTxtLineSpace), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTxtLineSpace))
		CopyNodeContents((AttrTxtLineSpace*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTxtLineSpace::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTxtLineSpace::GetNodeSize() const 
{     
	return (sizeof(AttrTxtLineSpace)); 
}


/********************************************************************************************

>	virtual void AttrTxtLineSpace::GetDebugDetails()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		-
	Purpose:	Used for debugging purposes during developement. Override in your own class
				and output suitable debugging details.

********************************************************************************************/

void AttrTxtLineSpace::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails( Str );

	
	TCHAR				buf[64];
	tsprintf( buf, 64, _T("\r\nLineSpace = %ld\r\nRatio = %.2f"), Value.Value, Value.Ratio.MakeDouble() );
	//TempStr._MakeMsg( TEXT("\r\nLine space=#1%ld\r\n Ratio=#2%f\r\n"), Value.Value,Value.Ratio.MakeDouble());
	(*Str) += buf;
}


/********************************************************************************************
>	void AttrTxtLineSpace::BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/95
	Inputs:		Scale  - 
				Aspect -
	Purpose:	Transform the attribute using the baseline relative scale and aspect
********************************************************************************************/

void AttrTxtLineSpace::BaseLineRelativeTransform(FIXED16 Scale, FIXED16 Aspect)
{
	// absolute line spacing only - which is 0 if line spacing is relative so has no effect in that case
	TxtLineSpaceAttribute* pLineSpaceAttrVal=(TxtLineSpaceAttribute*)GetAttributeValue();
	pLineSpaceAttrVal->Value = XLONG(pLineSpaceAttrVal->Value) * Scale;
}

/********************************************************************************************

>	BOOL AttrTxtLineSpace::WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	BOOL AttrTxtLineSpace::WritePreChildrenNative(BaseCamelotFilter* pFilter);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pFilter - new camelot filter to save to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves the text line spacing attribute to the new file format filter

********************************************************************************************/

BOOL AttrTxtLineSpace::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtLineSpace::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL AttrTxtLineSpace::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter==NULL, FALSE, "Parameter pFilter == NULL.")

	return CXaraFileTxtLineSpace::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CharDescription methods

/********************************************************************************************
>	CharDescription::CharDescription(WCHAR CharCode, WORD TypefaceHandle, BOOL Bold, BOOL Italic)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		CharCode	  : Unicode character, LO-BYTE identical to ASCII
				TypefaceHandle: This must be in the range 0..16384(2 power 14)
				Bold		  : Bold or not
				Italic		  : Italic or not		 
	Purpose:	CharDescription constructor
********************************************************************************************/

CharDescription::CharDescription(WCHAR CharCode, WORD TypefaceHandle, BOOL Bold, BOOL Italic)
{
	 charHandle = CharCode   <<16;		// CharCode occupies HI-WORD
	 charHandle+=(Bold   ? 1<<15 : 0);	// Top bit of LOW-WORD
	 charHandle+=(Italic ? 1<<14 : 0);	// Next bit 
	 charHandle+= TypefaceHandle; 		// Lowest 14 bits of LOW-WORD
	 
	 ERROR3IF (CharCode!=GetCharCode() || TypefaceHandle!=GetTypefaceHandle() ||
			   Bold!=GetBold() || Italic!=GetItalic(), "Invalid CharHandle generated"); 
}



/********************************************************************************************
>	CharDescription::CharDescription(CharDescription& that)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		Reference to a CharDescriptor
	Purpose:	CharDescription constructor	from another CharDescriptor
********************************************************************************************/

CharDescription::CharDescription(CharDescription& that)
{
	charHandle = that.charHandle;
}
