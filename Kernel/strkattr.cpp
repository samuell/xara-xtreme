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
// strkattr.cpp - Path stroking attributes

#include "camtypes.h"

//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "jason.h"
#include "ppairbsh.h"		// PathProcessorStrokeAirbrush
#include "ppstroke.h"		// PathProcessorStroke
#include "ppvecstr.h"		// PathProcessorStrokeVector
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "strkattr.h"
#include "strkcomp.h"
#include "valfunc.h"

// Native file load/save includes
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"
//#include "tim.h"			// For _R(IDE_FILE_WRITE_ERROR)

//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodepath.h"
#include "pbecomea.h" // for pathbecomea
// Stroke classes
CC_IMPLEMENT_DYNCREATE(AttrStrokeType, NodeAttribute)
CC_IMPLEMENT_DYNAMIC(StrokeTypeAttrValue, AttributeValue)

// Variable width classes
CC_IMPLEMENT_DYNCREATE(AttrVariableWidth, NodeAttribute)
CC_IMPLEMENT_DYNAMIC(VariableWidthAttrValue, AttributeValue)

// Version 2 file format loading handler for all Stroke attributes
CC_IMPLEMENT_DYNAMIC(StrokeAttrRecordHandler, CamelotRecordHandler);


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



/********************************************************************************************

>	AttrStrokeType::AttrStrokeType()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    7/1/97

	Purpose:	Default constructor for AttrStrokeType

********************************************************************************************/

AttrStrokeType::AttrStrokeType()
{
}



/********************************************************************************************

>	AttrStrokeType::AttrStrokeType(Node *ContextNode,
										AttachNodeDirection Direction,
										BOOL Locked,
										BOOL Mangled,
										BOOL Marked,
										BOOL Selected)
    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    7/1/97

	Purpose:	Constructs an AttrStrokeType Attribute

********************************************************************************************/

AttrStrokeType::AttrStrokeType(Node *ContextNode,
									AttachNodeDirection Direction,
									BOOL Locked,
									BOOL Mangled,
									BOOL Marked,
									BOOL Selected)
				: NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
}



/********************************************************************************************

>	virtual void AttrStrokeType::Render(RenderRegion *pRender)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	Renders this attribute (by simply calling the Render function of
				its contained AttributeValue)

********************************************************************************************/

void AttrStrokeType::Render(RenderRegion *pRender)
{
	GetAttributeValue()->Render(pRender);
}



/***********************************************************************************************

>	virtual void AttrStrokeType::CopyNodeContents(AttrStrokeType *NodeCopy)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/1/97

    Outputs:    NodeCopy - returned containing a copy of this node
		 
    Purpose:	Copies the node's contents to the node pointed to by NodeCopy

***********************************************************************************************/

void AttrStrokeType::CopyNodeContents(AttrStrokeType *NodeCopy)
{
	// Let the base class do its bit
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And then copy our Value
	*(NodeCopy->GetAttributeValue()) = *(GetAttributeValue());
}



/***********************************************************************************************
>   void AttrStrokeType::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrStrokeType::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrStrokeType), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrStrokeType))
		CopyNodeContents((AttrStrokeType*)pNodeCopy);
}



/***********************************************************************************************

>	virtual INT32 AttrStrokeType::operator==(const NodeAttribute &NodeAttrib); 

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/1/97

	Inputs:		NodeAttrib - The node to compare this node to
	Returns:	TRUE if the nodes are considered equal

    Purpose:	Comparison operator - determines if the AttributeValues	of both objects are ==

***********************************************************************************************/

INT32 AttrStrokeType::operator==(const NodeAttribute &NodeAttrib)
{
	// First check they are of the same type
	if (((NodeAttribute*)&NodeAttrib)->GetAttributeType() != GetAttributeType())
		return FALSE;

	// Make a more sensible pointer
	AttrStrokeType *Attr = (AttrStrokeType *) &NodeAttrib;

	// Now let the AttributeValues compare themselves
	return( *((StrokeTypeAttrValue *) Attr->GetAttributeValue())  ==
			*((StrokeTypeAttrValue *) GetAttributeValue()) );
}



/********************************************************************************************

>	virtual Node *AttrStrokeType::SimpleCopy()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/1/97

    Returns:    A copy of the node, or NULL if memory runs out 

    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

********************************************************************************************/

