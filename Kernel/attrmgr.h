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
*/

#ifndef INC_ATTRMGR
#define INC_ATTRMGR

#include "list.h"
#include "listitem.h" 
#include "doccolor.h"
#include "msg.h"		// Used to include msg.h - must be done because of rampant circular #includes!!!!!
#include "range.h"
#include "nodeattr.h"

class NodeRenderableInk;
class AttributeValue;
class Node;
class ColourList;

// Graeme (21/3/00):
// The AttributeIndex typedef has been moved into nodeattr.h. This allows me to create a method
// within the NodeAttribute class to return the attribute index. So, if you're looking for this
// list, you need to see the other file.

const INT32 NUM_BASE_ATTRS = ATTR_FIRST_FREE_ID;
const INT32 NUM_ATTR_GROUPS = 2;				// Increase this everytime a new attribute group is added
											// Currently there are only two  (text, and graphic).

// Tim's default attribute structures

class AttributeEntry : public SimpleCCObject
{
public:
	// Pointer to the attribute for this entry
	AttributeValue *pAttr;

	// TRUE if this attribute is temporary and should be deleted when it's finished with.
	BOOL Temp : 2;

	// If TRUE then this attribute should not be added to a path (see ApplyBasedOnDefaults).
	BOOL Ignore : 2;
};

class NodeTypeEntry
{
public:
	// The type of node that this attribute can be applied to.
	CCRuntimeClass *pNodeType;
};


/********************************************************************************************

>	class  CurrentAttributeGroup: public CCObject

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/93
	Purpose:	The current attribute group object consists of a group identifier, and a list of 
				attribute nodes which belong to the group. It is used by the AttributeManager

	SeeAlso:	AttributeManager

********************************************************************************************/

class AttributeGroup: public ListItem
{
public:
	CCRuntimeClass* AttrGroup; 	  	// Group identifier
	CCRuntimeClass* BaseGroup;      // The base group 	(Can be NULL)

	NodeAttribute* AttrListHd; 	// List of current attributes in the group

	String_256	GroupName; 		// Used for UI descriptions
	
}; 


/********************************************************************************************

>	class AttributeGroupItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	AttributeGroupItem
	SeeAlso:	-

********************************************************************************************/


class AttributeGroupItem: public ListItem
{
public:
	CCRuntimeClass* AttrGroup; 	
};


/********************************************************************************************

>	class AttributeManager : public CCObject

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/93

	Purpose:	The attribute manager is currently responsible for managing the application 
				of current attributes. It contains a list of Attribute groups along with 
				instances of current attributes which belong to each group. An attribute group 
				is identified by the RuntimeClass of an object 
				
				Each Document should have its own set of current attributes 

				It has been extended to store default attribute information by Tim

	SeeAlso:	-

********************************************************************************************/

// This class tidied by Simon 22/09/95
// Try to keep it clean eh !
class AttributeManager: public CCObject
{
	friend class BaseCamelotFilter;

public:
	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Construction/destruction and initialisation
	AttributeManager(); 
	~AttributeManager(); 

	// Instance initialisation
	BOOL InitInstance();
		// Initialises current attributes 			

	// Static initialisation 		(Stuff Tim knows about)
	static BOOL InitDefaults();		
		// Static initialiser (mostly Tim's default stuff)
	static void Deinit();
		// Static de-initialiser		

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Fns for applying attributes to objects.
	// From Simon
	// In the future these fns could perhaps be merged into one AttributesSelected fn
	// taking a range of objects to apply the attribute/attributes too. This would me much
	// neater. The AttributeSelected and ApplyAttribToNode fns are currently a little
	// rampant,
	// At the time I wrote the AttributesSelected fn it was thought too dangerous to change
	// these fundamental routines

	static void AttributeSelected(NodeAttribute* Attrib, NodeAttribute* OldAttr = NULL);
		// For applying attributes to the selection
	
	static void AttributesSelected(List& AttribsToApply, UINT32 OpName);
		// For applying a set of attributes to the selection (as used by PasteAttributes).

	static void ApplyAttribToNode(NodeRenderableInk* InkNode, NodeAttribute* NewAttr);
		// For applying attributes to a specific Object (For drag n' drop)
	 
	enum AttrsToMakeCurrent { ALL, NOT_APPLIED, NONE };
	 	// Type used by AttributesToMakeCurrent

	AttrsToMakeCurrent GetAttributesToMakeCurrent(List& Attribs, 
											   	  List& AttrGroupsList,
												  BOOL bPasteAttrs);
		// Called to determine which attributes in the Attribs list to make current.
		// Similar to  WeShouldMakeAttrCurrent. Used by AttributesSelected.

private:
	static void ApplyAttribToNodeHelper(NodeRenderableInk* InkNode, NodeAttribute* NewAttr);
	

public:

	static void ReplaceAttributes(NodeAttribute* Attrib, List* OldAttrs);
		// Called by OpGrad after modifying a complex fill


	// Both these fns are sort of private, they are currently public cos Will needs to
	// access them from his fill stuff
	BOOL WeShouldMakeAttrCurrent(BOOL AttributeApplied, NodeAttribute* pAttr, List* pAttrGroupList, 
								 BOOL DroppedAttr = FALSE, BOOL AttrDroppedOntoPage = FALSE);
		// Called to determine if an attribute being applied should be made current



	static void UpdateCurrentAttr(NodeAttribute* Attr, BOOL Mutate, List* pAttrGroupList, 
								  BOOL TellWorld = TRUE);
		// Not to be confused with UpdateCurrentAttribute. Updates current attrs in the specified
		// list of attribute groups. Not very clean this fn 
	static BOOL UpdateCurrentAppliedAttr(NodeAttribute* pAttr, List* pAttrGroupList, BOOL bAttrApplied, BOOL bMutate);
	static BOOL UpdateAfterAttrApplied(NodeAttribute* pAttr);

	static BOOL CanBeAppliedToSelection(NodeAttribute* Attrib, List* pAttrGroups);
		// This function would ideally live in the SelRange. It determines if the attribute can
		// be applied to at least one object in the selection. Used to determine if we should
		// apply the attribute or not!
 
	static BOOL CanBeAppliedToNode(NodeRenderableInk* pObject,
								   NodeAttribute* Attrib, List* pAttrGroups);

	
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Fns which deal with current attributes
	// 

	BOOL ApplyCurrentAttribsToNode(NodeRenderableInk* Node);
		// This is the fn that people call prior to DoInserting a new object into the tree
		// it attaches the current attributes from the attribute group associated with the object.

	NodeAttribute* GetSelToolCurrentAttribute(CCRuntimeClass* AttrType); 

	NodeAttribute* GetCurrentAttribute(CCRuntimeClass* AttrGroup,
										   		 	 	CCRuntimeClass* AttrType); 

	void GetCurrentLineAndFillColour(CCRuntimeClass* AttrGroup, DocColour* LineCol, 
									 DocColour* FillCol); 

	BOOL WriteCurrentAttributes(BaseCamelotFilter* pFilter);

private:

	// Internal fns for creating / accessing attribute groups

	BOOL CreateCurrentAttributeGroup(CCRuntimeClass* AttrGroup, CCRuntimeClass* BaseGroup, 
									 String_256& GrpName);
				// Creates a CurrentAttributeGroup ( Called from InitInstance)	

	NodeAttribute* UpdateCurrentAttribute(CCRuntimeClass* AttrGroup, 
										  NodeAttribute* Attribute,
										  BOOL fCheckTransparency = TRUE, 
										  BOOL DefiningGroups = FALSE, 
										  BOOL TellWorld = TRUE,
										  BOOL bForceNewValue = FALSE);
			// Adds attribute to specified group

	AttributeGroup* GetAttributeGroup(CCRuntimeClass* GroupID);
			// Returns pointer to attribute group 

	void AddAttributeToGroup(AttributeGroup* pAttrGroup, NodeAttribute* Attribute, 
							 BOOL fCheckTransparency, 
							 BOOL TellWorld = TRUE,
							 BOOL bRetainValues = FALSE);
			// Helper function for UpdateCurrentAttribute

	BOOL WriteCurrentAttributeGroupRecord(BaseCamelotFilter* pFilter, AttributeGroup* pGroup);
 
public:
								 
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Functions to handle default attributes (maintained globally)
	// (Tim's stuff)

	static void FindDefaultColour(ColourList *ColList, UINT32 NameResID,  DocColour *Result);

	static UINT32 RegisterDefaultAttribute(CCRuntimeClass *pNodeType, AttributeValue *pValue);
			// Pass in a default attribute value object, and it returns the unique ID allocated
			// for that attribute.

	static AttributeEntry *GetDefaultAttributes();
			// Get an array of default attribute entries.

	static NodeAttribute* GetDefaultAttribute(AttrIndex AttrID);
	static AttributeValue* GetDefaultAttributeVal(AttrIndex AttrID);
			// Added by Ilan

	static BOOL GetDefaultAttribute(AttrIndex aiToGet, AttributeValue* pavReturn);
			//Added by Graham 25/3/97
			//Get a particular default attribute (e.g. the default Web Address attribute)

