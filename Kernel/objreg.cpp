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
/*
//*/


#include "camtypes.h"
#include "objreg.h"
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
//#include "node.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodepath.h"
#include "noderect.h"
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "qualattr.h"
#include "nodebmp.h"
//#include "txtattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodetxts.h"
#include "nodershp.h"
#include "nodetext.h"
#include "nodebldr.h"
#include "ndmldpth.h"
#include "isetattr.h"
#include "userattr.h"
#include "webattr.h"
#include "strkattr.h"
#include "attrbev.h"
#include "nodeshad.h"
#include "brshattr.h"
#include "fthrattr.h"


// Declare all the class and details of memory leaks
CC_IMPLEMENT_MEMDUMP(NodeAttributeClassItem, ListItem)
CC_IMPLEMENT_MEMDUMP(NodeRenderableInkDetailsItem, ListItem)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

List ObjectRegistry::NodeRenderableInkDetailsList; 



// Implementation of NodeRenderableInkDetailsItem class

/********************************************************************************************

>	BOOL NodeRenderableInkDetailsItem::AddAttribClass(CCRuntimeClass* pAttribClass)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/2001

	Inputs:		pAttr	ptr to the runtime class of a specific attribute type.

	Returns:	TRUE if successful, FALSE otherwise (eg out of memory).

	Purpose:	Add a NodeAttribute class to this details item, eg:

					NodeRenderableInkDetailsItem* pFillDetails;
					pFillDetails->AddAttribClass( CC_RUNTIME_CLASS( AttrFillGeometry ));

				to add the AttrFillGeometry type to the pNewDetails item.

********************************************************************************************/
BOOL NodeRenderableInkDetailsItem::AddAttribClass(CCRuntimeClass* pAttribClass)
{
	NodeAttributeClassItem* pAttrClassItem = new NodeAttributeClassItem;
	if (pAttrClassItem == NULL)
		return FALSE;

	pAttrClassItem->AttributeClass = pAttribClass;
	NodeAttributeClassList.AddTail(pAttrClassItem);

	return TRUE;
}



/********************************************************************************************

>	void NodeRenderableInkDetailsItem::RemoveAttribClass(CCRuntimeClass* pAttribClass)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/2001

	Inputs:		pAttr	ptr to the runtime class of a specific attribute type.

	Purpose:	Remove a NodeAttribute class from this details item, eg:

					pFillDetails->RemoveAttribClass( CC_RUNTIME_CLASS( AttrFillGeometry ));

				to Remove the AttrFillGeometry type from the pFillDetails item.

	Notes:		* We do nothing if pAttribClass is not in our list.
				* We remove all occurrences of pAttribClass from our list.

********************************************************************************************/
void NodeRenderableInkDetailsItem::RemoveAttribClass(CCRuntimeClass* pAttribClass)
{
	NodeAttributeClassItem* pThisClassItem = (NodeAttributeClassItem*)
												NodeAttributeClassList.GetHead();
	NodeAttributeClassItem* pNextClassItem = NULL;
	while (pThisClassItem != NULL)
	{
		pNextClassItem = (NodeAttributeClassItem*) NodeAttributeClassList.GetNext(pThisClassItem);

		if (pThisClassItem->AttributeClass == pAttribClass)
		{
			NodeAttributeClassList.RemoveItem(pThisClassItem);
			delete pThisClassItem;
		}

		pThisClassItem = pNextClassItem;
	}
}



// Implementation of the object registry class


/********************************************************************************************

>	static void ObjectRegistry::Delete()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes the object registry data
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ObjectRegistry::Delete()
{
	// Delete the NodeRenderableInkDetailsList
	ListItem* Current = NodeRenderableInkDetailsList.GetHead(); 
	while (Current != NULL)
	{
		ListItem* Next = NodeRenderableInkDetailsList.GetNext(Current);
		((NodeRenderableInkDetailsItem*)Current)->NodeAttributeClassList.DeleteAll();  
		delete(NodeRenderableInkDetailsList.RemoveItem(Current));
		Current = Next; 
	}
}

/********************************************************************************************

>	static BOOL ObjectRegistry::Init() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE if we run out of memory 

	Purpose:	Sets up the NodeRenderableInkDetailsList for all NodeRenderableInkNodes defined
				in Camelot. 

	Errors:		If we run out of memory then ERROR is called and FALSE returned.
	SeeAlso:	-

********************************************************************************************/

