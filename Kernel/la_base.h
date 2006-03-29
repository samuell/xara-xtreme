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

	la_base.h

	Line-attribute operations.
*/

#ifndef LA_BASE_H
#define LA_BASE_H

#include "opdesc.h"


/********************************************************************************************
>	class ChangeLineAttribOpDesc : public UndoableOpDescriptor

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/95
	Purpose:	Abstract base-class OpDescriptor to allow the user to change a line-attribute
				of all selected objects, or to set the AttributeManager's current attribute
				if no objects are selected.

				The OpDescriptor invokes the OpApplyAttribToSelected operation if nodes
				are selected.
********************************************************************************************/

class OpChangeLineAttribOpDesc : public UndoableOpDescriptor
{
public:
	// Creation & destruction.
	OpChangeLineAttribOpDesc(UINT32 nTxtID, const TCHAR* lpcszToken,
							 UINT32 nBubbleID, UINT32 nControlID);   

	// Initialise the gadget.
	virtual void OnControlCreate(OpDescControlCreateMsg* CreateMsg);
	
	// Message handlers.
	virtual void OnSelectionChange(OpDescControlMsg* SelChangedMsg, List* GadgetList);
	virtual BOOL OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State);
	// This fn is now redundant	(replaced by OnCommonAttrsChangedMsg)
	//virtual BOOL OnSelChangingMsg(SelChangingMsg::SelectionState State);
	virtual BOOL OnCommonAttrsChangedMsg();

	// GetState function for the OpDescriptor
	static OpState GetState(String_256* Description, OpDescriptor*); 

protected:
	// This functions sets the given text in all gadgets associated with this OpDescriptor.
	void SetGadgetText(const String& str);

	// This function handles setting the text within the combo (to do this it calls the above
	// SetGadgetText function).  In extreme cases you can override this.
	virtual BOOL SetCurrentSelectedAttrib();
	
	// Derived classes must provide these functions.  This one returns the run-time class
	// of the attribute the derived class is interested in.
	virtual CCRuntimeClass* GetAttribRuntimeClass() const = 0;
	
	// This one returns a string ID which describes the given attribute.
	virtual UINT32 ConvertAttribToStringID(NodeAttribute* pAttr) const = 0;
	
	// This one returns the string ID of the text to put in the combo-box at the given
	// index (position), or 0 if there isn't any item at the given index.
	virtual UINT32 GetItemStringID(INT32 nItemIndex) const = 0;
	
	// This one converts the given combo index position into a pointer to the appropriate
	// kind of attribute.
	virtual NodeAttribute* GetNewAttrib(INT32 nItemIndex) const = 0;
};

#endif	// !LA_BASE_H
