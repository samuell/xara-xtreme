// $Id: isetattr.cpp 662 2006-03-14 21:31:49Z alex $
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
// isetattr.cpp - Imagesetting attributes (Overprint line/fill, Print on all plates)

#include "camtypes.h"

#include "attrmgr.h"
#include "isetattr.h"
//#include "isetres.h"
#include "rndrgn.h"

// Native file load/save includes
#include "camfiltr.h"
#include "cxfdefs.h"
#include "cxfrec.h"
#include "cxftags.h"
//#include "tim.h"			// For _R(IDE_FILE_WRITE_ERROR)


// Base classes
CC_IMPLEMENT_DYNAMIC(ImagesettingAttrValue, AttributeValue)
CC_IMPLEMENT_DYNCREATE(AttrImagesetting, NodeAttribute)

// Overprint line
CC_IMPLEMENT_DYNCREATE(OverprintLineAttrValue, ImagesettingAttrValue)
CC_IMPLEMENT_DYNCREATE(AttrOverprintLine, AttrImagesetting)

// Overprint fill
CC_IMPLEMENT_DYNCREATE(OverprintFillAttrValue, ImagesettingAttrValue)
CC_IMPLEMENT_DYNCREATE(AttrOverprintFill, AttrImagesetting)

// Print on all plates
CC_IMPLEMENT_DYNCREATE(PrintOnAllPlatesAttrValue, ImagesettingAttrValue)
CC_IMPLEMENT_DYNCREATE(AttrPrintOnAllPlates, AttrImagesetting)

// Version 2 file format loading handler for all Imagesetting attributes
CC_IMPLEMENT_DYNAMIC(ImagesettingAttrRecordHandler, CamelotRecordHandler);



// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	AttrImagesetting::AttrImagesetting()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Purpose:	Default constructor for AttrImagesetting

********************************************************************************************/

AttrImagesetting::AttrImagesetting()
{
}



/********************************************************************************************

>	AttrImagesetting::AttrImagesetting(Node *ContextNode,
										AttachNodeDirection Direction,
										BOOL Locked,
										BOOL Mangled,
										BOOL Marked,
										BOOL Selected)
    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Purpose:	Constructs an AttrImagesetting Attribute

********************************************************************************************/

AttrImagesetting::AttrImagesetting(Node *ContextNode,
									AttachNodeDirection Direction,
									BOOL Locked,
									BOOL Mangled,
									BOOL Marked,
									BOOL Selected)
				: NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
}



/********************************************************************************************

>	virtual void AttrImagesetting::Render(RenderRegion *pRender)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Purpose:	Renders this attribute (by simply calling the Render function of
				its contained AttributeValue)

********************************************************************************************/

void AttrImagesetting::Render(RenderRegion *pRender)
{
	GetAttributeValue()->Render(pRender);
}



/***********************************************************************************************

>	virtual void AttrImagesetting::CopyNodeContents(AttrImagesetting *NodeCopy)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

    Outputs:    NodeCopy - returned containing a copy of this node
		 
    Purpose:	Copies the node's contents to the node pointed to by NodeCopy

***********************************************************************************************/

void AttrImagesetting::CopyNodeContents(AttrImagesetting *NodeCopy)
{
	// Let the base class do its bit
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And then copy our Value
	*(NodeCopy->GetAttributeValue()) = *(GetAttributeValue());
}



/***********************************************************************************************
>   void AttrImagesetting::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrImagesetting::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrImagesetting), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrImagesetting))
		CopyNodeContents((AttrImagesetting*)pNodeCopy);
}















/********************************************************************************************

>	OverprintLineAttrValue::OverprintLineAttrValue()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Purpose:	Default Constuctor for OverprintLineAttrValue
				The Overprint state is set to FALSE

********************************************************************************************/

OverprintLineAttrValue::OverprintLineAttrValue()
{
	OverprintOn = FALSE;
} 



/********************************************************************************************

>	OverprintLineAttrValue::OverprintLineAttrValue(BOOL OverprintIsOn)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		OverprintIsOn - Initial state of line overprint for this attribute

	Purpose:	OverprintLineAttrValue constructor

********************************************************************************************/

OverprintLineAttrValue::OverprintLineAttrValue(BOOL OverprintIsOn)
{ 
	OverprintOn = OverprintIsOn;
}



/********************************************************************************************

>	BOOL OverprintLineAttrValue::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Returns:	TRUE if it initilised successfully

	Purpose:	Registers a default attribute of this type with the attribute manager

********************************************************************************************/

BOOL OverprintLineAttrValue::Init(void)
{
	OverprintLineAttrValue *pAttr = new OverprintLineAttrValue;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(OverprintLineAttrValue),
															pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising OverprintLineAttrValue");

	return(TRUE);
}



