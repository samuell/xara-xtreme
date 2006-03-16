// $Id: cartctl.cpp 556 2006-03-03 18:07:25Z alex $
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


#include "camtypes.h"

#include <wx/wxprec.h>
#include <wx/event.h>
#include <wx/tooltip.h>

#include "cartctl.h"
#include "cartprov.h"
#include "dlgevt.h"
#include "wxmousestate.h"
#include "dlgmgr.h"
#include "bars.h"

CC_IMPLEMENT_DYNCREATE(ControlList, CCObject)
ControlList * ControlList::m_pControlList = NULL;

/********************************************************************************************

>	ControlList::ControlList()

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ControlList::ControlList()
{
	m_pHash = new ControlPtrToListEntry;
	m_ChangedFlag = FALSE;
}

/********************************************************************************************

>	ControlList::~ControlList()

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ControlList::~ControlList()
{
	if (m_pHash)
	{
		delete m_pHash;
		m_pHash = NULL;
	}
}

/********************************************************************************************

>	static BOOL ControlList::Init()

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE on success
	Purpose:	static initalizer
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ControlList::Init()
{
	ERROR2IF(m_pControlList, FALSE, "Double init of ControlList");
	m_pControlList = new ControlList;
	ERROR2IF(!m_pControlList, FALSE, "Cannot get a new ControlList");

	wxToolTip::SetDelay(750);

	return TRUE;
}

/********************************************************************************************

>	static void ControlList::DeInit()

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE on success
	Purpose:	static initalizer
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ControlList::DeInit()
{
	ERROR3IF(!m_pControlList, "ControlList not initialized before DeInit()");
	if (m_pControlList)
	{
		delete m_pControlList;
		m_pControlList = NULL;
	}
	return;
}

/********************************************************************************************

>	BOOL ControlList::NewControl(wxControl * pControl)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		pControl - pointer to the control to add
	Outputs:	-
	Returns:	TRUE on success
	Purpose:	Adds a control to the list managed by us
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ControlList::NewControl(wxControl * pControl)
{
	ERROR2IF(!pControl, FALSE, "Null control pointer");
	ERROR2IF(!m_pHash, FALSE, "No hash table");
	ControlPtrToListEntry::iterator i=m_pHash->find(pControl);
	ERROR2IF((i!=m_pHash->end()), FALSE, "Control already present in hash table");
	ControlList::ListEntry l;
	(*m_pHash)[pControl]=l;
	m_ChangedFlag = TRUE;
	return TRUE;
}

/********************************************************************************************

>	BOOL ControlList::DeleteControl(wxControl * pControl)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		pControl - pointer to the control to delete
	Outputs:	-
	Returns:	TRUE on success
	Purpose:	Removes a control from the list managed by us
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ControlList::DeleteControl(wxControl * pControl)
{
	ERROR2IF(!pControl, FALSE, "Null control pointer");
	ERROR2IF(!m_pHash, FALSE, "No hash table");	
	ControlPtrToListEntry::iterator i=m_pHash->find(pControl);
	ERROR2IF((i==m_pHash->end()), FALSE, "Control not present in hash table");
	m_pHash->erase(i);
	m_ChangedFlag = TRUE;
	return TRUE;
}

/********************************************************************************************

>	BOOL ControlList::AssociateControl(wxControl * pControl, DialogOp * pDialogOp = NULL)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		pControl - pointer to the control to associate
				pDialogOp - pointer to the DialogOp to associate it with
	Outputs:	-
	Returns:	TRUE on success
	Purpose:	Associates a control with a dialog and an OpDescriptor (divined automagically)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ControlList::AssociateControl(wxControl * pControl, DialogOp * pDialogOp)
{
	ERROR2IF(!pControl, FALSE, "Null control pointer");
	ERROR2IF(!m_pHash, FALSE, "No hash table");	
	ControlPtrToListEntry::iterator i=m_pHash->find(pControl);
	ERROR2IF((i==m_pHash->end()), FALSE, "Control not present in hash table");
	OpDescriptor * pOpDesc = NULL;	
	wxString opDesc;

	BOOL SpecifiedOpDesc = FALSE;

	// wxCamArtControl class can have its OpDescriptor specified in the resources. If it's
	// there, use that
	if (pControl->IsKindOf(CLASSINFO(wxCamArtControl)))
	{
		opDesc = ((wxCamArtControl *)(pControl))->GetOpDesc();
	}

	if (!(opDesc.IsEmpty()))
	{
		pOpDesc = OpDescriptor::FindOpDescriptor((TCHAR *)(opDesc.c_str()));
		SpecifiedOpDesc = TRUE;
#if _DEBUG
		if (!pOpDesc)
		{
			// ERROR3_PF(("Cannot find OpDescriptor for %s",opDesc.c_str())); // this goes off too often right now
			TRACE(_T("ControlList::AssociateControl - Cannot find OpDescriptor for %s"),opDesc.c_str());
		}
#endif
	}
	else
	{
		pOpDesc = OpDescriptor::FindOpDescriptor((ResourceID)(pControl->GetId()));
	}

	(i->second).m_pOpDesc=pOpDesc;
	(i->second).m_SpecifiedOpDesc = SpecifiedOpDesc;
	(i->second).m_pDialogOp = pDialogOp;

	if (pOpDesc)
	{
		// Set the tooltip if any
		ResourceID ToolTip=pOpDesc->GetBubbleId();
		if (ToolTip)
			pControl->SetToolTip(wxString(CamResource::GetText(ToolTip)));

		ResourceID Help=pOpDesc->GetHelpId();
		if (Help)
			pControl->SetHelpText(wxString(CamResource::GetText(Help)));
	}

	m_ChangedFlag = TRUE;

	ReflectState(i->first, &i->second, TRUE);

	return TRUE;
}

/********************************************************************************************

>	BOOL ControlList::ReflectState(const wxControl * pControl, ControlList::ListEntry * pListEntry, BOOL ForceUpdate=FALSE);

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		pControl - pointer to the control to reflect state to
				pListEntry - pointer to list entry with data in
				ForceUpdate - TRUE to force an update even if old state supposedly the same (e.g. on creation)
	Outputs:	-
	Returns:	TRUE on success
	Purpose:	Removes a control from the list managed by us
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ControlList::ReflectState(wxControl * pControl, ControlList::ListEntry * pListEntry, BOOL ForceUpdate)	
{
	if (Camelot.CamelotIsDying())
		return FALSE;

	if (pListEntry->m_pOpDesc)
	{
		String_256 Dummy;
		OpState NewState = pListEntry->m_pOpDesc->GetOpsState(&Dummy);
		if (ForceUpdate || (pListEntry->m_OpState.Greyed != NewState.Greyed) || (pListEntry->m_OpState.Ticked != NewState.Ticked))
		{
			pListEntry->m_OpState = NewState;
			
			wxWindow*	pFocusWnd = wxWindow::FindFocus();
			pControl->Enable(!(pListEntry->m_OpState.Greyed));
			
			// The disabled control had focus, so we must find a sibling control to 
			// take the focus or face losing keypresses.
			if( pListEntry->m_OpState.Greyed && pFocusWnd == pControl )
			{
				wxWindowList& lstChildren = pControl->GetParent()->GetChildren();
				size_t	cChild = lstChildren.GetCount();
				
				bool	fFound = false;
				for( size_t ord = 0; ord < cChild; ++ord )
				{
					wxWindow* pSibling = lstChildren.Item( ord )->GetData();
					if( pSibling->IsEnabled() )
					{
						pSibling->SetFocus();
						fFound = true;
						break;
					}
				}
				
				// No enabled siblings, fall-back to selecting the panel
				if( !fFound )
					pControl->GetParent()->SetFocus();
			}
			
			// Send this to DialogManager so we have a generic implementation
			DialogManager::SetBoolGadgetSelected(pControl->GetParent(), pControl->GetId(), pListEntry->m_OpState.Ticked);
		}
	}
	else
	{
		// Hack to grey only the tool buttons when they have no OpDescriptor attached
		// if (pControl->GetStyle() & wxCACS_TOOLBACKGROUND)

		// If there was an OpDescriptor specified in the resource file, but we couldn't find it, we grey
		// the control. If no OpDescriptor was specified, we don't grey the control because it might be
		// simply that there is not meant to be one (e.g. infobar controls)
		if (pListEntry->m_SpecifiedOpDesc)
			pControl->Enable(FALSE);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL ControlList::ReflectAllStates(BOOL IgnoreChangedFlag = FALSE)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE on success
	Purpose:	Refreshes all controls on the list
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ControlList::ReflectAllStates(BOOL IgnoreChangedFlag)
{
	if (Camelot.CamelotIsDying())
		return FALSE;

	if (!m_ChangedFlag && !IgnoreChangedFlag)
		return TRUE;

	m_ChangedFlag = FALSE;

	ERROR2IF(!m_pHash, FALSE, "No hash table");	
	ControlPtrToListEntry::iterator i;
	for( i = m_pHash->begin(); i != m_pHash->end(); ++i )
	{
		ReflectState(i->first, &i->second);
	}
	return TRUE;
}

/********************************************************************************************

>	void ControlList::Invoke(wxControl * pControl, OpParam* pOpParam = NULL, BOOL fWithUndo = TRUE)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	02/12/2005
	Inputs:		pControl - pointer the control
				pOpParam - parameters
				fwithUndo - invoke with undo
	Outputs:	-
	Returns:	-
	Purpose:	Invokes the Op attached to the OpDescriptor, with the parameters pOpParam & fwithUndo
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ControlList::Invoke(wxControl * pControl, OpParam* pOpParam, BOOL fWithUndo)
{
	if (Camelot.CamelotIsDying())
		return;

	ERROR2IF(!m_pHash, (void)0, "No hash table");	
	ControlPtrToListEntry::iterator i=m_pHash->find(pControl);
	ERROR2IF((i==m_pHash->end()), (void)0, "Control not present in hash table");
	ReflectState(i->first, &i->second);
	
	OpDescriptor * pOpDesc = i->second.m_pOpDesc;
	if (pOpDesc && !i->second.m_OpState.Greyed)
	{
		pOpDesc->Invoke(pOpParam, fWithUndo);
	}
}

/********************************************************************************************

>	OpDescriptor * ControlList::Find(wxControl * pControl)

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	04/03/2006
	Inputs:		pControl - pointer to the control to look for
	Outputs:	-
	Returns:	NULL if not found, else pointer to the OpDescriptor
	Purpose:	Searches for the OpDescriptor attached to a control
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpDescriptor * ControlList::Find(wxControl * pControl)
{
	if (Camelot.CamelotIsDying())
		return NULL;

	ERROR2IF(!m_pHash, NULL, "No hash table");	
	ControlPtrToListEntry::iterator i=m_pHash->find(pControl);
	if (i==m_pHash->end())
		return NULL;

	return i->second.m_pOpDesc;
}

/********************************************************************************************
>	BOOL ControlList::BuildGadgetList(List* pOutputList, OpDescriptor * pOpDesc)

	Author:		Alex Bligh <alex@alex.org.uk>
	Created:	5 Mar 2005
	Inputs:		A pointer to a list which will hold the gadgets.
	Outputs:	A list of gadgets.
	Returns:	TRUE if there are some gadgets associated with this OpDescriptor,
				FALSE if there aren't any.
	Purpose:	Builds a list of GadgetListItems, each item holding a DialogBarOp* and a
				CGadgetID for a control associated with this OpDescriptor.  This allows
				the caller to manipulate the controls even if there isn't a message from
				them needing processing.  NB. the caller is responsible for allocating
				and deallocating this list!
	Errors:		-
	SeeAlso:	class GadgetListItem
********************************************************************************************/

