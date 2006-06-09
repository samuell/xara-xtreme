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
// FreeHand Tool Info bar operations


#ifndef LIVEEFFECTSINFO_INC
#define LIVEEFFECTSINFO_INC

//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "xmlutils.h"
#include "valfunc.h"  // for valuefunctionID

class LiveEffectsTool;
class EffectsStack;




/********************************************************************************************

>	class LiveEffectsInfoBarOp : public InformationBarOp

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Purpose:	Class for handling the FreeHand tool's information bar.

********************************************************************************************/

class LiveEffectsInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( LiveEffectsInfoBarOp )  
public:
	// Dummy default constructor for DYNCREATE
	LiveEffectsInfoBarOp();
	~LiveEffectsInfoBarOp();

	// All messages to the info bar come through here
	MsgResult 			Message(Msg* Msg);

	// Functions to allow the tool to talk to the info bar
	void				SetToolActiveState(BOOL IsToolActive);
	virtual void		UpdateState(); // Called during idle processing to disable controls
	virtual void		HandleCreate();

// LiveEffect Tool specific functionality
protected:
	void				SetGadgetWritable(INT32 id, BOOL enable);
	void				GadgetMessage_Label(DialogMsg* Message);
	void				GadgetMessage_Order(DialogMsg* Message);
	void				GadgetMessage_Edit(DialogMsg* Message);
	void				GadgetMessage_Add(DialogMsg* Message);
	void				GadgetMessage_Insert(DialogMsg* Message);
	void				GadgetMessage_Remove(DialogMsg* Message);
	void				GadgetMessage_RemoveAll(DialogMsg* Message);
	void				GadgetMessage_Setup(DialogMsg* Message);
	void				GadgetMessage_Resolution(DialogMsg* Message);
	void				GadgetMessage_ToggleLock(DialogMsg* Message);
	void				GadgetMessage_ToggleLockAll(DialogMsg* Message);

	void				PopulateOrderDropdown(EffectsStack* pPPStack, INT32 iSelectedStackPos = -1);
	void				PopulateResolutionDropdown(EffectsStack* pStack, 
													ListRange* pLevelRange,
													BOOL bUpdateList = TRUE);
	void				UpdateAddButton(EffectsStack* pStack, ListRange* pLevelRange);
	void				UpdateEditButton(EffectsStack* pStack, ListRange* pLevelRange);
	void				UpdateLockButton(ListRange* pLevelRange);
	void				UpdateRemoveButton(ListRange* pLevelRange);
	void				UpdateLockAllButton();
	void				UpdateRemoveAllButton();
	void				UpdateSetupButton();

public:	
	// Keep a pointer to the Freehand tool in here
	LiveEffectsTool*	m_pTool;
	
protected:
	// Since we get Selection Changing message before the controls are made
	// we need to keep a flag to indicate if the controls have been built
	BOOL 				m_bControlsExist;
	BOOL				m_bDropDownInited;
	BOOL				m_bLockSwitchState;
	BOOL				m_bLockAllSwitchState;
};
	

/********************************************************************************************

>	class LiveEffectsInfoBarOpCreate : public BarCreate

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Purpose:	Class for creating LiveEffectsInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class LiveEffectsInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp*	Create() { return (new LiveEffectsInfoBarOp); }
};



#endif	// LIVEEFFECTSINFO_INC






