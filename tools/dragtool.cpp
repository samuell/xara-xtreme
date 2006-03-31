// $Id$
// Implementation of the drag tool.
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
#include "dragtool.h"
#include "toollist.h"
#include "selector.h"
#include "spread.h"

DECLARE_SOURCE( "$Revision$" );

// Run-time type checking etc etc
CC_IMPLEMENT_MEMDUMP(DragTool, Tool_v1)

// report memory line info
#define new	 CAM_DEBUG_NEW

// These are still char* while we wait for resource technology to be developed for modules
TCHAR* DragTool::FamilyName = _T("Drag Tools");
TCHAR* DragTool::ToolName   = _T("Drag Tool");
TCHAR* DragTool::Purpose    = _T("The parent tool of Selector and Slice tools");
TCHAR* DragTool::Author     = _T("Karim MacDonald");

/********************************************************************************************

>	virtual void DragTool::DragMove(TransformBoundingData* pBoundingData)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/1999
	Purpose:	Placeholder for base-class implementations.
				Does nothing here.

********************************************************************************************/
void DragTool::DragMove(TransformBoundingData* pBoundingData)
{
	// empty.
}



/********************************************************************************************

>	virtual BOOL DragTool::BeyondFixedRange(double fpFixedVal)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com> from JustinF's SelectorTool::BeyondFixedRange code.
	Created:	14/10/1999
	Inputs:		fpFixedVal		the double that is to be converted to fixed-point
	Returns:	TRUE if the number is beyond the range that fixed-piont numbers can
				represent, FALSE if its OK to convert.
	Purpose:	Checks for under/overflow in fixed-point calculations.

				This function hails from SelectorTool::BeyondFixedRange.
				The reason it is here is that it needs to be called from
				a pointer to a DragTool, without going through everyone
				elses code and re-casting their pointer to SelectorTool*.
				Because it is a small function, it is implemented here, as well as
				in SelectorTool.

********************************************************************************************/
BOOL DragTool::BeyondFixedRange(double fpFixedVal)
{
	const double fpMinFixPt = 1.0 / (INT32(SHRT_MAX) + 1);		// 1/32768
	const double fpMaxFixPt = SHRT_MAX;							// 32767
	double fp = fabs(fpFixedVal);
	return fp < fpMinFixPt || fp > fpMaxFixPt;
}

/********************************************************************************************

>	virtual void DragTool::OnClick(DocCoord PointerPos, ClickType cType, ClickModifiers cMods, Spread* pSpread)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/7/2000
	Inputs:		standard things
	Returns:	-
	Purpose:	Call this function (for derived classes) when you don't want to process the
				message yourself (or you don't want to handle it).  This function locates the
				selector tool - and allows it to handle the message.

********************************************************************************************/

void DragTool::OnClick(DocCoord PointerPos, ClickType cType, ClickModifiers cMods, Spread* pSpread)
{
	if ((cType == CLICKTYPE_SINGLE) || (cType == CLICKTYPE_UP))
	{	
		ToolListItem* pSelectorItem = Tool::Find(TOOLID_SELECTOR);
		ERROR3IF(pSelectorItem == NULL, "ARGH! No Selector when we got a click!");
		ERROR3IF(pSelectorItem->m_pTool == NULL, "ARGH! No Selector when we got a click!");
		Tool* pSelector = pSelectorItem->m_pTool;

		if (pSelector)
		{	
			SelectorTool::AllowGuidelineScan (FALSE);
			pSelector->OnClick(PointerPos, cType, cMods, pSpread);
			SelectorTool::AllowGuidelineScan (TRUE);
		}
	}
}