// A little MACRO to make adding attributes to NodeRenderableInkDetailsItem's easier
#define ADD_ATTRIB_CLASS(ReqdAttribItem, AttribClass)  			\
AttrClassItem = new NodeAttributeClassItem; 		  		    \
if (AttrClassItem == NULL)							   			\
	goto OutOfMemory; 								   			\
AttrClassItem->AttributeClass = CC_RUNTIME_CLASS(AttribClass);	\
(ReqdAttribItem)->NodeAttributeClassList.AddTail(AttrClassItem); 


/*
BOOL ObjectRegistry::Init() 
{
	// All NodeRenderableInk nodes must be registered in here. If the node requires 
	// no attributes then simply give it an empty attribute list. 
	
	// Create all the attribute classes in an array so that they can easily be deleted
	// if we run out of memory. 
	NodeAttributeClassItem* AttrClassItem; 

	NodeRenderableInkDetailsItem* NodeGroupsDetails;
	NodeRenderableInkDetailsItem* NodePathsDetails; 
	NodeRenderableInkDetailsItem* NodeRectDetails;  
	NodeRenderableInkDetailsItem* NodeBitmapDetails;
	NodeRenderableInkDetailsItem* BaseTextClassDetails;
	NodeRenderableInkDetailsItem* NodeRegularShapeDetails; 

	NodeRenderableInkDetailsItem* DefaultDetails;


	// Have removed those attributes which are currently not used
	
	// ------------------------------------------------------------------------------------
    // Register NodeGroup 
	NodeGroupsDetails = new NodeRenderableInkDetailsItem(); 
	if (NodeGroupsDetails == NULL)
	{
		goto OutOfMemory; 
	}
	NodeGroupsDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeGroup); 
 	RegisterNodeRenderableInkDetails(NodeGroupsDetails);
 	
	// A node group allways requires all attributes
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrQuality);
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrLineWidth); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrStrokeTransp); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrStartArrow);
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrEndArrow); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrStartCap); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrEndCap); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrJoinType); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrMitreLimit); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrWindingRule); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrDashPattern); 
	ADD_ATTRIB_CLASS(NodeGroupsDetails, AttrMould);	 


	// ------------------------------------------------------------------------------------
    // Register NodeRegularShape
	NodeRegularShapeDetails = new NodeRenderableInkDetailsItem(); 
	if (NodeRegularShapeDetails == NULL)
	{
		goto OutOfMemory; 
	}
	NodeRegularShapeDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeRegularShape);
	NodeRegularShapeDetails->IsABaseClass = TRUE; 
 	RegisterNodeRenderableInkDetails(NodeRegularShapeDetails);
 	
	// A node group allways requires all attributes
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrQuality);
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrLineWidth); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrStrokeTransp); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrStartArrow);
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrEndArrow); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrStartCap); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrEndCap); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrJoinType); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrMitreLimit); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrWindingRule); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrDashPattern); 
	ADD_ATTRIB_CLASS(NodeRegularShapeDetails, AttrMould);

	// -------------------------------------------------------------------------------------
	// Register NodeRect
	NodeRectDetails = new NodeRenderableInkDetailsItem(); 
	if (NodeRectDetails == NULL)
	{
		goto OutOfMemory; 
	}
	NodeRectDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeRect); 
	RegisterNodeRenderableInkDetails(NodeRectDetails); 	

	ADD_ATTRIB_CLASS(NodeRectDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrQuality);
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrLineWidth); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrStrokeTransp); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrStartArrow);
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrEndArrow); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrStartCap); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrEndCap); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrJoinType); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrMitreLimit); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrWindingRule); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrDashPattern); 
	ADD_ATTRIB_CLASS(NodeRectDetails, AttrMould); 

	// -------------------------------------------------------------------------------------
	// Register NodeBitmap
	NodeBitmapDetails = new NodeRenderableInkDetailsItem(); 
	if (NodeBitmapDetails == NULL)
	{
		goto OutOfMemory; 
	}
	NodeBitmapDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeBitmap); 
	RegisterNodeRenderableInkDetails(NodeBitmapDetails); 	

	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrQuality);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrMould); 

	// BODGE we need to add a seperate list for KernCodes which don't require any attributes

	// -------------------------------------------------------------------------------------
	// Register BaseTextClass
	BaseTextClassDetails = new NodeRenderableInkDetailsItem(); 
	if (BaseTextClassDetails == NULL)
	{
		goto OutOfMemory; 
	}

	BaseTextClassDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(BaseTextClass);
	BaseTextClassDetails->IsABaseClass = TRUE; 
	RegisterNodeRenderableInkDetails(BaseTextClassDetails); 	

	// All Nodes Derived from BaseTextClass	require all the normal attributes plus attributes
	// which only make sense when applied to text nodes

	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrQuality);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrLineWidth); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrStrokeTransp); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrStartArrow);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrEndArrow); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrStartCap); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrEndCap); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrJoinType); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrMitreLimit); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrWindingRule); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrDashPattern); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrMould);

	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtFontTypeface);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtBold);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtItalic);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtUnderline);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtAspectRatio);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtJustification);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtTracking); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtFontSize); 

	// ------------------------------------------------------------------------------------
	// Register NodePath
   	NodePathsDetails = new NodeRenderableInkDetailsItem(); 
	if (NodePathsDetails == NULL)
	{
		goto OutOfMemory; 
	}
	NodePathsDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodePath); 
	RegisterNodeRenderableInkDetails(NodePathsDetails); 	

	ADD_ATTRIB_CLASS(NodePathsDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrQuality);
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrLineWidth); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrStrokeTransp); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrStartArrow);
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrEndArrow); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrStartCap); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrEndCap); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrJoinType); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrMitreLimit); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrWindingRule); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrDashPattern); 
	ADD_ATTRIB_CLASS(NodePathsDetails, AttrMould); 


	// Set up the default NodeAttributeClassList. This list is returned by 
	// GetRequiredAttribs if the NodeRenderableInkClass is not found in 
	// the registry (this should never occur!!).

	DefaultDetails = new NodeRenderableInkDetailsItem(); 
	if (DefaultDetails == NULL)
	{
		goto OutOfMemory; 
	}
	DefaultDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeRenderableInk); 
 	RegisterNodeRenderableInkDetails(DefaultDetails); 

	// Add all attributes in here 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrQuality);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrLineWidth); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrStrokeTransp); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrStartArrow);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrEndArrow); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrStartCap); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrEndCap); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrJoinType); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrMitreLimit); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrWindingRule); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrDashPattern); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrMould);
	
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTxtFontTypeface);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTxtBold);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTxtItalic);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTxtUnderline);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTxtAspectRatio);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTxtJustification);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTxtTracking); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTxtFontSize); 

	return (TRUE); 

	OutOfMemory:
	return (FALSE);					// Failed to initialise.  
}
*/