/********************************************************************************************

>	virtual void OverprintLineAttrValue::Render(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		pRegion - the render region to render this attribute into.

	Purpose:	Sets the OverprintLineAttrValue attribute for the given render region.

	Notes:		This attribute is so simple that its state can be read directly off
				the render region's stack. Thus, there is minimal special render region
				support for this attribute - we just stack and unstack it directly here.

********************************************************************************************/

void OverprintLineAttrValue::Render(RenderRegion *pRegion, BOOL Temp)
{
	// Stack the current overprint attribute and set ourselves up as the new one
//#pragma message( __LOCMSG__ "OverprintLineAttrValue::Render - do nothing" )
//	TRACE( _T("Warning - OverprintLineAttrValue::Render called\n") );
	pRegion->SetLineOverprint(this, Temp);
}



/********************************************************************************************

>	virtual void OverprintLineAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).

	Purpose:	Restores the OverprintLineAttrValue attribute for the given render region. 

	Notes:		This attribute is so simple that its state can be read directly off
				the render region's stack. Thus, there is minimal special render region
				support for this attribute - we just stack and unstack it directly here.

********************************************************************************************/

void OverprintLineAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "OverprintLineAttrValue::Restore - do nothing" )
//	TRACE( _T("Warning - OverprintLineAttrValue::Restore called\n") );
	pRegion->RestoreLineOverprint(this, Temp);
}



/********************************************************************************************

>	virtual void OverprintLineAttrValue::SimpleCopy(AttributeValue *pValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		pValue - pointer to the AttributeValue to copy

	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void OverprintLineAttrValue::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, OverprintLineAttrValue),
				"Invalid Attribute value passed to OverprintLineAttrValue::SimpleCopy");

	// Just uses the assignment operator
	*this = *((OverprintLineAttrValue *) pValue);
}



/********************************************************************************************

>	virtual NodeAttribute *OverprintLineAttrValue::MakeNode()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Returns:	Pointer to the new node, or NULL if out of memory.

	Purpose:	Make a new attribute node for this type of attr value - see base class

	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *OverprintLineAttrValue::MakeNode()
{
	// Create new attribute node
	AttrOverprintLine *pAttr = new AttrOverprintLine();
	if (pAttr == NULL)
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	return(pAttr);
}



/********************************************************************************************

>	virtual BOOL OverprintLineAttrValue::IsDifferent(AttributeValue *pAttr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Purpose:	Determines if this AttrValue is different from the given one

	Errors:		ERROR3 if the two attributes are not of the same type

	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL OverprintLineAttrValue::IsDifferent(AttributeValue *pAttr)
{
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(OverprintLineAttrValue)),
		   		"Different attribute types in OverprintLineAttrValue::IsDifferent()");

	// Check they are NOT the same using the == operator
	return ( !(*((OverprintLineAttrValue *)pAttr) == *this) );
}



/********************************************************************************************

>	virtual OverprintLineAttrValue &OverprintLineAttrValue::operator=(OverprintLineAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		Attrib - the attribute to copy

	Purpose:	Assignment operator

********************************************************************************************/

OverprintLineAttrValue &OverprintLineAttrValue::operator=(OverprintLineAttrValue &Attrib)
{
	OverprintOn = Attrib.OverprintOn;

	return(*this);
}



/********************************************************************************************

>	virtual INT32 OverprintLineAttrValue::operator==(const ImagesettingAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		Attrib - the attribute to compare this attribute with

	Returns:	TRUE if the attributes are considered equal

	Purpose:	Comparison operator

********************************************************************************************/

INT32 OverprintLineAttrValue::operator==(const ImagesettingAttrValue &Attrib)
{
	ERROR3IF(!Attrib.IsKindOf(CC_RUNTIME_CLASS(OverprintLineAttrValue)),
				"Other attribute value isn't an OverprintLineAttrValue");

	OverprintLineAttrValue *Other = (OverprintLineAttrValue *) &Attrib;
	return(Other->OverprintOn == OverprintOn);
}













/********************************************************************************************

>	AttrOverprintLine::AttrOverprintLine()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Purpose:	Default constructor for AttrOverprintLine

********************************************************************************************/

AttrOverprintLine::AttrOverprintLine()
{
}



/********************************************************************************************

>	AttrOverprintLine::AttrOverprintLine(Node *ContextNode,
										AttachNodeDirection Direction,
										BOOL Locked,
										BOOL Mangled,
										BOOL Marked,
										BOOL Selected)
    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Purpose:	Constructs an AttrOverprintLine Attribute

********************************************************************************************/

AttrOverprintLine::AttrOverprintLine(Node *ContextNode,
									AttachNodeDirection Direction,
									BOOL Locked,
									BOOL Mangled,
									BOOL Marked,
									BOOL Selected)
				: AttrImagesetting(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
}



/********************************************************************************************

>	virtual Node *AttrOverprintLine::SimpleCopy()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    24/7/96

    Returns:    A copy of the node, or NULL if memory runs out 

    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

********************************************************************************************/

