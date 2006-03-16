// $Id: rndstack.cpp 662 2006-03-14 21:31:49Z alex $
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

// A attribute stack to be used when rendering Camelot files.

/*
*/

#include "camtypes.h"

#include "fixmem.h"
#include "attrval.h"
#include "errors.h"

#include "rndrgn.h"
#include "rndstack.h"

/********************************************************************************************

Technical notes:

Important note:

This class implements a rendering attribute context stack.  It is done using a simple
array structure, which grows when required.  The stack is implemented using two classes;
RenderStack and AttributeRec.  The stack array is held by the RenderStack object, and the
array is an array of AttributeRec objects.

The stack is optimised to avoid saving attributes when it is unnecessary. The way this
works is that a 'context level' is maintained by the RenderStack object.  Whenever an
attribute is changed, then the render region that owns this stack pushes the current
attribute onto the stack, along with the current context level. Then when the render region
wants to save the context, all the stack object does is increment the context level. Hence
all future attributes pushed onto the context stack will have a different context level.
When the context is to be restored, the stack pops off all the stack records whose context 
level fields match the current context level, and then decrements the context level.

For, example, given the following sequence of calls:

	pRegion->SetLineColour(COLOUR_BLACK);
	pRegion->SetLineWidth(250);
	pRegion->SaveContext();
	pRegion->SetLineColour(COLOUR_RED);
	pRegion->SaveContext();
	pRegion->SetLineColour(COLOUR_GREEN);
	pRegion->SetLineWidth(0);
	pRegion->SetFillColour(COLOUR_CYAN);

then we would end up with a stack like this:


  	 AttributeValue     Context Level 

  +-------------------+---------------+
  |	                  |               |    <--- Next free element ('Top')
  +-------------------+---------------+
  |	FillColour: CYAN  |       2       |    <--- Last element
  +-------------------+---------------+
  | LineWidth:  0	  |	      2		  |
  +-------------------+---------------+
  | LineColour: GREEN |	      2		  |
  +-------------------+---------------+
  | LineColour: RED	  |	      1		  |
  +-------------------+---------------+
  | LineWidth:  250	  |       0		  |
  +-------------------+---------------+
  | LineColour: BLACK |       0       |    <--- First element
  +-------------------+---------------+


and the current context level would be 2.  Therefore when RestoreContext() is next called, 
the stack pops off all elements with the context level 2, i.e. the top three elements, and
decrements the context value to 1.  Obviously the next time RestoreContext() is called, 
then it just pops off one element (red line colour) and decrements the context level to 0.
And this is basically how the stack works - this way, we don't have to save/restore every
attribute whenever we save/restore the context, and so it is much more efficient.

There is one more complication, because the attribute value itself is not pushed onto the
stack, but a pointer to it.  The advantage of this is that less data has to be copied 
when setting and restoring attributes, and we can encapsulate the setting and restoring
of the attribute within the attribute class itself (see kernel\attr.{h,cpp}).  When an
attribute is rendered, the render region asks it to render itself via its virtual
function Render() - this will save the current attribute and install itself as the
new current attribute via the RenderRegion::SvaeAttribute() function.  Similarly, when
the attribute record is popped off the stack, the virtual function Restore() is called.
This will set the current attribute to be itself, via the RenderRegion::RestoreAttribute()
function.
At the moment (4/2/94), all our attributes our contained within the RenderRegion, so this
system is not strictly needed, but if we want tools to be able to add types of attributes,
then we must have an extensible system like this.  At present, the current attribute array
is of a fixed size, and when we start adding attributes via modules/tools, we will need
to have a system of declaring/registering attributes when the modules/tools start up.
They will then be given an 'ID' for their attribute.  This will in fact be an index into
the CurrentAttrs array maintained by RenderRegion, which will be dynamically allocated
when the render region is initialised, according to how many attributes have been
declared/registered.


********************************************************************************************/

// InitialStackSize - how many entries can fit in the default stack size.
// Various tests show that the stack very rarely gets deeper than 40 levels, so 30
// is more than enough.
// (Jason here, several years later: We now have more than 30 default attributes
// so the stack always has more than 30 items, and usually sits between 40 and 80
// entries, so is now guaranteed to realloc for every render! As the records are
// only 12 bytes each, I've changed this from 30+20 to 80+30 so that the
// reallocing should happen infrequently (if ever) and we can double the number
// of default attributes before we get into trouble again! However, I've
// kept it relatively small to reduce the overhead of initialising unused entries)
const INT32 InitialStackSize = 80;	// Was 30