BOOL ObjectRegistry::Init() 
{
	// The following attribute groups have been set up
	/* 

							  	
	   NodeBitmapDetails: All attributes which can be applied to bitmap objects. This is a
						  subset of the Default attributes.
	   
	   BaseTextClassDetails: All attributes which can be applied to objects derived from the 
	   					  	 BaseTextClass. The set includes all DefaultAttributes + attributes
						  	 which only make sense when applied to text objects like BOLD, 
						  	 ITALIC etc. 

	   DefaultAttributes: The attributes required by all other nodes

	   AllAttrDetails: A list of all attributes	 

	*/

	NodeAttributeClassItem* AttrClassItem; 

	NodeRenderableInkDetailsItem* NodeBitmapDetails;
	NodeRenderableInkDetailsItem* BaseTextClassDetails;
	NodeRenderableInkDetailsItem* DefaultDetails;
	NodeRenderableInkDetailsItem* AllAttrDetails;
	NodeRenderableInkDetailsItem* NodeBlenderDetails;
	NodeRenderableInkDetailsItem* NodeMouldPathDetails;



	// -------------------------------------------------------------------------------------
	// Register NodeBitmap
	NodeBitmapDetails = new NodeRenderableInkDetailsItem(); 
	if (NodeBitmapDetails == NULL)
	{
		goto OutOfMemory; 
	}
	NodeBitmapDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeBitmap); 
	RegisterNodeRenderableInkDetails(NodeBitmapDetails); 	

	// Audited by Will
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrQuality);
	// LineWidth required by NodeBitmap but should always be 0, it should be applied when a 
	// bitmap is created.
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrLineWidth);	   
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrStrokeColour); 

	//This is here so you can add HotLinks to bitmaps if you want...Graham
	ADD_ATTRIB_CLASS(NodeBitmapDetails, AttrWebAddress);


	// -------------------------------------------------------------------------------------
	// Register NodeMouldPath, require no attributes
	
	NodeMouldPathDetails = new NodeRenderableInkDetailsItem(); 
	if (NodeMouldPathDetails == NULL)
	{
		goto OutOfMemory; 
	}
	NodeMouldPathDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeMouldPath); 
	RegisterNodeRenderableInkDetails(NodeMouldPathDetails);

	// -------------------------------------------------------------------------------------
	// Register NodeBlender, require no attributes
	
	NodeBlenderDetails = new NodeRenderableInkDetailsItem(); 
	if (NodeBlenderDetails == NULL)
	{
		goto OutOfMemory; 
	}
	NodeBlenderDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeBlender); 
	RegisterNodeRenderableInkDetails(NodeBlenderDetails); 	

	// -------------------------------------------------------------------------------------
	// Register ShadowDetails attributes required by NodeShadow

	if (!RegisterShadowDetails())
		goto OutOfMemory;

	// -------------------------------------------------------------------------------------
	// Register BaseTextClass
	BaseTextClassDetails = new NodeRenderableInkDetailsItem(); 
	if (BaseTextClassDetails == NULL)
	{
		goto OutOfMemory; 
	}

	BaseTextClassDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(BaseTextClass);
	BaseTextClassDetails->IsABaseClass = TRUE; 
	RegisterNodeRenderableInkDetails(BaseTextClassDetails); 	

	// All Nodes (except kerns) Derived from BaseTextClass require all the default attributes 
	// plus attributes which only make sense when applied to text nodes

	// Proper default attribute list (audited by Will)
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrQuality);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrLineWidth); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrStrokeTransp); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrStartArrow);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrEndArrow); 
	//ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrEndCap);  // RIP
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrStartCap); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrJoinType); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrMitreLimit); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrUser);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrWebAddress);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrWindingRule); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrDashPattern); 
	//ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrMould);	   // RIP

	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtFontTypeface);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtBold);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtItalic);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtUnderline);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtAspectRatio);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtJustification);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtTracking); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtFontSize); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtBaseLine);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtScript); 
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrTxtLineSpace); 

	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrOverprintLine);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrOverprintFill);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrPrintOnAllPlates);
 