BOOL ControlList::BuildGadgetList(List* pOutputList, OpDescriptor * pOpDesc)
{
	if (Camelot.CamelotIsDying())
		return FALSE;

	ERROR2IF(!pOutputList, FALSE, "Null output list in OpDescriptor::BuildGadgetList");

	// Iterate through all controls
	ERROR2IF(!m_pHash, FALSE, "No hash table");	
	ControlPtrToListEntry::iterator i;
	for( i = m_pHash->begin(); i != m_pHash->end(); ++i )
	{
		if (i->second.m_pOpDesc == pOpDesc)
		{
			DialogOp * pDialogOp = i->second.m_pDialogOp;
			if (pDialogOp)
			{
				// The control shares our OpDescriptor, and has a dialog so put it on the list
				GadgetListItem* pgListItem = new GadgetListItem(pDialogOp, i->first->GetId());
				if (pgListItem == NULL)
				{
					pOutputList->DeleteAll();
					return FALSE;
				}
	
				pOutputList->AddHead(pgListItem);
			}
		}
	}

	// Return TRUE/FALSE depending on if there are any entries on the list.
	return !pOutputList->IsEmpty();
}



/********************************************************************************************

>	BOOL ControlList::SendMessageToAllBarControls(DialogOp * pDialogOp, DialogMsg * DlgMsg, CWindowID pWindow=NULL)
	Author:		Alex Bligh
	Created:	03-Mar-2005
	Inputs:		pDialogOp - pointer to the DialogBarOp
				DlgMsg - Pointer to the message to send
				pWindow - pointer to the Window to recurse through (NULL for all external callers)
	Purpose:    This iterates through the controls on a bar and sends them each DlgMsg, or
				more accurately calls SendMessageToBarControl for each control with an
				OpDescriptor attached
	Returns:	TRUE to destroy the window, else FALSE
	SeeAlso:	DialogOp::Open

********************************************************************************************/