Node *AttrStrokeType::SimpleCopy()
{
	AttrStrokeType* NodeCopy = new AttrStrokeType;
	if (NodeCopy == NULL)
		return(NULL);

	// Call the base class
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And call our AttributeValue to copy itself too
	NodeCopy->GetAttributeValue()->SimpleCopy(GetAttributeValue());
    
	return(NodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrStrokeType::GetAttrNameID(void)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/1/97

	Returns:	Attribute description string-resource ID

	Purpose:	Retrieves a string resource ID describing this attribute

********************************************************************************************/

UINT32 AttrStrokeType::GetAttrNameID(void)
{
	return(_R(IDS_ATTRSTROKETYPE));
}

/********************************************************************************************
>	virtual BOOL AttrStokeType::NeedsTransparency() const

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this Attribute requires transparency mode to render properly.
	Purpose:	Strokes like the airbrush require transparency to be turned on to work.
	Errors:		-
********************************************************************************************/

BOOL AttrStrokeType::NeedsTransparency() const
{
	BOOL SoWeDoReallyNeedTransparencyThenDoWe = FALSE;
	
	PathProcessorStroke *pPathProc = Value.GetPathProcessor();
	if(pPathProc != NULL)
	{
		SoWeDoReallyNeedTransparencyThenDoWe = pPathProc->NeedsTransparency();
	}

	return SoWeDoReallyNeedTransparencyThenDoWe;
}


/********************************************************************************************

>	virtual void AttrStrokeType::GetDebugDetails(StringBase *Str)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/1/97

	Outputs:	On return, Str is filled in with details on this node

	Purpose:	Produces debug details about this node

********************************************************************************************/

void AttrStrokeType::GetDebugDetails(StringBase *Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails(Str);

	String_256 TempStr;
	TempStr._MakeMsg( _T("\r\nStroke type #1%s\r\n"),
						(Value.GetPathProcessor() == NULL) ? _T("old-style line") : _T("new stroke:") );
	*Str += TempStr;

	if (Value.GetPathProcessor() != NULL)
	{
		TempStr._MakeMsg( _T("  #1%s\r\n"),
							Value.GetPathProcessor()->GetRuntimeClass()->GetClassName() );
		*Str += TempStr;
	}

	TempStr._MakeMsg( _T("\r\nNeedsTransparency=#1%s\r\n"), NeedsTransparency() ? _T("TRUE") : _T("FALSE") );
	*Str += TempStr;

#endif
}


/********************************************************************************************

>	BOOL AttrStrokeType::HasPathProcessor()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if attribute value has a path processor, FALSE otherwise
	Purpose:	A good way to determine if this is one of the default attributes

********************************************************************************************/

BOOL AttrStrokeType::HasPathProcessor()
{
	if (Value.GetPathProcessor() == NULL)
		return FALSE;
	return TRUE;
}


/********************************************************************************************

>	PathProcessorStroke* AttrStrokeType::GetPathProcessor()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		-
	Outputs:	-
	Returns:	the path processor of our attribute value
	Purpose:	Convenience function

********************************************************************************************/

PathProcessorStroke* AttrStrokeType::GetPathProcessor()
{
	return Value.GetPathProcessor();
}



/********************************************************************************************

>	virtual BOOL AttrStrokeType::DoBecomeA(BecomeA* pBecomeA, Node* pParent)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		pBecomeA - the becomeA object that tells us what to do
				pParent - the node that this attribute is applied to
	Outputs:	
	Returns:	TRUE if everything went ok,
	Purpose:	Its a little unusual for attributes to have their own dobecomea function but 
				this stroke attribute requires one due to all the work it does on paths.

********************************************************************************************/

BOOL AttrStrokeType::DoBecomeA(BecomeA* pBecomeA, Node* pParent)
{
	ERROR2IF(pBecomeA == NULL, FALSE, "BecomeA pointer is NULL in AttrBrushType::DoBecomeA");
	ERROR2IF(pParent == NULL, FALSE, "Parent node is NULL in AttrBrushType::DoBecomeA");
	return Value.DoBecomeA(pBecomeA, pParent);
}


/********************************************************************************************

>	virtual UINT32 AttrStrokeType::GetNodeSize() const

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Returns:	The size of this node, in bytes

	Purpose:	For finding the size of the node, in bytes

********************************************************************************************/

UINT32 AttrStrokeType::GetNodeSize() const
{
	return(sizeof(AttrStrokeType));
}



/********************************************************************************************

>	virtual BOOL AttrStrokeType::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
>	virtual BOOL AttrStrokeType::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Inputs:		pFilter - filter to write to

	Returns:	TRUE if the Node has written out a record to the filter

	Purpose:	Writes out a record that represents the node, to either Native or Web
				file format.

				This function is called before any of the AttrStrokeType's children
				are written to the filter.

				If the AttrStrokeType writes out a record successfully to the file,
				it will return TRUE.

				If the AttrStrokeType chooses not to write itself to the filter
				(e.g. because it is not appropriate	for this filter), then this
				function will return FALSE.

	Notes:		Simple StrokeTypes (not using fancy bitmap/vector brushes) just
				write out a 4-byte (UINT32) record containing the stroke type.
				Defined stroke types at this time are:
					0 - Simple variable width stroke

	SeeAlso:	Node::WritePreChildrenNative; Node::WritePreChildrenWeb;
				StrokeAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL AttrStrokeType::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
	return(WritePreChildrenNative(pFilter));
}

BOOL AttrStrokeType::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR3IF(pFilter == NULL, "Illegal NULL param");

	BOOL ok = TRUE;
	PathProcessorStroke *pProcessor = Value.GetPathProcessor();

	// Jason's being lazy this month, I'm afraid, because he won't be here next month...
	if (pProcessor == NULL ||
		!pProcessor->IsKindOf(CC_RUNTIME_CLASS(PathProcessorStrokeAirbrush)))
	{
		// It is a simple var-width stroke, or it is a vector stroke. These both export as
		// the same basic record tag, but with different "handle" words
		StrokeHandle Handle = 0x02000000;

		// Get the handle of the stroke
		if (pProcessor == NULL)
			Handle = 0x01000000;
		else if (pProcessor->IsKindOf(CC_RUNTIME_CLASS(PathProcessorStrokeVector)))
		{
			Handle = ((PathProcessorStrokeVector *)pProcessor)->GetStrokeDefinition();
			ERROR3IF((Handle & 0xff000000) != 0, "Handle overflow");

			// Make sure the stroke definition has preceeded any references to it
			ok = StrokeComponent::ExportStroke(pFilter, Handle);
		}
		//else
		//	it's a simple variable-width (base class) stroker, so we need not do anything

		// And write the record
		if (ok)
		{
			CamelotFileRecord Rec(pFilter, TAG_STROKETYPE, TAG_STROKETYPE_SIZE);

			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteUINT32((UINT32) Handle);
			if (ok) ok = pFilter->Write(&Rec);
		}
	}
	else
	{
		PathProcessorStrokeAirbrush *pProcessor = (PathProcessorStrokeAirbrush *)
													Value.GetPathProcessor();

		ValueFunction *pFunction = pProcessor->GetIntensityFunction();
		ERROR3IF(pFunction == NULL, "No intensity function!?");

		// Currently, an airbrush record consists only of its intensity ValueFunction
		CamelotFileRecord *pRec = pFunction->WriteFileRecord(TAG_STROKEAIRBRUSH, 0, pFilter);
		if (pRec != NULL)
		{
			ok = pFilter->Write(pRec);
			delete pRec;
			pRec = NULL;
		}
		else
			ok = FALSE;
	}

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return(ok);
#else
	return FALSE;
#endif
}
















/********************************************************************************************

>	StrokeTypeAttrValue::StrokeTypeAttrValue(PathProcessorStroke *pPathProcessor = NULL);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pPathProcessor - the stroke path-processor which will do the stroking
				effect "applied" by this attribute. NULL indicates that this attribute
				does no stroking (i.e. that the path should have an old-style "line")

				NOTE that this object is NOW OWNED by this StrokeTypeAttrValue, and will be
				deleted when the attribute value is deleted.

	Purpose:	Default Constuctor for StrokeTypeAttrValue

********************************************************************************************/

StrokeTypeAttrValue::StrokeTypeAttrValue(PathProcessorStroke *pPathProcessor)
{
	// Remember our processor, and let it know that we "own" it
	pProcessor = pPathProcessor;
	if (pProcessor != NULL)
		pProcessor->SetParentAttr(this);
}



/********************************************************************************************

>	StrokeTypeAttrValue::~StrokeTypeAttrValue()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	Destructor
				Deletes any attached PathProcessor (see the constructor)

********************************************************************************************/

StrokeTypeAttrValue::~StrokeTypeAttrValue()
{
	if (pProcessor != NULL)
		delete pProcessor;
}



/********************************************************************************************

>	static BOOL StrokeTypeAttrValue::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Returns:	TRUE if it initilised successfully

	Purpose:	Registers a default attribute of this type with the attribute manager

********************************************************************************************/

BOOL StrokeTypeAttrValue::Init(void)
{
	// The default attribute is one that has no effect (i.e. produces old-style "lines")
	StrokeTypeAttrValue *pAttr = new StrokeTypeAttrValue;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(StrokeTypeAttrValue),
															pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising StrokeTypeAttrValue");

	return(TRUE);
}



/********************************************************************************************

>	virtual void StrokeTypeAttrValue::Render(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pRegion - the render region to render this attribute into.

	Purpose:	Sets the StrokeTypeAttrValue attribute for the given render region.

	Notes:		This attribute makes itself current in the render region, and
				also (possibly) adds a PathProcessor to handle stroking of
				all future rendered paths (until the attr is "restored")

	SeeAlso:	StrokeTypeAttrValue::Restore

********************************************************************************************/

void StrokeTypeAttrValue::Render(RenderRegion *pRegion, BOOL Temp)
{
	// Stack the current attribute and set ourselves up as the new one
	pRegion->SetStrokeType(this, Temp);

	// Find if we have a path processor to do the stroking, and if we do, 
	// stack a copy of it (a copy must be used to be thread-safe & bgrender-safe)
	if (pProcessor != NULL)
	{
		PathProcessorStroke *pNewProcessor = pProcessor->Clone();
		if (pNewProcessor != NULL)
			pRegion->PushPathProcessor(pNewProcessor);
	}
}



/********************************************************************************************

>	virtual void StrokeTypeAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).

	Purpose:	Restores the StrokeTypeAttrValue attribute for the given render region. 

	Notes:		This attribute makes sure it removes any PathProcessor it added
				to handle path stroking in StrokeTypeAttrValue::Render

********************************************************************************************/

void StrokeTypeAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)
{
	pRegion->RestoreStrokeType(this, Temp);
}



/********************************************************************************************

>	void StrokeTypeAttrValue::GoingOutOfScope(RenderRegion *pRegion)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/97

	Inputs:		pRegion - the render region the attribute is in use by

	Purpose:	A sister function to Render()

				This is called by a render region when an attribute goes out of
				scope, i.e. when the attribute is no longer in use and is popped
				off the stack for the last time. (Do NOT confuse this with being
				pushed onto the render stack when overridden by another attr)

				It gives the attribute a chance to remove any PathProcessor(s)
				it added to the RenderRegion when it was Render()ed.

	Notes:		This attribute makes sure it removes any PathProcessor it added
				to handle path stroking in StrokeTypeAttrValue::Render

	SeeAlso:	StrokeTypeAttrValue::Render()

********************************************************************************************/

void StrokeTypeAttrValue::GoingOutOfScope(RenderRegion *pRegion)
{
	if (pProcessor != NULL)
		pRegion->PopPathProcessor();
}



/********************************************************************************************

>	virtual void StrokeTypeAttrValue::SimpleCopy(AttributeValue *pValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pValue - pointer to the AttributeValue to copy

	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void StrokeTypeAttrValue::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, StrokeTypeAttrValue),
				"Invalid Attribute value passed to StrokeTypeAttrValue::SimpleCopy");

	// Just uses the assignment operator
	*this = *((StrokeTypeAttrValue *) pValue);
}



/********************************************************************************************

>	virtual NodeAttribute *StrokeTypeAttrValue::MakeNode()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Returns:	Pointer to the new node, or NULL if out of memory.

	Purpose:	Make a new attribute node for this type of attr value - see base class

	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *StrokeTypeAttrValue::MakeNode()
{
	// Create new attribute node
	AttrStrokeType *pAttr = new AttrStrokeType;
	if (pAttr == NULL)
		return NULL;

	// Copy attribute value (if any) into the new node.
	if (pAttr->GetAttributeValue() != NULL)
		pAttr->GetAttributeValue()->SimpleCopy(this);

	return(pAttr);
}



/********************************************************************************************

>	virtual BOOL StrokeTypeAttrValue::IsDifferent(AttributeValue *pAttr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	Determines if this AttrValue is different from the given one

	Errors:		ERROR3 if the two attributes are not of the same type

	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL StrokeTypeAttrValue::IsDifferent(AttributeValue *pAttr)
{
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(StrokeTypeAttrValue)),
		   		"Different attribute types in StrokeTypeAttrValue::IsDifferent()");

	// Check they are NOT the same using the == operator
	return ( !(*((StrokeTypeAttrValue *)pAttr) == *this) );
}



/********************************************************************************************

>	virtual StrokeTypeAttrValue &StrokeTypeAttrValue::operator=(StrokeTypeAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		Attrib - the attribute to copy

	Purpose:	Assignment operator

********************************************************************************************/

StrokeTypeAttrValue &StrokeTypeAttrValue::operator=(StrokeTypeAttrValue &Attrib)
{
	// Get rid of our old processor (if any)
	if (pProcessor != NULL)
		delete pProcessor;
	pProcessor = NULL;

	// Copy the other attr's processor. If this fails, we'll get back a NULL pointer,
	// and will simply "convert" into a simple no-stroke attribute.
	if (Attrib.pProcessor != NULL)
	{
		pProcessor = Attrib.pProcessor->Clone();
		if (pProcessor != NULL)
			pProcessor->SetParentAttr(this);
	}

	return(*this);
}



/********************************************************************************************

>	virtual INT32 StrokeTypeAttrValue::operator==(const StrokeTypeAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		Attrib - the attribute to compare this attribute with

	Returns:	TRUE if the attributes are considered equal

	Purpose:	Comparison operator

********************************************************************************************/

INT32 StrokeTypeAttrValue::operator==(const StrokeTypeAttrValue &Attrib)
{
	ERROR3IF(!Attrib.IsKindOf(CC_RUNTIME_CLASS(StrokeTypeAttrValue)),
				"Other attribute value isn't an StrokeTypeAttrValue");

	StrokeTypeAttrValue *Other = (StrokeTypeAttrValue *) &Attrib;

	// Equal if they both have same processor (only applies if they are both NULL)
	if (pProcessor == Other->pProcessor)
		return(TRUE);

	// Otherwise, if one of them is NULL, they can't be the same
	if (pProcessor == NULL || Other->pProcessor == NULL)
		return(FALSE);

	// Finally, ask the processors if they are of the same type
	return(!pProcessor->IsDifferent(Other->pProcessor));
}



/********************************************************************************************

>	void StrokeTypeAttrValue::SetPathProcessor(PathProcessorStroke *pNewProcessor)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pNewProcessor - The new PathProcessorStroke to be used by this attr.
				May be NULL, in which case it sets "old style lines" stroking mode.

	Purpose:	To set the path processor used by this object. The processor is now
				"owned" by this attribute, and will be auto-deleted upon destruction

********************************************************************************************/

void StrokeTypeAttrValue::SetPathProcessor(PathProcessorStroke *pNewProcessor)
{
	if (pProcessor != NULL)
		delete pProcessor;

	pProcessor = pNewProcessor;
	if (pProcessor != NULL)
		pProcessor->SetParentAttr(this);
}


/********************************************************************************************

>	BOOL StrokeTypeAttrValue::DoBecomeA(BecomeA* pBecomeA, Node* pParent)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/2000
	Inputs:		pBecomeA - the object that tells us what to become, and recieves the results
				pParent - the node that this attribute is applied to
	Outputs:	
	Returns:	TRUE if everything went ok,
	Purpose:	To convert our stroke into something more palatable.  pParent
				should always be a nodepath 
	
	Notes:		Due to problems with bevelling and contouring this function does not get
				called in the normal DoBecomeA procedure unless you specifically locate this
				attribute and call this function directly.
********************************************************************************************/

BOOL StrokeTypeAttrValue::DoBecomeA(BecomeA* pBecomeA, Node* pParent)
{
	ERROR2IF(pBecomeA == NULL, FALSE, "BecomeA pointer is NULL in VariableWidthAttrValue::DoBecomeA");
	ERROR2IF(pParent == NULL, FALSE, "Parent node is NULL in VariableWidthAttrValue::DoBecomeA");

	if (pProcessor == NULL)
		return FALSE;

	if (!pBecomeA->BAPath())
		return FALSE;

	BOOL Success = FALSE;
//	UINT32 Dummy = 0;
	// if we have a nodepath then we can simply use its member path, otherwise we
	// have to ask it to become a nodepath
	if (pParent->IsNodePath())
	{	
		// we have a special calculation for nodepaths, because we can use their path
		Success =  pProcessor->DoBecomeA(pBecomeA, &((NodePath*)pParent)->InkPath, pParent);
	}
	else if (pParent->CanBecomeA(pBecomeA))
	{
		// we must be applied to some arbitrary shape.  The thing is that we need to have
		// a path to pass to the PPB, so want to get a passback of all the paths in 
		// the node	
		
		// we need to allocate a path
		Path* pPath = new Path;
		if (pPath != NULL && pPath->Initialise())
		{
			PathBecomeA BecomeAPath(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), NULL, FALSE, pPath);
			if (pParent->DoBecomeA(&BecomeAPath))
			{
				Success =  pProcessor->DoBecomeA(pBecomeA, pPath, pParent);
			}
			delete pPath;
		}
	}
	return Success;
}