#ifdef VECTOR_STROKING // Neville 6/8/97
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrStrokeType);
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrVariableWidth);
#endif // VECTOR_STROKING
	ADD_ATTRIB_CLASS(BaseTextClassDetails, AttrBrushType);

	// -------------------------------------------------------------------------------------
	// Register DefaultDetails
				  
	DefaultDetails = new NodeRenderableInkDetailsItem(); 
	if (DefaultDetails == NULL)
	{
		goto OutOfMemory; 
	}
	DefaultDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeRenderableInk);
	DefaultDetails->IsABaseClass = TRUE; 
 	RegisterNodeRenderableInkDetails(DefaultDetails); 

	// Proper default attribute list (audited by Will)
	ADD_ATTRIB_CLASS(DefaultDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrQuality);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrLineWidth); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrStrokeTransp); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrStartArrow);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrEndArrow); 
	//ADD_ATTRIB_CLASS(DefaultDetails, AttrEndCap);  // RIP
	ADD_ATTRIB_CLASS(DefaultDetails, AttrStartCap); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrJoinType); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrMitreLimit); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrUser);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrWebAddress);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrWindingRule); 
	ADD_ATTRIB_CLASS(DefaultDetails, AttrDashPattern); 
	//ADD_ATTRIB_CLASS(DefaultDetails, AttrMould);	   // RIP

	ADD_ATTRIB_CLASS(DefaultDetails, AttrOverprintLine);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrOverprintFill);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrPrintOnAllPlates);
	
