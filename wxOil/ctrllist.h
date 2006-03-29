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

#ifndef INC_CTRLLIST
#define	INC_CTRLLIST

#include "opdesc.h"

class DialogOp;

/*******************************************************************************************

>	class ControlList : public CCObject

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	19/12/2005
	Purpose:	A class to maintain a list of the wxControls that might be on bars
	Notes:		In the OIL
	See Also:	

This class maintains a list of wxControls, and what OpDescriptor (if any) they
are associated with. It is by no means a sin for a wxControl not to have an
OpDescriptor. However, if it does, ControlList will cause it to grey and
ungrey, and be ticked and unticked.

Right now we are doing this on idle events (why oh why?) for compatibilty.

********************************************************************************************/

class ControlPtrToListEntry;

class ControlList : public CCObject
{
public:
	CC_DECLARE_DYNCREATE( ControlList )

	class ListEntry
	{
		public:
			OpState m_OpState;
			OpDescriptor * m_pOpDesc;
			DialogOp * m_pDialogOp;
			BOOL m_SpecifiedOpDesc;
			ListEntry(OpDescriptor * opdesc = NULL) {m_pOpDesc = opdesc; m_SpecifiedOpDesc = FALSE; m_pDialogOp = NULL;}
	};

	ControlList();
	virtual ~ControlList();

	static ControlList * Get() {return m_pControlList;}
	static BOOL Init();
	static void DeInit();

	BOOL NewControl(wxControl * pControl);
	BOOL DeleteControl(wxControl * pControl);
	BOOL AssociateControl(wxControl * pControl, DialogOp * pDialogOp = NULL);

	BOOL ReflectAllStates(BOOL IgnoreChangedFlag = FALSE);

	void Changed() {m_ChangedFlag = TRUE;}

	void Invoke(wxControl * pControl, OpParam* pOpParam = NULL, BOOL fWithUndo = TRUE);
	OpDescriptor * Find(wxControl * pControl);
	BOOL BuildGadgetList(List* pOutputList, OpDescriptor * pOpDesc);

	static void Idle() {if (m_pControlList) m_pControlList->ReflectAllStates();}

	BOOL SendMessageToAllControls(DialogOp * pDialogOp, DialogMsg * DlgMsg, CWindowID pWindow = NULL);

	void RegisterWindowAndChildren(wxWindow * pwxWindow, DialogOp * pDialogOp );
	void RemoveWindowAndChildren(wxWindow * pwxWindow);

private:
	static ControlList * m_pControlList;

	ControlPtrToListEntry * m_pHash;
	BOOL m_ChangedFlag;

	BOOL ReflectState(wxControl * pControl, ControlList::ListEntry * pListEntry, BOOL ForceUpdate=FALSE);
};

typedef wxControl * wxControl_ptr;
WX_DECLARE_HASH_MAP( wxControl_ptr, ControlList::ListEntry, wxPointerHash, wxPointerEqual, ControlPtrToListEntry );

#endif