/********************************************************************************************

>	AttrVariableWidth::AttrVariableWidth()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    7/1/97

	Purpose:	Default constructor for AttrVariableWidth

********************************************************************************************/

AttrVariableWidth::AttrVariableWidth()
{
}



/********************************************************************************************

>	AttrVariableWidth::AttrVariableWidth(Node *ContextNode,
										AttachNodeDirection Direction,
										BOOL Locked,
										BOOL Mangled,
										BOOL Marked,
										BOOL Selected)
    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    7/1/97

	Purpose:	Constructs an AttrVariableWidth Attribute

********************************************************************************************/

AttrVariableWidth::AttrVariableWidth(Node *ContextNode,
									AttachNodeDirection Direction,
									BOOL Locked,
									BOOL Mangled,
									BOOL Marked,
									BOOL Selected)
				: NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
}



/********************************************************************************************

>	virtual void AttrVariableWidth::Render(RenderRegion *pRender)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	Renders this attribute (by simply calling the Render function of
				its contained AttributeValue)

********************************************************************************************/

void AttrVariableWidth::Render(RenderRegion *pRender)
{
	GetAttributeValue()->Render(pRender);
}



/***********************************************************************************************

>	virtual void AttrVariableWidth::CopyNodeContents(AttrVariableWidth *NodeCopy)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    7/1/97

    Outputs:    NodeCopy - returned containing a copy of this node
		 
    Purpose:	Copies the node's contents to the node pointed to by NodeCopy

***********************************************************************************************/

