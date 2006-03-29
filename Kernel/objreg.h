// $Id$
// Definition of the ObjectRegistry class
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
#ifndef INC_OBJREG
#define INC_OBJREG


#include "listitem.h" 
#include "list.h"

/********************************************************************************************

>	Class NodeAttributeClassItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/94
	Purpose:	This ListItem holds an attribute's runtime class 
	SeeAlso:	ObjectRegistry

********************************************************************************************/


class NodeAttributeClassItem: public ListItem
{
	CC_DECLARE_MEMDUMP(NodeAttributeClassItem);

	public:
	CCRuntimeClass* AttributeClass; 
};

/********************************************************************************************

>	class NodeRenderableInkDetailsItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/94
	Purpose:	This ListItem is used by the Object Registry. It has the RuntimeClass of 
				a NodeRenderableInk node, along with the following information which is
				likely to grow. 
				
				A list of the runtime classes of attributes that the NodeRenderableInk 
				class requires.  
				A Flag which indicates if the node can have NodeRenderableInk children
	SeeAlso:	-

********************************************************************************************/

class NodeRenderableInkDetailsItem: public ListItem
{
	CC_DECLARE_MEMDUMP(NodeRenderableInkDetailsItem);

	public:
		NodeRenderableInkDetailsItem() { IsABaseClass = FALSE; }
		BOOL AddAttribClass(CCRuntimeClass* pAttribClass);
		void RemoveAttribClass(CCRuntimeClass* pAttribClass);

	public:
		BOOL IsABaseClass; // When set to TRUE IsKindOf(NodeRenderableInkClass) will be called
						   // on the object's runtime class, if set to FALSE 
						   // IS_A will be called.  

		CCRuntimeClass* NodeRenderableInkClass;
		List NodeAttributeClassList; 			// A list of attribute classes that the 
												// node requires
};


/********************************************************************************************

>	class ObjectRegistry: public CCObject

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/94

	Purpose:	The object registry currently records the attributes which are required by
				each NodeRenderableInk node in Camelot. Such a list is required during
				attribute optimisation, to determine the attributes to collect to make a
				subtree attribute complete. 

				In future the object registry may contain other information we need to 
				record, such as the number of attributes defined (lots of silly DLL stuff).  

				There should never be an instance of the ObjectRegistry it is a static object

	SeeAlso:	-

********************************************************************************************/

class ObjectRegistry: public CCObject
{
	public:


		// Init Sets up the RequiredAttribList for all NodeRenderableInkNodes defined
		// in Camelot. 
		static BOOL Init(); 

		// RegisterReqdAttribs can be called to register extra NodeRenderableInkNodes 
		// if they get added in new tools.  
		static void RegisterNodeRenderableInkDetails(NodeRenderableInkDetailsItem* ReqdAttribItem);  
		
		// GetRequiredAttribs returns a list of attribute classes which are required by 
		// the NodeRenderableInk class.  
		static List* GetRequiredAttribs(CCRuntimeClass* NodeRenderableInkClass); 

		static NodeRenderableInkDetailsItem* GetAttribDetails(CCRuntimeClass* pInkClass);

		static void Delete(); // Deletes the registry 

		static BOOL RegisterShadowDetails();
		

	private:
		static List NodeRenderableInkDetailsList; // A list of NodeRenderableInkDetailsItems.  
}; 				
#endif