Node *AttrOverprintLine::SimpleCopy()
{
	AttrOverprintLine* NodeCopy = new AttrOverprintLine;
	if (NodeCopy == NULL)
		return(NULL);

	// Call the base class
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And call our AttributeValue to copy itself too
	NodeCopy->GetAttributeValue()->SimpleCopy(GetAttributeValue());
    
	return(NodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrOverprintLine::GetAttrNameID(void)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Returns:	Attribute description string-resource ID

	Purpose:	Retrieves a string resource ID describing this attribute

********************************************************************************************/

UINT32 AttrOverprintLine::GetAttrNameID(void)
{
	return(_R(IDS_ATTROVERPRINTLINE));
}



/********************************************************************************************

>	virtual void AttrOverprintLine::GetDebugDetails(StringBase *Str)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Outputs:	On return, Str is filled in with details on this node

	Purpose:	Produces debug details about this node

********************************************************************************************/

void AttrOverprintLine::GetDebugDetails(StringBase *Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails(Str);

	String_256 TempStr;
	TempStr._MakeMsg( _T("\r\nOverprint of Lines is #1%s\r\n"), (Value.IsOverprintOn()) ? _T("ON") : _T("OFF") );
	*Str += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrOverprintLine::GetNodeSize() const

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Returns:	The size of this node, in bytes

	Purpose:	For finding the size of the node, in bytes

********************************************************************************************/

UINT32 AttrOverprintLine::GetNodeSize() const
{
	return(sizeof(AttrOverprintLine));
}



/***********************************************************************************************

>	virtual INT32 AttrOverprintLine::operator==(const NodeAttribute &NodeAttrib); 

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Inputs:		NodeAttrib - The node to compare this node to
	Returns:	TRUE if the nodes are considered equal

    Purpose:	Comparison operator - determines if the AttributeValues	of both objects are ==

***********************************************************************************************/

INT32 AttrOverprintLine::operator==(const NodeAttribute &NodeAttrib)
{
	// First check they are of the same type
	if (((NodeAttribute*)&NodeAttrib)->GetAttributeType() != GetAttributeType())
		return FALSE;

	// Make a more sensible pointer
	AttrOverprintLine *Attr = (AttrOverprintLine *) &NodeAttrib;

	// Now let the AttributeValues compare themselves
	return( *((OverprintLineAttrValue *) Attr->GetAttributeValue())  ==
			*((OverprintLineAttrValue *) GetAttributeValue()) );
}



/********************************************************************************************

>	virtual BOOL AttrOverprintLine::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
>	virtual BOOL AttrOverprintLine::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96

	Inputs:		pFilter - filter to write to

	Returns:	TRUE if the Node has written out a record to the filter

	Purpose:	Writes out a record that represents the node, to either Native or Web
				file format.

				This function is called before any of the AttrOverprintLine's children
				are written to the filter.

				If the AttrOverprintLine writes out a record successfully to the file,
				it will return TRUE.

				If the AttrOverprintLine chooses not to write itself to the filter
				(e.g. because it is not appropriate	for this filter), then this
				function will return FALSE.

	SeeAlso:	Node::WritePreChildrenNative; Node::WritePreChildrenWeb;
				ImagesettingAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL AttrOverprintLine::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	// Imagesetting attributes are not saved in the web file format
	return FALSE;
#else
	return FALSE;
#endif
}

BOOL AttrOverprintLine::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR3IF(pFilter == NULL, "Illegal NULL param");

	CamelotFileRecord *Rec = NULL;
	if (Value.IsOverprintOn())
		Rec = new CamelotFileRecord(pFilter, TAG_OVERPRINTLINEON, TAG_OVERPRINTLINEON_SIZE);
	else
		Rec = new CamelotFileRecord(pFilter, TAG_OVERPRINTLINEOFF, TAG_OVERPRINTLINEOFF_SIZE);

	BOOL ok = (Rec != NULL);

	if (ok) ok = Rec->Init();
	if (ok) ok = pFilter->Write(Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	if (Rec != NULL)		// We've written the record - now delete it
		delete Rec;

	return(ok);
#else
	return FALSE;
#endif
}












/********************************************************************************************

>	OverprintFillAttrValue::OverprintFillAttrValue()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Purpose:	Default Constuctor for OverprintFillAttrValue
				The Overprint state is set to FALSE

********************************************************************************************/

OverprintFillAttrValue::OverprintFillAttrValue()
{
	OverprintOn = FALSE;
} 



/********************************************************************************************

>	OverprintFillAttrValue::OverprintFillAttrValue(BOOL OverprintIsOn)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		OverprintIsOn - Initial state of Fill overprint for this attribute

	Purpose:	OverprintFillAttrValue constructor

********************************************************************************************/

OverprintFillAttrValue::OverprintFillAttrValue(BOOL OverprintIsOn)
{ 
	OverprintOn = OverprintIsOn;
}



/********************************************************************************************

>	BOOL OverprintFillAttrValue::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Returns:	TRUE if it initilised successfully

	Purpose:	Registers a default attribute of this type with the attribute manager

********************************************************************************************/

BOOL OverprintFillAttrValue::Init(void)
{
	OverprintFillAttrValue *pAttr = new OverprintFillAttrValue;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(OverprintFillAttrValue),
															pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising OverprintFillAttrValue");

	return(TRUE);
}



/********************************************************************************************

>	virtual void OverprintFillAttrValue::Render(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		pRegion - the render region to render this attribute into.

	Purpose:	Sets the OverprintFillAttrValue attribute for the given render region.

	Notes:		This attribute is so simple that its state can be read directly off
				the render region's stack. Thus, there is minimal special render region
				support for this attribute - we just stack and unstack it directly here.

********************************************************************************************/

void OverprintFillAttrValue::Render(RenderRegion *pRegion, BOOL Temp)
{
	// Stack the current overprint attribute and set ourselves up as the new one
//#pragma message( __LOCMSG__ "OverprintFillAttrValue::Render - do nothing" )
//	TRACE( _T("Warning - OverprintFillAttrValue::Render called\n") );
	pRegion->SetFillOverprint(this, Temp);
}



/********************************************************************************************

>	virtual void OverprintFillAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).

	Purpose:	Restores the OverprintFillAttrValue attribute for the given render region. 

	Notes:		This attribute is so simple that its state can be read directly off
				the render region's stack. Thus, there is minimal special render region
				support for this attribute - we just stack and unstack it directly here.

********************************************************************************************/

void OverprintFillAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "OverprintFillAttrValue::Restore - do nothing" )
//	TRACE( _T("Warning - OverprintFillAttrValue::Restore called\n") );
	pRegion->RestoreFillOverprint(this, Temp);
}



/********************************************************************************************

>	virtual void OverprintFillAttrValue::SimpleCopy(AttributeValue *pValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		pValue - pointer to the AttributeValue to copy

	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void OverprintFillAttrValue::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, OverprintFillAttrValue),
				"Invalid Attribute value passed to OverprintFillAttrValue::SimpleCopy");

	// Just uses the assignment operator
	*this = *((OverprintFillAttrValue *) pValue);
}



/********************************************************************************************

>	virtual NodeAttribute *OverprintFillAttrValue::MakeNode()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Returns:	Pointer to the new node, or NULL if out of memory.

	Purpose:	Make a new attribute node for this type of attr value - see base class

	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *OverprintFillAttrValue::MakeNode()
{
	// Create new attribute node
	AttrOverprintFill *pAttr = new AttrOverprintFill();
	if (pAttr == NULL)
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	return(pAttr);
}



/********************************************************************************************

>	virtual BOOL OverprintFillAttrValue::IsDifferent(AttributeValue *pAttr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Purpose:	Determines if this AttrValue is different from the given one

	Errors:		ERROR3 if the two attributes are not of the same type

	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL OverprintFillAttrValue::IsDifferent(AttributeValue *pAttr)
{
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(OverprintFillAttrValue)),
		   		"Different attribute types in OverprintFillAttrValue::IsDifferent()");

	// Check they are NOT the same using the == operator
	return ( !(*((OverprintFillAttrValue *)pAttr) == *this) );
}



/********************************************************************************************

>	virtual OverprintFillAttrValue &OverprintFillAttrValue::operator=(OverprintFillAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		Attrib - the attribute to copy

	Purpose:	Assignment operator

********************************************************************************************/

OverprintFillAttrValue &OverprintFillAttrValue::operator=(OverprintFillAttrValue &Attrib)
{
	OverprintOn = Attrib.OverprintOn;

	return(*this);
}



/********************************************************************************************

>	virtual INT32 OverprintFillAttrValue::operator==(const ImagesettingAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		Attrib - the attribute to compare this attribute with

	Returns:	TRUE if the attributes are considered equal

	Purpose:	Comparison operator

********************************************************************************************/

INT32 OverprintFillAttrValue::operator==(const ImagesettingAttrValue &Attrib)
{
	ERROR3IF(!Attrib.IsKindOf(CC_RUNTIME_CLASS(OverprintFillAttrValue)),
				"Other attribute value isn't an OverprintFillAttrValue");

	OverprintFillAttrValue *Other = (OverprintFillAttrValue *) &Attrib;
	return(Other->OverprintOn == OverprintOn);
}













/********************************************************************************************

>	AttrOverprintFill::AttrOverprintFill()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Purpose:	Default constructor for AttrOverprintFill

********************************************************************************************/

AttrOverprintFill::AttrOverprintFill()
{
}



/********************************************************************************************

>	AttrOverprintFill::AttrOverprintFill(Node *ContextNode,
										AttachNodeDirection Direction,
										BOOL Locked,
										BOOL Mangled,
										BOOL Marked,
										BOOL Selected)
    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Purpose:	Constructs an AttrOverprintFill Attribute

********************************************************************************************/

AttrOverprintFill::AttrOverprintFill(Node *ContextNode,
									AttachNodeDirection Direction,
									BOOL Locked,
									BOOL Mangled,
									BOOL Marked,
									BOOL Selected)
				: AttrImagesetting(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
}



/********************************************************************************************

>	virtual Node *AttrOverprintFill::SimpleCopy()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    24/7/96

    Returns:    A copy of the node, or NULL if memory runs out 

    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

********************************************************************************************/

Node *AttrOverprintFill::SimpleCopy()
{
	AttrOverprintFill* NodeCopy = new AttrOverprintFill;
	if (NodeCopy == NULL)
		return(NULL);

	// Call the base class
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And call our AttributeValue to copy itself too
	NodeCopy->GetAttributeValue()->SimpleCopy(GetAttributeValue());
    
	return(NodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrOverprintFill::GetAttrNameID(void)  

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Returns:	Attribute description string-resource ID

	Purpose:	Retrieves a string resource ID describing this attribute

********************************************************************************************/

UINT32 AttrOverprintFill::GetAttrNameID(void)
{
	return(_R(IDS_ATTROVERPRINTFILL));
}



/********************************************************************************************

>	virtual void AttrOverprintFill::GetDebugDetails(StringBase *Str)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Outputs:	On return, Str is filled in with details on this node

	Purpose:	Produces debug details about this node

********************************************************************************************/

void AttrOverprintFill::GetDebugDetails(StringBase *Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails(Str);

	String_256 TempStr;
	TempStr._MakeMsg( _T("\r\nOverprint of Fill is #1%s\r\n"), (Value.IsOverprintOn()) ? _T("ON") : _T("OFF") );
	*Str += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrOverprintFill::GetNodeSize() const

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Returns:	The size of this node, in bytes

	Purpose:	For finding the size of the node, in bytes

********************************************************************************************/

UINT32 AttrOverprintFill::GetNodeSize() const
{
	return(sizeof(AttrOverprintFill));
}



/***********************************************************************************************

>	virtual INT32 AttrOverprintFill::operator==(const NodeAttribute &NodeAttrib); 

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Inputs:		NodeAttrib - The node to compare this node to
	Returns:	TRUE if the nodes are considered equal

    Purpose:	Comparison operator - determines if the AttributeValues	of both objects are ==

***********************************************************************************************/

INT32 AttrOverprintFill::operator==(const NodeAttribute &NodeAttrib)
{
	// First check they are of the same type
	if (((NodeAttribute*)&NodeAttrib)->GetAttributeType() != GetAttributeType())
		return FALSE;

	// Make a more sensible pointer
	AttrOverprintFill *Attr = (AttrOverprintFill *) &NodeAttrib;

	// Now let the AttributeValues compare themselves
	return( *((OverprintFillAttrValue *) Attr->GetAttributeValue())  ==
			*((OverprintFillAttrValue *) GetAttributeValue()) );
}



/********************************************************************************************

>	virtual BOOL AttrOverprintFill::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
>	virtual BOOL AttrOverprintFill::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96

	Inputs:		pFilter - filter to write to

	Returns:	TRUE if the Node has written out a record to the filter

	Purpose:	Writes out a record that represents the node, to either Native or Web
				file format.

				This function is called before any of the AttrOverprintFill's children
				are written to the filter.

				If the AttrOverprintFill writes out a record successfully to the file,
				it will return TRUE.

				If the AttrOverprintFill chooses not to write itself to the filter
				(e.g. because it is not appropriate	for this filter), then this
				function will return FALSE.

	SeeAlso:	Node::WritePreChildrenNative; Node::WritePreChildrenWeb;
				ImagesettingAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL AttrOverprintFill::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	// Imagesetting attributes are not saved in the web file format
	return FALSE;
#else
	return FALSE;
#endif
}

BOOL AttrOverprintFill::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR3IF(pFilter == NULL, "Illegal NULL param");

	CamelotFileRecord *Rec = NULL;
	if (Value.IsOverprintOn())
		Rec = new CamelotFileRecord(pFilter, TAG_OVERPRINTFILLON, TAG_OVERPRINTFILLON_SIZE);
	else
		Rec = new CamelotFileRecord(pFilter, TAG_OVERPRINTFILLOFF, TAG_OVERPRINTFILLOFF_SIZE);

	BOOL ok = (Rec != NULL);

	if (ok) ok = Rec->Init();
	if (ok) ok = pFilter->Write(Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	if (Rec != NULL)		// We've written the record - now delete it
		delete Rec;

	return(ok);
#else
	return FALSE;
#endif
}












/********************************************************************************************

>	PrintOnAllPlatesAttrValue::PrintOnAllPlatesAttrValue()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Purpose:	Default Constuctor for PrintOnAllPlatesAttrValue
				The "Print On All Plates" state is set to FALSE

********************************************************************************************/

PrintOnAllPlatesAttrValue::PrintOnAllPlatesAttrValue()
{
	PrintOnAllPlates = FALSE;
} 



/********************************************************************************************

>	PrintOnAllPlatesAttrValue::PrintOnAllPlatesAttrValue(BOOL PrintOnAllPlatesIsOn)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		PrintOnAllPlatesIsOn - Initial state of PrintOnAllPlates for this attribute

	Purpose:	PrintOnAllPlatesAttrValue constructor

********************************************************************************************/

PrintOnAllPlatesAttrValue::PrintOnAllPlatesAttrValue(BOOL PrintOnAllPlatesIsOn)
{ 
	PrintOnAllPlates = PrintOnAllPlatesIsOn;
}



/********************************************************************************************

>	BOOL PrintOnAllPlatesAttrValue::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Returns:	TRUE if it initilised successfully

	Purpose:	Registers a default attribute of this type with the attribute manager

********************************************************************************************/

BOOL PrintOnAllPlatesAttrValue::Init(void)
{
	PrintOnAllPlatesAttrValue *pAttr = new PrintOnAllPlatesAttrValue;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(PrintOnAllPlatesAttrValue),
															pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising PrintOnAllPlatesAttrValue");

	return(TRUE);
}



/********************************************************************************************

>	virtual void PrintOnAllPlatesAttrValue::Render(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		pRegion - the render region to render this attribute into.

	Purpose:	Sets the PrintOnAllPlatesAttrValue attribute for the given render region.

	Notes:		This attribute is so simple that its state can be read directly off
				the render region's stack. Thus, there is minimal special render region
				support for this attribute - we just stack and unstack it directly here.

********************************************************************************************/

void PrintOnAllPlatesAttrValue::Render(RenderRegion *pRegion, BOOL Temp)
{
	// Stack the current overprint attribute and set ourselves up as the new one
//#pragma message( __LOCMSG__ "PrintOnAllPlatesAttrValue::Render - do nothing" )
//	TRACE( _T("Warning - PrintOnAllPlatesAttrValue::Render called\n") );
	pRegion->SetPrintOnAllPlates(this, Temp);
}



/********************************************************************************************

>	virtual void PrintOnAllPlatesAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).

	Purpose:	Restores the PrintOnAllPlatesAttrValue attribute for the given render region. 

	Notes:		This attribute is so simple that its state can be read directly off
				the render region's stack. Thus, there is minimal special render region
				support for this attribute - we just stack and unstack it directly here.

********************************************************************************************/

void PrintOnAllPlatesAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)
{
//#pragma message( __LOCMSG__ "PrintOnAllPlatesAttrValue::Restore - do nothing" )
//	TRACE( _T("Warning - PrintOnAllPlatesAttrValue::Restore called\n") );
	pRegion->RestorePrintOnAllPlates(this, Temp);
}



/********************************************************************************************

>	virtual void PrintOnAllPlatesAttrValue::SimpleCopy(AttributeValue *pValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		pValue - pointer to the AttributeValue to copy

	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void PrintOnAllPlatesAttrValue::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, PrintOnAllPlatesAttrValue),
				"Invalid Attribute value passed to PrintOnAllPlatesAttrValue::SimpleCopy");

	// Just uses the assignment operator
	*this = *((PrintOnAllPlatesAttrValue *) pValue);
}



/********************************************************************************************

>	virtual NodeAttribute *PrintOnAllPlatesAttrValue::MakeNode()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Returns:	Pointer to the new node, or NULL if out of memory.

	Purpose:	Make a new attribute node for this type of attr value - see base class

	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *PrintOnAllPlatesAttrValue::MakeNode()
{
	// Create new attribute node
	AttrPrintOnAllPlates *pAttr = new AttrPrintOnAllPlates();
	if (pAttr == NULL)
		return NULL;

	// Copy attribute value into the new node.
	pAttr->Value.SimpleCopy(this);

	return(pAttr);
}



/********************************************************************************************

>	virtual BOOL PrintOnAllPlatesAttrValue::IsDifferent(AttributeValue *pAttr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Purpose:	Determines if this AttrValue is different from the given one

	Errors:		ERROR3 if the two attributes are not of the same type

	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL PrintOnAllPlatesAttrValue::IsDifferent(AttributeValue *pAttr)
{
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(PrintOnAllPlatesAttrValue)),
		   		"Different attribute types in PrintOnAllPlatesAttrValue::IsDifferent()");

	// Check they are NOT the same using the == operator
	return ( !(*((PrintOnAllPlatesAttrValue *)pAttr) == *this) );
}



/********************************************************************************************

>	virtual PrintOnAllPlatesAttrValue &PrintOnAllPlatesAttrValue::operator=(PrintOnAllPlatesAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		Attrib - the attribute to copy

	Purpose:	Assignment operator

********************************************************************************************/

PrintOnAllPlatesAttrValue &PrintOnAllPlatesAttrValue::operator=(PrintOnAllPlatesAttrValue &Attrib)
{
	PrintOnAllPlates = Attrib.PrintOnAllPlates;

	return(*this);
}



/********************************************************************************************

>	virtual INT32 PrintOnAllPlatesAttrValue::operator==(const ImagesettingAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/96

	Inputs:		Attrib - the attribute to compare this attribute with

	Returns:	TRUE if the attributes are considered equal

	Purpose:	Comparison operator

********************************************************************************************/

INT32 PrintOnAllPlatesAttrValue::operator==(const ImagesettingAttrValue &Attrib)
{
	ERROR3IF(!Attrib.IsKindOf(CC_RUNTIME_CLASS(PrintOnAllPlatesAttrValue)),
				"Other attribute value isn't an PrintOnAllPlatesAttrValue");

	PrintOnAllPlatesAttrValue *Other = (PrintOnAllPlatesAttrValue *) &Attrib;
	return(Other->PrintOnAllPlates == PrintOnAllPlates);
}













/********************************************************************************************

>	AttrPrintOnAllPlates::AttrPrintOnAllPlates()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Purpose:	Default constructor for AttrPrintOnAllPlates

********************************************************************************************/

AttrPrintOnAllPlates::AttrPrintOnAllPlates()
{
}



/********************************************************************************************

>	AttrPrintOnAllPlates::AttrPrintOnAllPlates(Node *ContextNode,
										AttachNodeDirection Direction,
										BOOL Locked,
										BOOL Mangled,
										BOOL Marked,
										BOOL Selected)
    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Purpose:	Constructs an AttrPrintOnAllPlates Attribute

********************************************************************************************/

AttrPrintOnAllPlates::AttrPrintOnAllPlates(Node *ContextNode,
									AttachNodeDirection Direction,
									BOOL Locked,
									BOOL Mangled,
									BOOL Marked,
									BOOL Selected)
				: AttrImagesetting(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
}



/********************************************************************************************

>	virtual Node *AttrPrintOnAllPlates::SimpleCopy()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    24/7/96

    Returns:    A copy of the node, or NULL if memory runs out 

    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

********************************************************************************************/

Node *AttrPrintOnAllPlates::SimpleCopy()
{
	AttrPrintOnAllPlates* NodeCopy = new AttrPrintOnAllPlates;
	if (NodeCopy == NULL)
		return(NULL);

	// Call the base class
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And call our AttributeValue to copy itself too
	NodeCopy->GetAttributeValue()->SimpleCopy(GetAttributeValue());
    
	return(NodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrPrintOnAllPlates::GetAttrNameID(void)  

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Returns:	Attribute description string-resource ID

	Purpose:	Retrieves a string resource ID describing this attribute

********************************************************************************************/

UINT32 AttrPrintOnAllPlates::GetAttrNameID(void)
{
	return(_R(IDS_ATTRPRINTONALLPLATES));
}



/********************************************************************************************

>	virtual void AttrPrintOnAllPlates::GetDebugDetails(StringBase *Str)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Outputs:	On return, Str is filled in with details on this node

	Purpose:	Produces debug details about this node

********************************************************************************************/

void AttrPrintOnAllPlates::GetDebugDetails(StringBase *Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails(Str);

	String_256 TempStr;
	TempStr._MakeMsg( _T("\r\nPrintOnAllPlates is #1%s\r\n"), (Value.IsPrintOnAllPlatesOn()) ? _T("ON") : _T("OFF") );
	*Str += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrPrintOnAllPlates::GetNodeSize() const

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Returns:	The size of this node, in bytes

	Purpose:	For finding the size of the node, in bytes

********************************************************************************************/

UINT32 AttrPrintOnAllPlates::GetNodeSize() const
{
	return(sizeof(AttrPrintOnAllPlates));
}



/***********************************************************************************************

>	virtual INT32 AttrPrintOnAllPlates::operator==(const NodeAttribute &NodeAttrib); 

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/7/96

	Inputs:		NodeAttrib - The node to compare this node to
	Returns:	TRUE if the nodes are considered equal

    Purpose:	Comparison operator - determines if the AttributeValues	of both objects are ==

***********************************************************************************************/

INT32 AttrPrintOnAllPlates::operator==(const NodeAttribute &NodeAttrib)
{
	// First check they are of the same type
	if (((NodeAttribute*)&NodeAttrib)->GetAttributeType() != GetAttributeType())
		return FALSE;

	// Make a more sensible pointer
	AttrPrintOnAllPlates *Attr = (AttrPrintOnAllPlates *) &NodeAttrib;

	// Now let the AttributeValues compare themselves
	return( *((PrintOnAllPlatesAttrValue *) Attr->GetAttributeValue())  ==
			*((PrintOnAllPlatesAttrValue *) GetAttributeValue()) );
}



/********************************************************************************************

>	virtual BOOL AttrPrintOnAllPlates::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
>	virtual BOOL AttrPrintOnAllPlates::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96

	Inputs:		pFilter - filter to write to

	Returns:	TRUE if the Node has written out a record to the filter

	Purpose:	Writes out a record that represents the node, to either Native or Web
				file format.

				This function is called before any of the AttrPrintOnAllPlates's children
				are written to the filter.

				If the AttrPrintOnAllPlates writes out a record successfully to the file,
				it will return TRUE.

				If the AttrPrintOnAllPlates chooses not to write itself to the filter
				(e.g. because it is not appropriate	for this filter), then this
				function will return FALSE.

	SeeAlso:	Node::WritePreChildrenNative; Node::WritePreChildrenWeb;
				ImagesettingAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL AttrPrintOnAllPlates::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	// Imagesetting attributes are not saved in the web file format
	return FALSE;
#else
	return FALSE;
#endif
}

BOOL AttrPrintOnAllPlates::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR3IF(pFilter == NULL, "Illegal NULL param");

	CamelotFileRecord *Rec = NULL;
	if (Value.IsPrintOnAllPlatesOn())
		Rec = new CamelotFileRecord(pFilter, TAG_PRINTONALLPLATESON, TAG_PRINTONALLPLATESON_SIZE);
	else
		Rec = new CamelotFileRecord(pFilter, TAG_PRINTONALLPLATESOFF, TAG_PRINTONALLPLATESOFF_SIZE);

	BOOL ok = (Rec != NULL);

	if (ok) ok = Rec->Init();
	if (ok) ok = pFilter->Write(Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	if (Rec != NULL)		// We've written the record - now delete it
		delete Rec;

	return(ok);
#else
	return FALSE;
#endif
}












/********************************************************************************************

>	virtual UINT32 *ImagesettingAttrRecordHandler::GetTagList()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96

	Returns:	A list of tag values, terminated by CXFRH_TAG_LIST_END

	Purpose:	Provides the record handler system with a list of records handled by this
				handler - all Imagesetting attributes:
					Overprint line (on/off)
					Overprint fill (on/off)
					Print on all plates (on/off)

	SeeAlso:	ImagesettingAttrRecordHandler::HandleRecord

********************************************************************************************/

UINT32 *ImagesettingAttrRecordHandler::GetTagList()
{
	static UINT32 TagList[] =
	{
		TAG_OVERPRINTLINEON,
		TAG_OVERPRINTLINEOFF,
		TAG_OVERPRINTFILLON,
		TAG_OVERPRINTFILLOFF,
		TAG_PRINTONALLPLATESON,
		TAG_PRINTONALLPLATESOFF,
		CXFRH_TAG_LIST_END
	};

	return(TagList);
}



/********************************************************************************************

>	virtual BOOL ImagesettingAttrRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)

 	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96

	Inputs:		pCXaraFileRecord - The record to handle - may not be NULL
	Returns:	TRUE if handled successfuly

	Purpose:	Handles loading of the given imagesetting attribute record

	SeeAlso:	AttrOverprintLine::WritePreChildrenNative;
				AttrOverprintFill::WritePreChildrenNative;
				AttrPrintOnAllPlates::WritePreChildrenNative

********************************************************************************************/

BOOL ImagesettingAttrRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR3IF(pCXaraFileRecord == NULL, "pCXaraFileRecord is NULL");

	NodeAttribute *NewNode = NULL;
	
	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_OVERPRINTLINEON:
			NewNode = new AttrOverprintLine;
			if (NewNode != NULL)
				((OverprintLineAttrValue *)NewNode->GetAttributeValue())->SetOverprint(TRUE);
			break;

		case TAG_OVERPRINTLINEOFF:
			NewNode = new AttrOverprintLine;
			if (NewNode != NULL)
				((OverprintLineAttrValue *)NewNode->GetAttributeValue())->SetOverprint(FALSE);
			break;

		case TAG_OVERPRINTFILLON:
			NewNode = new AttrOverprintFill;
			if (NewNode != NULL)
				((OverprintFillAttrValue *)NewNode->GetAttributeValue())->SetOverprint(TRUE);
			break;

		case TAG_OVERPRINTFILLOFF:
			NewNode = new AttrOverprintFill;
			if (NewNode != NULL)
				((OverprintFillAttrValue *)NewNode->GetAttributeValue())->SetOverprint(FALSE);
			break;

		case TAG_PRINTONALLPLATESON:
			NewNode = new AttrPrintOnAllPlates;
			if (NewNode != NULL)
				((PrintOnAllPlatesAttrValue *)NewNode->GetAttributeValue())->SetPrintOnAllPlates(TRUE);
			break;

		case TAG_PRINTONALLPLATESOFF:
			NewNode = new AttrPrintOnAllPlates;
			if (NewNode != NULL)
				((PrintOnAllPlatesAttrValue *)NewNode->GetAttributeValue())->SetPrintOnAllPlates(FALSE);
			break;

		default:
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	if (NewNode == NULL)
		return(FALSE);

	// Get the base class to insert the new node for us
	InsertNode(NewNode);
	return(TRUE);
}