	static UINT32 GetNumAttributes();
			// Find out how	many attributes have been registered.

	static BOOL ApplyBasedOnDefaults(Node *Target, AttributeEntry *AttrsToApply);
			// Given a set of attributes, apply the ones that differ from the defaults to the node.
			// Used in the filters




	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Other fns 
	
	void UpdateForDeletedColours(ColourList *ParentList);
			// Ensures no current attributes are using deleted colours.
			// Called by the ColourManager when colour lists change.
			// Nobody else should call it!

	static NodeAttribute* GetOtherAttrToApply(NodeAttribute* AttrApplied, BOOL* IsMutate);
			// Will's magic

public:
 
	// Attribute application preference vars
	static BOOL LastAttrAppliedBecomesCurrent;
	static BOOL AskBeforeSettingCurrentAttr;

	static BOOL ShouldAskAboutContoneColours;
	static BOOL HaveAskedAboutContoneColours;
	static BOOL UserCancelledContoneColours;

	static BOOL SendMessages;

	static DocColour DefaultBlack;
	static DocColour DefaultWhite;

	static NodeRenderableInk* pLastNodeAppliedTo;
	static DocRect LastAppliedBounds;

private:
	static BOOL RegisterBasicAttributes();
	static BOOL EnsureTableSpaceAvailable();

	List AttrGroupList;
		// Contains a list of all current attribute groups 
	  	
	static AttributeEntry *DefaultAttrValues;
		// Array of default attributes.

	static NodeTypeEntry  *DefaultNodeAttrs;
		// Array of default NodeAttributes.

	static UINT32 NumAttributes;
		// Number of attributes that have been registered.

	static UINT32 AttributeTableSize;
		// Size of the attribute table(s).

}; 

/********************************************************************************************

>	class CurrentAttrChangedMsg: public Msg

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Purpose:	This message is sent whenever the current attributes are updated.
	SeeAlso:	-

********************************************************************************************/

class CurrentAttrChangedMsg: public Msg
{
	CC_DECLARE_DYNAMIC(CurrentAttrChangedMsg);
public:

	CurrentAttrChangedMsg() { } 
};

/********************************************************************************************

>	class CommonAttributeItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	This ListItem is used by Range::FindCommonAttributes.

	SeeAlso:	NodeRenderableInk::FactorOutCommonAttributes
	SeeAlso:	Range::FindCommonAttributes
	SeeAlso:	CommonAttributeSet 

********************************************************************************************/

class CommonAttributeItem: public ListItem
{
 
public:
	CommonAttributeItem(CCRuntimeClass* AtrType, NodeAttribute* pAttrib=NULL, 
						Range::CommonAttribResult status = Range::ATTR_NONE) 
		{ AttrType = AtrType; pAttr = pAttrib; Status = status; pAttrIsCopy = FALSE; };

	~CommonAttributeItem();  

	
	// Initialiser method for creating a CommonAttributeTypeItem with a copy of the pAttrib
	// This is useful if CommonAttributeItems are cached.	
	BOOL InitSafeAttrItem(CCRuntimeClass* AtrType, NodeAttribute* pAttrib=NULL, 
						  Range::CommonAttribResult status = Range::ATTR_NONE);
	
	// Clears the CommonAttrItem's result fields					  
	void ClearResults() { if (pAttrIsCopy && pAttr)
					 	delete pAttr; 
					 pAttr = NULL; 
					 Status = Range::ATTR_NONE; };
 		
	CCRuntimeClass* AttrType;	  // The RuntimeClass of the attribute
	NodeAttribute* pAttr;		  // A pointer to an instance of an attribute with AttrType
								  // Runtime class - can be NULL

	Range::CommonAttribResult Status; // One of the following values:-
	
								  // ATTR_COMMON - pAttr will point at an instance of the 
								  //			   common attribute

								  // ATTR_NONE - There is no common attribute, cos there were
								  // 			 no items in the range, or none of the items
								  // 			 required the attribute. In this situation 
								  //			 pAttr points at the current attribute for 
								  //			 the selected tool. This is useful cos it's
								  //			 the attribute you would display in this 
								  //			 situation. 
								  
								  // ATTR_MANY - There is no common attribute, cos 2 or more 
								  // 			 objects had different attributes of type 
								  //			 AttrType.in this situation pAttr will be NULL.
								  //			 In this situation 'MANY' would normally be 
								  //			 displayed.
private:
	BOOL pAttrIsCopy; 			  // Is pAttr a pointer to an attribute, or a copy. If it is a 
								  // copy then it is the responsibility of the CommonAttributeItem's
								  // destructor to destroy it.
	 
};


#endif