#ifdef VECTOR_STROKING // Neville 6/8/97
	ADD_ATTRIB_CLASS(DefaultDetails, AttrStrokeType);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrVariableWidth);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrBevelIndent);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrBevelLightAngle);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrBevelContrast);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrBevelType);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrBevelLightTilt);	
#endif // VECTOR_STROKING
	ADD_ATTRIB_CLASS(DefaultDetails, AttrBrushType);
	ADD_ATTRIB_CLASS(DefaultDetails, AttrFeather);

	// -------------------------------------------------------------------------------------
	// Register AllAttrDetails attributes required by compound nodes (all attributes)

   	AllAttrDetails = new NodeRenderableInkDetailsItem(); 
	if (AllAttrDetails == NULL)
	{
		goto OutOfMemory; 
	}

	AllAttrDetails->NodeRenderableInkClass = 0; // Has no class
 	RegisterNodeRenderableInkDetails(AllAttrDetails); 

	// Add all attributes in here 
	// Proper default attribute list (audited by Will)
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrFillGeometry);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTranspFillGeometry);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrFillMapping);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTranspFillMapping);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrFillEffect);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrQuality);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrLineWidth); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrStrokeColour); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrStrokeTransp); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrStartArrow);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrEndArrow); 
	//ADD_ATTRIB_CLASS(AllAttrDetails, AttrEndCap);  // RIP
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrStartCap); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrJoinType); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrMitreLimit); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrUser);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrWebAddress);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrWindingRule); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrDashPattern); 
	//ADD_ATTRIB_CLASS(AllAttrDetails, AttrMould);	   // RIP
	
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtFontTypeface);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtBold);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtItalic);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtUnderline);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtAspectRatio);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtJustification);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtTracking); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtFontSize); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtBaseLine);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtScript); 
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrTxtLineSpace); 

	ADD_ATTRIB_CLASS(AllAttrDetails, AttrOverprintLine);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrOverprintFill);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrPrintOnAllPlates);

#ifdef VECTOR_STROKING // Neville 6/8/97
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrStrokeType);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrVariableWidth);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrBevelIndent);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrBevelLightAngle);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrBevelContrast);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrBevelType);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrBevelLightTilt);
#endif // VECTOR_STROKING
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrBrushType);
	ADD_ATTRIB_CLASS(AllAttrDetails, AttrFeather);

	return (TRUE); 

	OutOfMemory:
	return (FALSE);					// Failed to initialise.  
}


/********************************************************************************************

>	static void ObjectRegistry::RegisterNodeRenderableInkDetails(NodeRenderableInkDetailsItem* ReqdAttribItem)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/94
	Inputs:		The NodeRenderableInkDetailsItem that needs to be registered. 
	Outputs:	-
	Returns:	-
	Purpose:	RegisterNodeRenderableInkDetails can be called to register extra 
				NodeRenderableInkNodes if they get added in new tools. This must be done at startup  

	Errors:		-
	SeeAlso:	ObjectRegistry::Init

********************************************************************************************/

void ObjectRegistry::
	RegisterNodeRenderableInkDetails(NodeRenderableInkDetailsItem* ReqdAttribItem)  
{
	// Simply add the item 
	NodeRenderableInkDetailsList.AddTail(ReqdAttribItem);
	return;  			
}

/********************************************************************************************

>	static List* ObjectRegistry::GetRequiredAttribs(CCRuntimeClass* NodeRenderableInkClass) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/94
	Inputs:		NodeRenderableInkClass: The class to find the required attributes for
				when NULL a complete list of all attributes is returned. 
	Outputs:	-
	Returns:	A list of required attributes for NodeRenderableInkClass. The list contains 
				NodeAttributeClassItems

	Purpose:	GetRequiredAttribs returns a list of attribute classes which are required by 
				the NodeRenderableInk class. Note that the NodeRenderableInkClass must have
				been registerd. 

				In the situation where a NodeRenderableInk class has not been registered
				we return back a default attribute list	in a retail build. In a debug
				build an ENSURE failure will occur. 

	Errors:		An ENSURE failure will occur if the NodeRenderableInk class has not been 
				registered
	SeeAlso:	-

********************************************************************************************/