void AttrVariableWidth::CopyNodeContents(AttrVariableWidth *NodeCopy)
{
	// Let the base class do its bit
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And then copy our Value
	*(NodeCopy->GetAttributeValue()) = *(GetAttributeValue());
}



/***********************************************************************************************
>   void AttrVariableWidth::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrVariableWidth::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrVariableWidth), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrVariableWidth))
		CopyNodeContents((AttrVariableWidth*)pNodeCopy);
}



/***********************************************************************************************

>	virtual INT32 AttrVariableWidth::operator==(const NodeAttribute &NodeAttrib); 

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/7/97

	Inputs:		NodeAttrib - The node to compare this node to
	Returns:	TRUE if the nodes are considered equal

    Purpose:	Comparison operator - determines if the AttributeValues	of both objects are ==

***********************************************************************************************/

INT32 AttrVariableWidth::operator==(const NodeAttribute &NodeAttrib)
{
	// First check they are of the same type
	if (((NodeAttribute*)&NodeAttrib)->GetAttributeType() != GetAttributeType())
		return FALSE;

	// Make a more sensible pointer
	AttrVariableWidth *Attr = (AttrVariableWidth *) &NodeAttrib;

	// Now let the AttributeValues compare themselves
	return( *((VariableWidthAttrValue *) Attr->GetAttributeValue())  ==
			*((VariableWidthAttrValue *) GetAttributeValue()) );
}



