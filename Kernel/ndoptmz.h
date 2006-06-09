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
 * */
// Declaration of classes required for attribute optimisation

#ifndef INC_NDOPTMZ
#define INC_NDOPTMZ											  

//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]

/********************************************************************************************

>	class RequiredAttributeItem: public ListItem


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/94
	Purpose:	This ListItem holds an attribute's runtime class, plus a flag indicating 
				if we have found the attribute. It is used by NodeRenderable::MakeAttributeComplete
				 
	SeeAlso:	NodeRenderableInk::MakeAttributeComplete

********************************************************************************************/


class RequiredAttributeItem: public ListItem
{
	public:
	CCRuntimeClass* AttributeClass;  // The class of the attribute we need to find
	BOOL Found; 					 // A flag which indicates that we have found the 
									 // attribute. 
};

/********************************************************************************************

>	class NodeAttributePtrItem: public ListItem


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/94
	Purpose:	This ListItem holds a ptr to an attribute 
				 
	SeeAlso:	NodeRenderable::MakeAttributeCompleteInk

********************************************************************************************/


class NodeAttributePtrItem: public ListItem
{
	public:
	NodeAttribute* NodeAttribPtr; 

}; 


/********************************************************************************************

>	class FactorOutCommonChildAttrAct: public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Purpose:	When executed this action will factor out all attributes which are common 
				to all children of the compound object. All common attributes become first 
				children of the compound object.
				
				A LocaliseCommonAttrAct action twin is generated to redo
 

	SeeAlso:	UndoableOperation::DoLocaliseCommonAttributes

********************************************************************************************/


class CCAPI FactorOutCommonChildAttrAct: public Action
{                                
	CC_DECLARE_DYNCREATE( FactorOutCommonChildAttrAct )

public:
	FactorOutCommonChildAttrAct(); 

	virtual ActionCode Execute();  
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   NodeRenderableInk* CompoundObject,
						   BOOL Global, 
						   AttrTypeSet* pAffectedAttrTypes, 
						   Action** NewAction);
						   
	virtual void Slaughter(); // Destroys the pSetOfAffectedAttrTypes 

private:
		
	NodeRenderableInk* CompoundObj; 
	AttrTypeSet* pSetOfAffectedAttrTypes;
	BOOL Global; 

};   
	
/********************************************************************************************

>	class LocaliseCommonAttrAct: public Action

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Purpose:	This action copies all attributes common to the group to each child object 
				within the group which requires each attribute. The groups common attributes 
				are deleted.

				A FactorOutCommonChildAttrAct action twin is generated to redo

	SeeAlso:	UndoableOperation::DoFactorOutCommonChildAttributes

********************************************************************************************/


class CCAPI LocaliseCommonAttrAct: public Action
{                                
	CC_DECLARE_DYNCREATE( LocaliseCommonAttrAct )

public:
	LocaliseCommonAttrAct(); 

	virtual ActionCode Execute();  
	static ActionCode Init(Operation* const pOp, 
						   ActionList* pActionList, 
						   NodeRenderableInk* CompoundObject,
						   BOOL global, 
						   AttrTypeSet* pAffectedAttrTypes,  
						   Action** NewAction); 

	virtual void Slaughter(); // Destroys the pSetOfAffectedAttrTypes 

private:
		
	NodeRenderableInk* CompoundObj;
	AttrTypeSet* pSetOfAffectedAttrTypes; 
	BOOL Global;
};   

#endif