List* ObjectRegistry::GetRequiredAttribs(CCRuntimeClass* NodeRenderableInkClass)
{
	// Search for the required Attrib list for the NodeRenderableInkClass
	NodeRenderableInkDetailsItem* Current = (NodeRenderableInkDetailsItem*)
		NodeRenderableInkDetailsList.GetHead();
	while (Current != NULL)
	{
		if ( 
				((Current->IsABaseClass) && 
					(NodeRenderableInkClass != NULL) && 
					(NodeRenderableInkClass->IsKindOf(Current->NodeRenderableInkClass))) ||
				(Current->NodeRenderableInkClass == NodeRenderableInkClass)
		   ) 
		{
			// The default attributes 
			return (&(Current->NodeAttributeClassList)); 
		}
	
		Current = (NodeRenderableInkDetailsItem*)
			NodeRenderableInkDetailsList.GetNext(Current); 
	}
	ENSURE(FALSE, "Could not find required attributes");
	return NULL;
}



/********************************************************************************************

>	static NodeRenderableInkDetailsItem* ObjectRegistry::GetAttribDetails(CCRuntimeClass* pInkClass)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/2001

	Inputs:		pInkClass	the class type to lookup details for.

	Returns:	The attribute details item for the given class type.

	Errors:		ERROR2 if NULL input param or we couldn't find details for it.

********************************************************************************************/
NodeRenderableInkDetailsItem* ObjectRegistry::GetAttribDetails(CCRuntimeClass* pInkClass)
{
	// validate inputs.
	ERROR2IF(pInkClass == NULL, NULL,
			"ObjectRegistry::GetAttribDetails; NULL input parameter!");

	// Search for the required details item for the given NodeRenderableInk Class.
	NodeRenderableInkDetailsItem* pCurrent = NULL;
	pCurrent = (NodeRenderableInkDetailsItem*)NodeRenderableInkDetailsList.GetHead();

	while (pCurrent != NULL)
	{
		if ((pCurrent->IsABaseClass && pInkClass->IsKindOf(pCurrent->NodeRenderableInkClass)) ||
			pCurrent->NodeRenderableInkClass == pInkClass)
		{
			return pCurrent;
		}

		pCurrent = (NodeRenderableInkDetailsItem*)NodeRenderableInkDetailsList.GetNext(pCurrent);
	}

	ERROR2(NULL, "ObjectRegistry::GetAttribDetails; Couldn't find details!");
}



/********************************************************************************************

>	static BOOL ObjectRegistry::RegisterShadowDetails()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/2001

	Purpose:	Register required attribute details for shadows.

	See also:	Init(), RegisterNodeRenderableInkDetails().

********************************************************************************************/
BOOL ObjectRegistry::RegisterShadowDetails()
{
	NodeRenderableInkDetailsItem* pShadowDetails = new NodeRenderableInkDetailsItem();
	BOOL	ok = (pShadowDetails != NULL);
	if (ok)
	{
		pShadowDetails->NodeRenderableInkClass = CC_RUNTIME_CLASS(NodeShadow);
		RegisterNodeRenderableInkDetails(pShadowDetails);
	}

	if (ok)	ok = pShadowDetails->AddAttribClass( CC_RUNTIME_CLASS( AttrFillGeometry		));
	if (ok)	ok = pShadowDetails->AddAttribClass( CC_RUNTIME_CLASS( AttrFillMapping		));
	if (ok)	ok = pShadowDetails->AddAttribClass( CC_RUNTIME_CLASS( AttrFillEffect		));
	if (ok)	ok = pShadowDetails->AddAttribClass( CC_RUNTIME_CLASS( AttrUser				));
	if (ok)	ok = pShadowDetails->AddAttribClass( CC_RUNTIME_CLASS( AttrWebAddress		));
	if (ok)	ok = pShadowDetails->AddAttribClass( CC_RUNTIME_CLASS( AttrOverprintLine	));
	if (ok)	ok = pShadowDetails->AddAttribClass( CC_RUNTIME_CLASS( AttrOverprintFill	));
	if (ok)	ok = pShadowDetails->AddAttribClass( CC_RUNTIME_CLASS( AttrPrintOnAllPlates	));

	return ok;
}
