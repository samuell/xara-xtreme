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
// Implementation of the node operations

#include "camtypes.h"
#include "selop.h"	
#include "nodeattr.h"	
#include "opnode.h"	

#include "objchge.h"	
#include "app.h"	
#include "attrmgr.h"	
//#include "tmpltres.h"	


DECLARE_SOURCE( "$Revision$" );

CC_IMPLEMENT_DYNAMIC(RemoveAttributeParam, OpParam)
CC_IMPLEMENT_DYNCREATE(OpRemoveAttributeFromSelection, SelOperation)

#define new CAM_DEBUG_NEW


#define ENSURE_NOT_NULL(Pointer)		ERROR2IF(Pointer == NULL, 0, "NULL Args")
#ifndef VOID_ENSURE_NOT_NULL
#define VOID_ENSURE_NOT_NULL(p)		if (p == NULL) {	ERROR2RAW("NULL Args")	}
#endif
#define ENSURE_KIND(pNode, Kind)		ERROR3IF(!pNode->IS_KIND_OF(Kind), "pNode is not kind of " #Kind);

/********************************************************************************************

>	BOOL OpRemoveAttributeFromSelection::Declare()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Will - I wish I hadn't)
	Created:	09/07/97

	Purpose:	Declares the OpDescriptor for OpRemoveAttributeFromSelection. Should be
				called on start up.

	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
********************************************************************************************/
BOOL OpRemoveAttributeFromSelection::Declare()
{
	
	// Register the opdescriptors for the OpRemoveAttributeFromSelection operation

	return (RegisterOpDescriptor(	0,
								 	_R(IDS_DELETEATTROP),
									CC_RUNTIME_CLASS(OpRemoveAttributeFromSelection),
								 	OPTOKEN_DELETEATTR,
								 	OpRemoveAttributeFromSelection::GetState,
 									0,	/* help ID */
 									0,
 									0,
 									0,
									SYSTEMBAR_ILLEGAL,			// For now !
									FALSE, 						// Receive messages
									FALSE,
									FALSE,
									0,
									0 // (GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
	       )); 
}               


/********************************************************************************************

>	OpState	OpRemoveAttributeFromSelection::GetState(String_256*, OpDescriptor* OpDesc)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Will - I wish I hadn't)
	Created:	09/07/97

	Purpose:	For finding OpRemoveAttributeFromSelection state.

	Returns:	The state of the OpRemoveAttributeFromSelection operation

	Notes:		Always available cos it ain't in no menu
				
********************************************************************************************/
OpState	OpRemoveAttributeFromSelection::GetState(String_256*, OpDescriptor* OpDesc)
{
	OpState OpSt;

	OpSt.Greyed = ( FALSE );  
												
	return(OpSt);   
}


/********************************************************************************************

>	virtual void OpRemoveAttributeFromSelection::GetOpName(String_256* OpName) 

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Will - I wish I hadn't)
	Created:	09/07/97

	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 

	Outputs:	The undo string for the operation

********************************************************************************************/
void OpRemoveAttributeFromSelection::GetOpName(String_256* OpName) 
{ 
	*OpName += String_256(_R(IDS_ATTR_CHANGE));
}  


/********************************************************************************************

>	virtual void OpRemoveAttributeFromSelection::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Will - I wish I hadn't)
	Created:	09/07/97

	Purpose:	Performs the OpRemoveAttributeFromSelection operation.
				It removes all attributes with the given AttributeIdentifier from the
				selected objects.

	Inputs:		pOpDesc:	See base class
				pOpParam:	A RemoveAttributeParam containing the AttributeIdentifier of
							the attributes to remove.

********************************************************************************************/
void OpRemoveAttributeFromSelection::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{    
	VOID_ENSURE_NOT_NULL(pOpParam);
	ENSURE_KIND(pOpParam, RemoveAttributeParam);

	const RemoveAttributeParam* const pRemoveParam = (RemoveAttributeParam*)pOpParam;

	// Objects used to mark changed nodes, so that parents will update after attr replacement
	ObjChangeFlags cFlags;
	cFlags.Attribute	= TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	BOOL Ok = TRUE;

	Ok = DoStartSelOp(FALSE, FALSE, FALSE, FALSE);	// FALSE's == don't render the blobs

	// Obtain the current selection
	SelRange* const pSelRange = GetApplication()->FindSelection();
	Ok = (pSelRange != NULL);

	if (Ok)
	{

		if (Ok)
		{
			Range CurrentSelection(*pSelRange);
			// Mark nodes that will allow this to happen, 
			// and error if no nodes will let it happen
	
			Ok = CurrentSelection.AllowOp(&ObjChange);
		}
	}

	if (Ok)
	{
		AttributeIdentifier AttrIDToDelete = pRemoveParam->GetAttrIDToDelete();

		Node* pNode = pSelRange->FindFirst();
		while (pNode != NULL && Ok)
		{
			NodeAttribute* const pAttr = FindAttributeOfClassID(pNode, AttrIDToDelete);
			if (pAttr != NULL)
			{
				Ok = DoRemove(pAttr);
			}

			pNode = pSelRange->FindNext(pNode);
		}
	}

	if (Ok)
	{
		// Update all parents of this
		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
		Ok = UpdateChangedNodes(&ObjChange);
	}

	if (!Ok)
	{
		FailAndExecute();
	}

	End(); // End of operation
} 


/********************************************************************************************

>	NodeAttribute* OpRemoveAttributeFromSelection::FindAttributeOfClassID(Node* const pNode, 
																AttributeIdentifier AttrID)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08//07/97

	Purpose:	Support function to find an attribute of the given class

	Inputs:		pNode : The attribute to go
				AttrID:	The AttributeIdentifier of the attribute to find

	Returns:	An attribute of the given class or NULL if none found

********************************************************************************************/
NodeAttribute* OpRemoveAttributeFromSelection::FindAttributeOfClassID(Node* const pNode, AttributeIdentifier AttrID)
{
	NodeAttribute* pAttr = NodeAttribute::FindFirstAppliedAttr(pNode);

	while (pAttr)
	{
		if (pAttr->GetAttributeClassID() == AttrID)
			return pAttr;

		pAttr = NodeAttribute::FindPrevAppliedAttr(pAttr);
	}

	return NULL;
}



/********************************************************************************************

>	BOOL OpRemoveAttributeFromSelection::DoRemove(NodeAttribute* const pOldAttr)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08//07/97

	Purpose:	Support function to remove the attribute

	Inputs:		pOldAttr : The attribute to go
	Returns:	TRUE if successful 
				FALSE if the operation should be aborted (TIDYUP THEN CALL End()!) 

********************************************************************************************/
BOOL OpRemoveAttributeFromSelection::DoRemove(NodeAttribute* const pOldAttr)
{
	ENSURE_NOT_NULL(pOldAttr);		// well use a reference then!

	BOOL Ok = TRUE;

	Node* CurrentNode = pOldAttr->FindParent();
	ERROR3IF(CurrentNode == NULL, "Can't find parent node in Replace Attr");

	// Now we have done with the old attribute, so lets hide it, so
	// the changes can be undone
	Ok = DoHideNode(pOldAttr, 
					TRUE,		// Include the subtree size (not that there is one)
					NULL,		// the hidden node
					TRUE);		// make sure the attribute's HidingNode function is called

	return Ok;
}