// StackGranularity - this dictates how many entries should be added to the stack
// when we run out of space.
const INT32 StackGranularity = 30;	// Was 20

// ITEM_SIZE - this value is used when allocating memory for the stack.
#define ITEM_SIZE (sizeof(AttributeRec))

/********************************************************************************************

>	class AttributeRec : public CC_CLASS_MEMDUMP

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Purpose:	Encapsulates an item in the RenderStack class.
	SeeAlso:	RenderStack

********************************************************************************************/

class AttributeRec : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(AttributeRec)

public:
	AttributeRec();
	AttributeValue *pAttrValue;
	UINT32 ContextLevel;
	BOOL Temporary;
};

CC_IMPLEMENT_MEMDUMP(AttributeRec, CCObject)
CC_IMPLEMENT_MEMDUMP(RenderStack, CCObject)

/********************************************************************************************

>	AttributeRec::AttributeRec()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Purpose:	Initialise a stack element in the RenderStack.

********************************************************************************************/

AttributeRec::AttributeRec()
{
	pAttrValue = NULL;
	ContextLevel = 0;
	Temporary = FALSE;
}

/********************************************************************************************

>	RenderStack::RenderStack()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Purpose:	Sets up the empty rendering stack.

********************************************************************************************/

RenderStack::RenderStack()
{
	// Initialise stack variables
	ContextLevel = 0;
	StackLimit = 0;
	Top = 0;
	TheStack = NULL;
}

/********************************************************************************************

>	RenderStack::~RenderStack()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Purpose:	Frees stack memory, including any temporary objects that belong to the
				stack.

********************************************************************************************/

RenderStack::~RenderStack()
{
	// Delete all temporary items left in the stack
	for (UINT32 i = 0; i < Top; i++)
	{
		if (TheStack[i].Temporary)
		{
			// Make sure pointer is sane before we delete it.
			CC_ASSERT_VALID(TheStack[i].pAttrValue);
			delete TheStack[i].pAttrValue;
		}
	}

	// Clean up and free stack objects.
	ContextLevel = 0;
	StackLimit = 0;
	Top = 0;
	CCFree(TheStack);
	TheStack = NULL;
}

/********************************************************************************************

>	BOOL RenderStack::Copy(const RenderStack *Other, RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/94

	Inputs:		Other	- pointer to the RenderStack object to copy.
				pRegion - The render-region in which this stack lives. Needed so that
						  all attrs currently on the stack can be properly de-initialised,
						  and all new attrs being copied over can be properly rendered into
						  the destination render region. Awful things are likely to happen
						  if "this" is not equal to "&pRegion->TheStack"!

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	Copies a RenderStack object.  This cannot be done as a copy constructor as
				it allocates memory and hence can fail.
				This function handles RenderStacks which contain temporary objects, i.e.
				objects created especially to put in the stack, as opposed to objects
				from a document tree.  Such objects are duplicated in the new stack, as
				otherwise the destructors of both stacks would try to delete the same
				object twice.

	Errors:		Out of memory. Ensure failure if there is already data in the stack.

********************************************************************************************/