BOOL ControlList::SendMessageToAllControls(DialogOp * pDialogOp, DialogMsg * DlgMsg, CWindowID pWindow)
{
	BOOL Destroy = FALSE;

	// If at the top of the recursion, look at the appropriate wxWindow
	if (!pWindow)
	{
		pWindow = pDialogOp->GetReadWriteWindowID();
	}

	// Handle this window
	OpDescriptor * pOpDesc = OpDescriptor::FindOpDescriptor((ResourceID)(pWindow->GetId()));
	if (pOpDesc)
	{
		const BarControlInfo * pBarControlInfo = pOpDesc->GetBarControlInfo();
		if (pBarControlInfo)
		{
			BarControlInfo bci = *pBarControlInfo;
			bci.ControlID = pWindow->GetId();
			bci.ResourceID = pWindow->GetId();
			pOpDesc->SetBarControlInfo(bci);
			// Pass a flag saying whether we think this has been internally processed
			Destroy |= pDialogOp->SendMessageToControl(pOpDesc, DlgMsg,
														pWindow->IsKindOf(CLASSINFO(wxCamArtControl)));
		}
	}

	// Now handle all children
	wxWindowList::Node * pNode = pWindow->GetChildren().GetFirst();
	while (pNode)
	{
		Destroy |= SendMessageToAllControls(pDialogOp, DlgMsg, pNode->GetData());
		pNode = pNode->GetNext();
	}
	return Destroy;
}