/********************************************************************************************

>	virtual Node *AttrVariableWidth::SimpleCopy()

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/1/97

    Returns:    A copy of the node, or NULL if memory runs out 

    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  

********************************************************************************************/

Node *AttrVariableWidth::SimpleCopy()
{
	AttrVariableWidth* NodeCopy = new AttrVariableWidth;
	if (NodeCopy == NULL)
		return(NULL);

	// Call the base class
	NodeAttribute::CopyNodeContents(NodeCopy);

	// And call our AttributeValue to copy itself too
	NodeCopy->GetAttributeValue()->SimpleCopy(GetAttributeValue());
    
	return(NodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrVariableWidth::GetAttrNameID(void)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/1/97

	Returns:	Attribute description string-resource ID

	Purpose:	Retrieves a string resource ID describing this attribute

********************************************************************************************/

UINT32 AttrVariableWidth::GetAttrNameID(void)
{
	return(_R(IDS_ATTRVARWIDTH));
}



/********************************************************************************************

>	virtual void AttrVariableWidth::GetDebugDetails(StringBase *Str)

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/1/97

	Outputs:	On return, Str is filled in with details on this node

	Purpose:	Produces debug details about this node

********************************************************************************************/

void AttrVariableWidth::GetDebugDetails(StringBase *Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails(Str);

	String_256 TempStr;
	TempStr._MakeMsg( _T("\r\nVariable width of type:\r\n") );
	*Str += TempStr;

	if (Value.GetWidthFunction() != NULL)
	{
		TempStr._MakeMsg( _T("  #1%s\r\n"),
							Value.GetWidthFunction()->GetRuntimeClass()->GetClassName() );
	}
	else
		TempStr._MakeMsg( _T("  old-style constant width\r\n") );

	*Str += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrVariableWidth::GetNodeSize() const

    Author:     Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Returns:	The size of this node, in bytes

	Purpose:	For finding the size of the node, in bytes

********************************************************************************************/

UINT32 AttrVariableWidth::GetNodeSize() const
{
	return(sizeof(AttrVariableWidth));
}



/********************************************************************************************

>	BOOL AttrVariableWidth::HasActiveValueFunction() const

    Author:     Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/11/2000

	Returns:	True if our attr value has a value function, false if not

	Purpose:	To find out if this attribute is actually going to do anything or not

********************************************************************************************/

BOOL AttrVariableWidth::HasActiveValueFunction() 
{
	return (Value.GetWidthFunction() != NULL);
}

/********************************************************************************************

>	virtual BOOL AttrVariableWidth::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
>	virtual BOOL AttrVariableWidth::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Inputs:		pFilter - filter to write to

	Returns:	TRUE if the Node has written out a record to the filter

	Purpose:	Writes out a record that represents the node, to either Native or Web
				file format.

				This function is called before any of the AttrVariableWidth's children
				are written to the filter.

				If the AttrVariableWidth writes out a record successfully to the file,
				it will return TRUE.

				If the AttrVariableWidth chooses not to write itself to the filter
				(e.g. because it is not appropriate	for this filter), then this
				function will return FALSE.

	Notes:		Simple 

	SeeAlso:	Node::WritePreChildrenNative; Node::WritePreChildrenWeb;
				StrokeAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL AttrVariableWidth::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
	return(WritePreChildrenNative(pFilter));
}

BOOL AttrVariableWidth::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	ERROR3IF(pFilter == NULL, "Illegal NULL param");

	BOOL ok = TRUE;
	VariableWidthID PredefinedFunctionID = Value.GetWidthFunctionID();

	if (PredefinedFunctionID == VarWidth_NotPredefined)
	{
		ValueFunction *pFunction = Value.GetWidthFunction();
		if (pFunction != NULL)
		{
			CamelotFileRecord *pRec = pFunction->WriteFileRecord(TAG_VARIABLEWIDTHTABLE, 0, pFilter);
			if (pRec != NULL)
			{
				pFilter->Write(pRec);
				delete pRec;
				pRec = NULL;
			}
			else
				ok = FALSE;
		}
		else
		{
			// The default attribute (for simple old-style lines) has no ValueFunction
			CamelotFileRecord Rec(pFilter, TAG_VARIABLEWIDTHFUNC, TAG_VARIABLEWIDTHFUNC_SIZE);
			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteUINT32((UINT32) 0);
			if (ok) ok = pFilter->Write(&Rec);
		}
	}
	else
	{
		CamelotFileRecord Rec(pFilter, TAG_VARIABLEWIDTHFUNC, TAG_VARIABLEWIDTHFUNC_SIZE);

		if (ok) ok = Rec.Init();
		if (ok) ok = Rec.WriteUINT32((UINT32) PredefinedFunctionID);
		if (ok) ok = pFilter->Write(&Rec);
	}

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return(ok);
#else
	return FALSE;
#endif
}















/********************************************************************************************

>	VariableWidthAttrValue::VariableWidthAttrValue(ValueFunction *pValueFunction = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pValueFunction - NULL (for simple constant-width "line") or a ValueFunction
								 describing the variable width of paths affected by this attr.

								 NOTE that this object now BELONGS to the new attr, and
								 will be deleted automatically when this object is destructed

	Purpose:	Constuctor for VariableWidthAttrValue

********************************************************************************************/

VariableWidthAttrValue::VariableWidthAttrValue(ValueFunction *pValueFunction)
{
	WidthFunction = pValueFunction;
	PredefinedFunctionID = VarWidth_NotPredefined;
}



/********************************************************************************************

>	VariableWidthAttrValue::~VariableWidthAttrValue()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	Destructor for VariableWidthAttrValue

********************************************************************************************/

VariableWidthAttrValue::~VariableWidthAttrValue()
{
	if (WidthFunction != NULL)
		delete WidthFunction;
}



/********************************************************************************************

>	static BOOL VariableWidthAttrValue::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Returns:	TRUE if it initilised successfully

	Purpose:	Registers a default attribute of this type with the attribute manager

********************************************************************************************/

BOOL VariableWidthAttrValue::Init(void)
{
	// The default attribute is one that has no effect (i.e. produces constant-width "lines")
	VariableWidthAttrValue *pAttr = new VariableWidthAttrValue;
	if (pAttr == NULL)
		return FALSE;

	UINT32 ID = AttributeManager::RegisterDefaultAttribute(CC_RUNTIME_CLASS(VariableWidthAttrValue),
															pAttr);

	ERROR2IF(ID == ATTR_BAD_ID, FALSE, "Bad ID when Initialising VariableWidthAttrValue");

	return(TRUE);
}



/********************************************************************************************

>	virtual void VariableWidthAttrValue::Render(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pRegion - the render region to render this attribute into.

	Purpose:	Sets the VariableWidthAttrValue attribute for the given render region.

	Notes:		This attribute is so simple that its state can be read directly off
				the render region's stack. Thus, there is minimal special render region
				support for this attribute - we just stack and unstack it directly here.

********************************************************************************************/

void VariableWidthAttrValue::Render(RenderRegion *pRegion, BOOL Temp)
{
	pRegion->SetVariableWidth(this, Temp);
}



/********************************************************************************************

>	virtual void VariableWidthAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pRegion - the render region to restore the attribute into.
				Temp    - TRUE if this is a temporary attribute, FALSE if it is
						  permanent (e.g. it's in a document tree).

	Purpose:	Restores the VariableWidthAttrValue attribute for the given render region. 

	Notes:		This attribute is so simple that its state can be read directly off
				the render region's stack. Thus, there is minimal special render region
				support for this attribute - we just stack and unstack it directly here.

********************************************************************************************/

void VariableWidthAttrValue::Restore(RenderRegion *pRegion, BOOL Temp)
{
	pRegion->RestoreVariableWidth(this, Temp);
}



/********************************************************************************************

>	virtual void VariableWidthAttrValue::SimpleCopy(AttributeValue *pValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pValue - pointer to the AttributeValue to copy

	Purpose:	See AttributeValue::SimpleCopy

********************************************************************************************/

void VariableWidthAttrValue::SimpleCopy(AttributeValue *pValue)
{
	ERROR3IF(!IS_A(pValue, VariableWidthAttrValue),
				"Invalid Attribute value passed to VariableWidthAttrValue::SimpleCopy");

	// Just uses the assignment operator
	*this = *((VariableWidthAttrValue *) pValue);
}



/********************************************************************************************

>	virtual NodeAttribute *VariableWidthAttrValue::MakeNode()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Returns:	Pointer to the new node, or NULL if out of memory.

	Purpose:	Make a new attribute node for this type of attr value - see base class

	SeeAlso:	AttributeValue::MakeNode

********************************************************************************************/

NodeAttribute *VariableWidthAttrValue::MakeNode()
{
	// Create new attribute node
	AttrVariableWidth *pAttr = new AttrVariableWidth();
	if (pAttr == NULL)
		return NULL;

	// Copy attribute value (if any) into the new node.
	if (pAttr->GetAttributeValue() != NULL)
		pAttr->GetAttributeValue()->SimpleCopy(this);

	return(pAttr);
}



/********************************************************************************************

>	virtual BOOL VariableWidthAttrValue::IsDifferent(AttributeValue *pAttr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	Determines if this AttrValue is different from the given one

	Errors:		ERROR3 if the two attributes are not of the same type

	SeeAlso:	AttributeValue::IsDifferent

********************************************************************************************/

BOOL VariableWidthAttrValue::IsDifferent(AttributeValue *pAttr)
{
	ERROR3IF(!pAttr->IsKindOf(CC_RUNTIME_CLASS(VariableWidthAttrValue)),
		   		"Different attribute types in VariableWidthAttrValue::IsDifferent()");

	// Check they are NOT the same using the == operator
	return ( !(*((VariableWidthAttrValue *)pAttr) == *this) );
}



/********************************************************************************************

>	virtual VariableWidthAttrValue &VariableWidthAttrValue::operator=(VariableWidthAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		Attrib - the attribute to copy

	Purpose:	Assignment operator

********************************************************************************************/

VariableWidthAttrValue &VariableWidthAttrValue::operator=(VariableWidthAttrValue &Attrib)
{
	// Delete our existing WidthFunction (if any)
	if (WidthFunction != NULL)
		delete WidthFunction;
	WidthFunction = NULL;

	// Try to clone the other attribute's width function. If this fails, we'll just
	// end up as a constant-width line (NULL ValueFunction pointer)
	if (Attrib.GetWidthFunction() != NULL)
		WidthFunction = Attrib.GetWidthFunction()->Clone();

	// And copy its ID member across too
	PredefinedFunctionID = Attrib.PredefinedFunctionID;

	return(*this);
}



/********************************************************************************************

>	virtual INT32 VariableWidthAttrValue::operator==(const VariableWidthAttrValue &Attrib)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		Attrib - the attribute to compare this attribute with

	Returns:	TRUE if the attributes are considered equal

	Purpose:	Comparison operator

********************************************************************************************/

INT32 VariableWidthAttrValue::operator==(const VariableWidthAttrValue &Attrib)
{
	ERROR3IF(!Attrib.IsKindOf(CC_RUNTIME_CLASS(VariableWidthAttrValue)),
				"Other attribute value isn't an VariableWidthAttrValue");

//	VariableWidthAttrValue *Other = (VariableWidthAttrValue *) &Attrib;

	// If both width functions are NULL, we are equal
	if (WidthFunction == NULL && Attrib.WidthFunction == NULL)
		return(TRUE);

	// If only one function is NULL, we ca't be considered equal
	if (WidthFunction == NULL || Attrib.WidthFunction == NULL)
		return(FALSE);

	// Finally, if both have a valid width function, ask them if they're equal
	return(!WidthFunction->IsDifferent(Attrib.WidthFunction));
}



/********************************************************************************************

>	void VariableWidthAttrValue::SetWidthFunction(ValueFunction *pNewFunction)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pNewFunction - The new value function to use for variable-width strokes
				affected by this attribute. May be NULL, in which case all strokes will
				be simple constant-width strokes.

				NOTE that this object now belongs to this Attr, and will be automatically
				deleted upon destruction of the attr.

	Purpose:	To set the width function used in stroking

	Notes:		See the alternative form of this method - it uses predefined function
				"shapes", which save in a more compact format.

********************************************************************************************/

void VariableWidthAttrValue::SetWidthFunction(ValueFunction *pNewFunction)
{
	if (WidthFunction != NULL)
		delete WidthFunction;

	WidthFunction = pNewFunction;
	PredefinedFunctionID = VarWidth_NotPredefined;
}



/********************************************************************************************

>	void VariableWidthAttrValue::SetWidthFunction(VariableWidthID PredefinedFuncID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		PredefinedFuncID - The ID of which predefined width function type you
				wish to use. Predefined types are used to save space in the file format
				when exporting simple variable width attributes.

	Purpose:	To set the width function used in stroking

********************************************************************************************/

void VariableWidthAttrValue::SetWidthFunction(VariableWidthID PredefinedFuncID)
{
	ERROR3IF(PredefinedFuncID == VarWidth_NotPredefined, "You what?!");

	// get rid of any old width function
	if (WidthFunction != NULL)
	{
		delete WidthFunction;
		WidthFunction = NULL;
	}

	// Remember the new predefined-width-function identifier, so that we
	// know that our width is a spaecial predefined form which can be saved
	// in a much simpler & smaller format
	PredefinedFunctionID = PredefinedFuncID;

	switch(PredefinedFunctionID)
	{
		case VarWidth_Constant:
			WidthFunction = new ValueFunctionConstant(1.0);
			break;

		case VarWidth_LinRamp:
			WidthFunction = new ValueFunctionRampLinear(1.0, 0.0);
			break;

		case VarWidth_SRamp:
			WidthFunction = new ValueFunctionRampS(1.0, 0.0);
			break;

		default:
			ERROR3("Unsupported predefined width function");
			break;
	}
}









/********************************************************************************************

>	virtual UINT32 *StrokeAttrRecordHandler::GetTagList()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Returns:	A list of tag values, terminated by CXFRH_TAG_LIST_END

	Purpose:	Provides the record handler system with a list of records handled by this
				handler - all StrokeType attributes:
					Stroke (by type)
					Stroke (by vector subtree definition)
					Stroke (airbrush)
					Variable width function (predefined)
					Variable width function (recorded)

	SeeAlso:	StrokeTypeAttrRecordHandler::HandleRecord

********************************************************************************************/

UINT32 *StrokeAttrRecordHandler::GetTagList()
{
	static UINT32 TagList[] =
	{
		TAG_STROKEDEFINITION,
		TAG_STROKETYPE,
		TAG_STROKEAIRBRUSH,

		TAG_VARIABLEWIDTHFUNC,
		TAG_VARIABLEWIDTHTABLE,

		CXFRH_TAG_LIST_END
	};

	return(TagList);
}



/********************************************************************************************

>	virtual BOOL StrokeAttrRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)

 	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Inputs:		pCXaraFileRecord - The record to handle - may not be NULL
	Returns:	TRUE if handled successfuly

	Purpose:	Handles loading of the given StrokeType attribute record

	SeeAlso:	AttrStrokeType::WritePreChildrenNative;
				AttrOverprintFill::WritePreChildrenNative;
				AttrPrintOnAllPlates::WritePreChildrenNative

********************************************************************************************/

BOOL StrokeAttrRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR3IF(pCXaraFileRecord == NULL, "pCXaraFileRecord is NULL");

	NodeAttribute *pNewNode = NULL;
	
	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_STROKEDEFINITION:
			return(StrokeComponent::StartImportStroke(this, pCXaraFileRecord));


		case TAG_STROKETYPE:
			{
				UINT32 Handle = 0x01000000;
				pCXaraFileRecord->ReadUINT32(&Handle);

				pNewNode = new AttrStrokeType;
				if (pNewNode != NULL)
				{
					PathProcessorStroke *pProcessor = NULL;

					switch ((Handle >> 24) & 0xff)
					{
						case 0:
							// Complex vector stroke
							{
								StrokeHandle NewHandle = StrokeComponent::FindImportedStroke(Handle & 0x00ffffff);
								if (NewHandle != StrokeHandle_NoStroke)
									pProcessor = new PathProcessorStrokeVector;

								if (pProcessor != NULL)
									((PathProcessorStrokeVector *)pProcessor)->SetStrokeDefinition(NewHandle);
							}
							break;

						case 1:
							// Simple old-style constant width line. Use pProcessor == NULL
							break;

						case 2:
							// Simple variable-width stroke
							pProcessor = new PathProcessorStroke;
							break;

						default:
							ERROR3("Unknown/Unsupported stroke type");
							break;
					}

					if (pProcessor != NULL)
						((StrokeTypeAttrValue *)pNewNode->GetAttributeValue())->SetPathProcessor(pProcessor);
				}
			}
			break;


		case TAG_STROKEAIRBRUSH:
			{
				// Airbrushes currently only hold an intensity-function, so we simply read that
				// function and build an airbrush-stroke attribute around it.
				ValueFunction *pValFunc = ValueFunction::ReadFileRecord(pCXaraFileRecord);
				if (pValFunc != NULL)
				{
					pNewNode = new AttrStrokeType;
					if (pNewNode != NULL)
					{
						PathProcessorStrokeAirbrush *pProcessor = new PathProcessorStrokeAirbrush;
						if (pProcessor != NULL)
						{
							pProcessor->SetIntensityFunction(pValFunc);
							((StrokeTypeAttrValue *)pNewNode->GetAttributeValue())->SetPathProcessor(pProcessor);
						}
						else
						{
							delete pNewNode;
							pNewNode = NULL;
						}
					}
				}

				if (pNewNode == NULL)	// We failed, so clean up
					delete pValFunc;
			}
			break;


		case TAG_VARIABLEWIDTHFUNC:
			// Create a new variable-width attribute
			pNewNode = new AttrVariableWidth;
			if (pNewNode != NULL)
			{
				// and fill in the value with the appropriate function definition
				VariableWidthID Function = VarWidth_NotPredefined;
				if (pCXaraFileRecord->ReadUINT32((UINT32 *)&Function))
				{
					if ((UINT32)Function != 0)
						((VariableWidthAttrValue *)pNewNode->GetAttributeValue())->SetWidthFunction(Function);
				}
			}
			break;


		case TAG_VARIABLEWIDTHTABLE:
			{
				ValueFunction *pValFunc = ValueFunction::ReadFileRecord(pCXaraFileRecord);
				if (pValFunc != NULL)
				{
					pNewNode = new AttrVariableWidth;
					if (pNewNode != NULL)
						((VariableWidthAttrValue *)pNewNode->GetAttributeValue())->SetWidthFunction(pValFunc);
					else
						delete pValFunc;
				}
			}
			break;

		default:
			ERROR3_PF(("I don't handle records with the tag (%d)\n", pCXaraFileRecord->GetTag()));
			break;
	}

	ERROR3IF(pNewNode == NULL, "Failed to load stroke information");

	// Get the base class to insert the new node for us
	if (pNewNode != NULL)
		InsertNode(pNewNode);
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL StrokeAttrRecordHandler::BeginSubtree(UINT32 Tag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97

	Inputs:		Tag = tag value of the tag this handler last handled
	Returns:	TRUE if this func wants exclusive handling of the tag
				FALSE otherwise
	Purpose:	Informs the record handler that a subtree is following a tag of type 'Tag'

				If you override this func and you do not wish other parts of the system to be informed of the
				subtree start, you should return TRUE

********************************************************************************************/

BOOL StrokeAttrRecordHandler::BeginSubtree(UINT32 Tag)
{
	// We only want to know about following subtrees when doing stroke definition records.
	// If we are doing one, then we grab the subtree so nobody else can faff about with it
	if (Tag != TAG_STROKEDEFINITION)
		return(FALSE);

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL StrokeAttrRecordHandler::EndSubtree(UINT32 Tag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97

	Inputs:		Tag = tag value of the tag this handler last handled
	Returns:	TRUE if this func wants exclusive handling of the tag
				FALSE otherwise

	Purpose:	Informs the record handler that a subtree that followed a tag of type 'Tag' has ended

				If you override this func and you do not wish other parts of the system to be
				informed of the subtree end, you should return TRUE

********************************************************************************************/

BOOL StrokeAttrRecordHandler::EndSubtree(UINT32 Tag)
{
	// We only want to know about following subtrees when doing custom print mark records.
	// If we are doing one, then we grab the subtree so nobody else can faff about with it
	if (Tag != TAG_STROKEDEFINITION)
		return(FALSE);

#if !defined(EXCLUDE_FROM_RALPH)
	// OK, it is the end of a stroke definition record, so complete the import
	StrokeComponent::EndImportStroke(this);
#endif
	
	return(TRUE);
}



/********************************************************************************************

>	virtual void StrokeAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/97

	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update

	Returns:	-
	Purpose:	Provides descriptions for the stroke attribute records.

	Notes:		This function is only present in _DEBUG builds

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void StrokeAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first to output the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord, pStr);

	UINT32 Tag = pRecord->GetTag();
//	INT32 RecordNumber = pRecord->GetRecordNumber();

	switch (Tag)
	{
		case TAG_STROKETYPE:
			*pStr += String_64(_T("New-style stroke (basic type)"));	// Only 1 stroke type so far!
			break;

		case TAG_STROKEDEFINITION:
			*pStr += String_64(_T("New-style stroke (Vector type)"));
			break;

		case TAG_STROKEAIRBRUSH:
			{
				// Load the value function and see what type of object we get back!
				ValueFunction *pValFunc = ValueFunction::ReadFileRecord(pRecord);
				if (pValFunc != NULL)
				{
					String_256 Desc;
					Desc._MakeMsg(_T("Airbrush stroke of class #1%s"), 
									(TCHAR *)pValFunc->GetRuntimeClass()->m_lpszClassName);
					*pStr += Desc;
					delete pValFunc;
				}
				else
					*pStr += String_64(_T("Unknown Airbrush stroke type (failed to load)"));
			}
			break;
			break;

		case TAG_VARIABLEWIDTHFUNC:
			{
				VariableWidthID Function = VarWidth_NotPredefined;
				pRecord->ReadUINT32((UINT32 *)&Function);
				switch(Function)
				{
					case VarWidth_Constant:
						*pStr += String_64(_T("Constant predefined stroke width"));
						break;

					case VarWidth_LinRamp:
						*pStr += String_64(_T("Linear-Ramp predefined stroke width"));
						break;

					case VarWidth_SRamp:
						*pStr += String_64(_T("S-Ramp predefined stroke width"));
						break;

					default:
						*pStr += String_64(_T("Unknown predefined stroke width type"));
						break;
				}
			}
			break;

		case TAG_VARIABLEWIDTHTABLE:
			{
				// Load the value function and see what type of object we get back!
				ValueFunction *pValFunc = ValueFunction::ReadFileRecord(pRecord);
				if (pValFunc != NULL)
				{
					String_256 Desc;
					Desc._MakeMsg(_T("VariableWidth of class #1%s"), 
									(TCHAR *)pValFunc->GetRuntimeClass()->m_lpszClassName);
					*pStr += Desc;
					delete pValFunc;
				}
				else
					*pStr += String_64(_T("Unknown VariableWidth ValueFunction (failed to load)"));
			}
			break;

		default:
			ERROR3(_T("Unknown tag passed to StrokeAttrRecordHandler"));
			break;
	}
}

#endif		// _DEBUG