BOOL RenderStack::Copy(const RenderStack *Other, RenderRegion *pRegion)
{
	ERROR3IF(Other == NULL || pRegion == NULL, "Illegal NULL params");

// WEBSTER - markn 15/8/97
// This has bee taken out of Webster builds as it can cause fatal access violations with Paper render regions.
// It is new path processor code that not's totally bug-free, and as path processors are not used in Webster,
// it can be safely taken out.
#ifndef WEBSTER
	// Make sure all attributes are properly popped && deinitialised, so that any used
	// PathProcessors are correctly removed, etc
	// (Note: This doesn't deinit the DEFAULT attributes, which we hope will never use
	// PathProcessors - this should be OK, because the places where this is called from
	// (at present) always make sure all PathProcessors are properly cleaned up too)
	CleanUpBeforeDestruct(pRegion);
#endif // WEBSTER

	// Make sure we have an empty stack to work with
	if (TheStack!=NULL)
	{
		// Empty the current stack before copying the new one onto it
		// For every item in the stack, check to see if it is 'temporary'.  If it is,
		// we can't use it in our stack - we must take a copy.
		for (UINT32 i=0; i<Top; i++)
		{
			// Get rid of all the temp attrs in the stack
			if (TheStack[i].Temporary)
			{
				// Delete this tempory attr.
				CC_ASSERT_VALID(TheStack[i].pAttrValue);
				delete TheStack[i].pAttrValue;
			}
		}

		// then get rid of the stack itself
		CCFree(TheStack);
		TheStack=NULL;

		ContextLevel = 0;
		StackLimit = 0;
		Top = 0;
	}

	// Sanity check
	ENSURE(TheStack == NULL,"RenderStack::Copy called on a non-empty stack");

#if FALSE
	// Jason (7/3/97)
	// Copying the stack like this is rampant. We must render all the attributes across onto
	// the new stack, so that they are properly stacked and initialised
/*
	// Copy stack variables
	ContextLevel = Other->ContextLevel;
	StackLimit = Other->StackLimit;
	Top = Other->Top;

	// The other stack may be empty, which will often be the case when merging as it may
	// not have started rendering yet.
	if (Other->TheStack == NULL)
	{
		// Just to be on the safe side
		TheStack = NULL;

		// We're done.
		return TRUE;
	}

	// Allocate the same amount of memory for the stack
	TheStack = (AttributeRec *) CCMalloc(StackLimit * ITEM_SIZE);
	if (TheStack == NULL)
		return FALSE;

	// Copy the other render region's stack data into this new stack
	memcpy(TheStack, Other->TheStack, StackLimit * ITEM_SIZE);

	// For every item in the stack, check to see if it is 'temporary'.  If it is,
	// we can't use it in our stack - we must take a copy.
	for (UINT32 i = 0; i < Top; i++)
	{
		if (TheStack[i].Temporary)
		{
			// Get the runtime class info on this object
			CCRuntimeClass *pCCRuntimeClass = TheStack[i].pAttrValue->GetRuntimeClass();
	
			// Create another object of the same type
			AttributeValue *pNewAttr = (AttributeValue *) pCCRuntimeClass->CreateObject();

			if (pNewAttr == NULL)
			{
				// Failed to create object - quit with error, but first ensure that this
				// stack is limited to the items copied so far. Otherwise the destructor
				// will attempt to delete objects that belong to the other stack.
				Top = i;
				return FALSE;
			}

			// Object created ok - get the object to copy its contents across.
			pNewAttr->SimpleCopy(TheStack[i].pAttrValue);

			// Put it in the stack
			TheStack[i].pAttrValue = pNewAttr;
		}
	}
*/
#else
	if (Other->TheStack == NULL)
		return(TRUE);

	UINT32 LastContextLevel = 0;

	// Copy all attributes across to the new stack by rendering them into the provided render region.
	// If we aren't "pRegion->TheStack" then we've just lost our paddle, and we're being sucked up the creek...
	for (UINT32 i = 0; i < Other->Top; i++)
	{
		// Make sure we copy context level information across too - if the context level changed, we must
		// save the context level now.
		if (LastContextLevel != Other->TheStack[i].ContextLevel)
			pRegion->SaveContext();

		// Get each attribute to render into our render region
		AttributeValue *pAttrVal = Other->TheStack[i].pAttrValue;

		// If it's temporary, then replace it with a unique copy for this render region
		if (Other->TheStack[i].Temporary)
		{
			// Get the runtime class info on this object
			CCRuntimeClass *pCCRuntimeClass = Other->TheStack[i].pAttrValue->GetRuntimeClass();
	
			// Create another object of the same type
			pAttrVal = (AttributeValue *) pCCRuntimeClass->CreateObject();

			if (pAttrVal == NULL)
				return FALSE;

			// Object created ok - get the object to copy its contents across.
			pAttrVal->SimpleCopy(Other->TheStack[i].pAttrValue);
		}

		// and render the attribute into this render region
		pAttrVal->Render(pRegion, Other->TheStack[i].Temporary);
	}
	
#endif

	// It worked
	return TRUE;
}


/********************************************************************************************

>	BOOL RenderStack::GrowStack()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Returns:	TRUE if the stack size was increased, FALSE if not.
	Purpose:	Increase the size of the stack because a new element needs to be pushed onto
				it, and there isn't enough room.
	Errors:		Out of memory.

********************************************************************************************/

