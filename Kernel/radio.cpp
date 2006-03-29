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
// 


#include "camtypes.h"
#include "dlgtypes.h"
#include "dialogop.h"
#include "radio.h"


// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(RadioGroup, CC_CLASS_MEMDUMP)


// We want better memory tracking
#define new CAM_DEBUG_NEW


// Functions follow

/********************************************************************************************

>	BOOL RadioGroup::SetAssociations(DialogOp* pDialog, CGadgetAssociation* AssocArray, 
					UINT32 NumGadgets)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/96
	Inputs:		pDialog - The dialog in which this RadioGroup is
				AssocArray - the array of CGadgetID / CCObject associations
				NumGadgets - the number of radio items in the array
	Outputs:	-
	Returns:	TRUE if set OK
	Purpose:	Sets up the RadioGroup. Call prior to any other member.
	Errors:		ERROR2IF pDialog, AssocArray NULL or NumGadgets zero
	SeeAlso:	class RadioGroup

********************************************************************************************/
BOOL RadioGroup::SetAssociations(DialogOp* pDialog, CGadgetAssociation* AssocArray, UINT32 NumGadgets)
{
	ERROR2IF(pDialog == NULL, FALSE, "RadioGroup::SetAssociations() - pDialog NULL")
	ERROR2IF(AssocArray == NULL, FALSE, "RadioGroup::SetAssociations() - AssocArray NULL")
	ERROR2IF(NumGadgets == 0, FALSE, "RadioGroup::SetAssociations() - NumGadgets is zero")

	m_pDialog = pDialog;
	m_Associations = AssocArray;
	m_nNumAssociations = NumGadgets;
	return TRUE;
}


/********************************************************************************************

>	CCObject*	RadioGroup::GetDefault()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/96
	Inputs:		-
	Outputs:	-
	Returns:	NULL
	Purpose:	Allows the setting of a new ApplyNow state.
	Errors:		ERROR3IF always because its not implemented. Do not use
	SeeAlso:	class RadioGroup

********************************************************************************************/
CCObject*	RadioGroup::GetDefault()
{
	ERROR3IF(TRUE, "RadioGroup::GetDefault() - not implemented")

	return NULL;
}


/********************************************************************************************

>	BOOL RadioGroup::Disable()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
				FALSE otherwise
	Purpose:	Disables all the radio items in this group
	Errors:		ERROR2IF no dialog associated with this RadioGroup
	SeeAlso:	Enable()

********************************************************************************************/
BOOL RadioGroup::Disable()
{
	ERROR2IF(m_pDialog == NULL, FALSE, "RadioGroup::Disable - m_pDialog not set")

	for (UINT32 i = 0; i < GetRadioBoxCount(); i++)
	{
		m_pDialog->EnableGadget(m_Associations[i].m_GadgetID, FALSE);
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL RadioGroup::Enable()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful
				FALSE otherwise
	Purpose:	Disables all the radio items in this group
	Errors:		ERROR2IF no dialog associated with this RadioGroup
	SeeAlso:	Enable()

********************************************************************************************/
BOOL RadioGroup::Enable()
{
	ERROR2IF(m_pDialog == NULL, FALSE, "RadioGroup::Disable - m_pDialog not set")

	for (UINT32 i = 0; i < GetRadioBoxCount(); i++)
	{
		m_pDialog->EnableGadget(m_Associations[i].m_GadgetID, FALSE);
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL RadioGroup::SetDefault(CCObject* pDefaultObject)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/96
	Inputs:		pDefaultObject - pointer to the object 
	Outputs:	-
	Returns:	TRUE if the gadget with which pDefaultObject is associated was set
				FALSE otherwise
	Purpose:	Sets the gadget associated with the given object as the default (using
				SetBoolGadgetSelected)
	Errors:		ERROR2IF no dialog associated with this RadioGroup
				ERROR3IF pDefaultObject is not associated with any gadget in this radio group
	SeeAlso:	class RadioGroup

********************************************************************************************/
BOOL RadioGroup::SetDefault(CCObject* pDefaultObject)
{
	ERROR2IF(m_pDialog == NULL, FALSE, "RadioGroup::SetDefault - m_pDialog not set")

	for (UINT32 i = 0; i < GetRadioBoxCount(); i++)
	{
		if (m_Associations[i].m_pObject == pDefaultObject)
			return (m_pDialog->SetBoolGadgetSelected(m_Associations[i].m_GadgetID, TRUE));
	}
	ERROR3IF(TRUE, "RadioGroup::SetDefault - pDefaultObject not in RadioGroup");
	return FALSE;
}	


/********************************************************************************************

>	CCObject* RadioGroup::GetSelected()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/05/96
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to the Object associated with the radio box selected
	Purpose:	Allows the setting of a new ApplyNow state.
	Errors:		ERROR2IF no dialog associated with this RadioGroup
	SeeAlso:	class RadioGroup

********************************************************************************************/
CCObject* RadioGroup::GetSelected()
{
	ERROR2IF(m_pDialog == NULL, NULL, "RadioGroup::GetSelected - m_pDialog not set")

	for (UINT32 i = 0; i < GetRadioBoxCount(); i++)
	{
		if (m_pDialog->GetBoolGadgetSelected(m_Associations[i].m_GadgetID))
			return (m_Associations[i].m_pObject);

	}
	ERROR3IF(TRUE, "RadioGroup::GetSelected() - None selected");
	return NULL;
}	
