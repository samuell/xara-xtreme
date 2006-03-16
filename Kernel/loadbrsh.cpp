// $Id: loadbrsh.cpp 662 2006-03-14 21:31:49Z alex $
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

// Load default brushes on start-up - Implementation

#include "camtypes.h"
#include "loadbrsh.h"
#include "camdoc.h"
#include "camelot.h"
#include "lddirect.h"
#include "document.h"
#include "docview.h"
#include "colormgr.h"
#include "bmpcomp.h"

CC_IMPLEMENT_MEMDUMP(LoadBrushDirect, LoadDirect);

#define new CAM_DEBUG_NEW

/*****************************************************************************************

>	virtual BOOL LoadBrushDirect::OnLoadDocument(Document* pKernelDoc)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/2000
	Inputs:		pKernelDoc = a pointer to the loaded kernel document
	Returns		TRUE if the document was processed correctly
				FALSE if not.
	Purpose:	We need to replace indexed brush colours by local colours - since the linked
				indexed colours get deleted (this especially effects fill ramps!).

				NOTE:  identical to that in void PrintMarksMan::ConvertAllDocColours(Document* pDoc)

*****************************************************************************************/

BOOL LoadBrushDirect::OnLoadDocument(Document* pDoc)
{
	ERROR3IF(pDoc==NULL,"No doc pointer during PrintMarksMan::ConvertAllDocColours!");

	Node *CurNode = Node::DocFindFirstDepthFirst(pDoc);
	Node *NextNode;

	while (CurNode !=NULL)
	{
		// We may be about to chop this node out of the tree, so get the next node now
		NextNode = CurNode->DocFindNextDepthFirst();

		// Use to scan the colour fields of the attribute.
		UINT32 Context = 0;
		DocColour *pColour;

		if (CurNode->IsAnAttribute())
		{
			NodeAttribute *pNodeAttr = (NodeAttribute *) CurNode;

			// Get the next colour field from the attribute
			pColour = pNodeAttr->EnumerateColourFields(Context++);

			while (pColour != NULL)
			{
				// For each colour field, make sure the colour is a local DocColour so that
				// the sub-tree is entirely stand-alone
				if (pColour->FindParentIndexedColour() != NULL)
				{
					ColourGeneric ColDef;
					ColourContext *cc = ColourManager::GetColourContext(pColour->GetColourModel());
					ERROR3IF(cc == NULL, "Can't find colour context?!");

					// Get the IndexedColour definition as a standalone colour definition
					cc->ConvertColour(pColour->FindParentIndexedColour(), &ColDef);

					// Make the DocColour into a simple standalone "lookalike" of the parent colour
					*pColour = DocColour(pColour->GetColourModel(), &ColDef);
				}

				pColour = pNodeAttr->EnumerateColourFields(Context++);
			}
		}
		CurNode = NextNode;
	}

	// All is well we hope.
	return TRUE;
}