BOOL RenderStack::GrowStack()
{
	INT32 FirstNewItem = 0;

	if (TheStack == NULL)
	{
		// Try to allocate initial memory block for the stack.
		TheStack = (AttributeRec *) CCMalloc(InitialStackSize * ITEM_SIZE);

		// Complain and return if it didn't work.
		if (TheStack == NULL)
			return FALSE;

		// Update stack limit to reflect new size
		StackLimit = InitialStackSize;

	}
	else
	{
//		if (IsUserName("Tim"))
//			TRACE( _T("Growing heap from %d to %d entries\n"), 
//				  StackLimit, StackLimit + StackGranularity);

		// Increase the stack allocation
		AttributeRec *NewStack = (AttributeRec *)
			CCRealloc(TheStack, (StackLimit + StackGranularity) * ITEM_SIZE);

		// Complain if no more memory
		if (NewStack == NULL)
			return FALSE;

		// Otherwise use this new block
		TheStack = NewStack;

		// Make sure only new items are initialised
		FirstNewItem = StackLimit;

		// Update stack limit to reflect new size
		StackLimit += StackGranularity;
	}

	// Initialise the stack elements
	for (UINT32 i = FirstNewItem; i < StackLimit; i++)
	{
		TheStack[i].pAttrValue = NULL;
		TheStack[i].ContextLevel = 0;
		TheStack[i].Temporary = FALSE;
	}

	// Return success
	return TRUE;
}


/********************************************************************************************

>	BOOL RenderStack::Push(AttributeValue *pAttr, BOOL Temporary = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Inputs:		pAttr - the attribute to push.
				Temporary - TRUE if the AttributeValue should be deleted when the
				attribute is popped from the stack, FALSE if not.
	Returns:	TRUE if the push succeeded, FALSE if not.
	Purpose:	Push an attribute value on to the stack.
				The 'Temporary' parameter is used when the attribute has been constructed
				especially for changing the attribute - i.e. it is not part of the Camelot
				tree.  For example, when rendering something that is not a Camelot
				document, we still need to change the line colour, so the function that
				changes the line colour creates a temporary attribute that is pushed onto
				the stack and then automatically deleted when it is removed.
				If pAttr is NULL, then it is assumed that this is the first time this
				attribute has been set, so we don't need to push the 'old' value.
	Errors:		Out of memory.
	SeeAlso:	RenderStack::RestoreContext; RenderStack::SaveContext

********************************************************************************************/

BOOL RenderStack::Push(AttributeValue *pAttr, BOOL Temporary)
{
	// If we're saving an attribute that hasn't been set up yet, don't bother.
	if (pAttr == NULL)
		return TRUE;

	// If we need more space, then get some.
	if (Top == StackLimit)
	{
		if (!GrowStack())
			return FALSE;
	}

	// Push the record onto the stack
	TheStack[Top].pAttrValue   = pAttr;
	TheStack[Top].ContextLevel = ContextLevel;
	TheStack[Top].Temporary    = Temporary;

	// Sanity checks
	CC_ASSERT_VALID(pAttr);
	ENSURE((Temporary == TRUE) || (Temporary == FALSE), 
		   "Duff Temporary flag passed to Push()");
		
	// Move stack pointer to next record
	Top++;

	// Worked ok
	return TRUE;
}


/********************************************************************************************

>	void RenderStack::RestoreContext(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Inputs:		pRegion - the render region that the context should be restored to.
	Purpose:	Restore all attributes saved by the last SaveContext() call.

********************************************************************************************/

void RenderStack::RestoreContext(RenderRegion *pRegion)
{
	ENSURE(ContextLevel > 0, "Too many calls to RestoreContext!");

	// Restore all attributes that have changed since SaveContext() was last called.
	while ((Top > 0) && (TheStack[Top-1].ContextLevel == ContextLevel))
	{
		// Move pointer to the item, because the stack is an 'empty' one.
		Top--;

		// Restore the attribute
		TheStack[Top].pAttrValue->Restore(pRegion, TheStack[Top].Temporary);

		// Make sure this is a valid pointer.
		CC_ASSERT_VALID((TheStack[Top].pAttrValue));
	}

	ContextLevel--;
}



/********************************************************************************************

>	void RenderStack::CleanUpBeforeDestruct(RenderRegion *pRegion)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/01/97
	Inputs:		pRegion - the render region that the context should be restored to.

	Purpose:	Restore all attributes saved by all SaveContext() calls ever.
				This just pops ALL entries from the stack, which gives all remaining
				attributes in the stack a chance to clean up and avoid memory leaks.
				(Used now to clean up stacked PathProcessors)

********************************************************************************************/

void RenderStack::CleanUpBeforeDestruct(RenderRegion *pRegion)
{
	ERROR3IF(pRegion == NULL, "Illegal NULL param");

	// Just repeatedly restore until we've popped everything.
	// That way if someone changes Restore, they won't break this function!
	while (ContextLevel > 0)
		RestoreContext(pRegion);
}