/********************************************************************************************

>	void ControlList::RegisterWindowAndChildren(wxWindow * pWindow, DialogOp * pDialogOp = NULL)
	Author:		Alex Bligh
	Created:	03-Mar-2005
	Inputs:		pWindow - pointer to the Window to recurse through
	Purpose:    pDialogOp - pointer to a DialogOp to associate with (or NULL for none)
				This iterates through a windows children, and registers it and all its children
	Returns:	TRUE to destroy the window, else FALSE
	SeeAlso:	DialogOp::Open

********************************************************************************************/

void ControlList::RegisterWindowAndChildren(wxWindow * pWindow, DialogOp * pDialogOp)
{
	ERROR2IF(!m_pHash, (void)0, "No hash table");	
	if (pWindow)
	{
		if (pWindow->IsKindOf(CLASSINFO(wxControl)))
		{
			wxControl * pControl = (wxControl *)pWindow;
			// Remove this one
			ControlPtrToListEntry::iterator i=m_pHash->find(pControl);
			if (i==m_pHash->end())
			{
				if (NewControl(pControl))
					AssociateControl(pControl, pDialogOp);
			}
		}

		// Iterate through all children
		wxWindowList::Node * pNode = pWindow->GetChildren().GetFirst();
		while (pNode)
		{
			RegisterWindowAndChildren(pNode->GetData(), pDialogOp);
			pNode = pNode->GetNext();
		}
	}
	return;
}

/********************************************************************************************

>	void ControlList::RemoveWindowAndChildren(wxWindow * pWindow)
	Author:		Alex Bligh
	Created:	03-Mar-2005
	Inputs:		pWindow - pointer to the Window to recurse through (NULL for all external callers)
	Purpose:    This iterates through a windows children, and removes it and the children
				from the list.
	Returns:	TRUE to destroy the window, else FALSE
	SeeAlso:	DialogOp::Open

********************************************************************************************/

void ControlList::RemoveWindowAndChildren(wxWindow * pWindow)
{
	ERROR2IF(!m_pHash, (void)0, "No hash table");	
	if (pWindow)
	{
		if (pWindow->IsKindOf(CLASSINFO(wxControl)))
		{
			wxControl * pControl = (wxControl *)pWindow;
			// Remove this one
			ControlPtrToListEntry::iterator i=m_pHash->find(pControl);
			if (i!=m_pHash->end())
				DeleteControl(pControl);
		}

		// Iterate through all children
		wxWindowList::Node * pNode = pWindow->GetChildren().GetFirst();
		while (pNode)
		{
			RemoveWindowAndChildren(pNode->GetData());
			pNode = pNode->GetNext();
		}
	}
	return;
}
