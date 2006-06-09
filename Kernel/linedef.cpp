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

// The line definition class implementation.  An abstract base class from which to
// derive the definitions of things like strokes, brushes.

#include "camtypes.h"
#include "linedef.h"
//#include "colormgr.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]

CC_IMPLEMENT_DYNAMIC(LineDefinition, CCObject)



/********************************************************************************************

>	LineDefinition::LineDefinition()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/97

	Purpose:	default constructor, please don't use it though, i am only keeping it around so 
				I can integrate the StrokeDefinition stuff

********************************************************************************************/

LineDefinition::LineDefinition()
{
	// set to NULL or else CascadeDelete fails in the destructor
	m_pTree = NULL;
}

/********************************************************************************************

>	LineDefinition::LineDefinition(Node *pLineTree);

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/97

	Inputs:		pLineTree - A pointer to a Spread node which is the root of a clipart
							  subtree which should be used for this Line definition. It
							  must obey these rules:
								* It must be a Spread
								* It must not be linked into any other parent tree structure
								* It should contain at least one path node (or else the Line
								  will appear "blank"). You should do Make Shapes on the 
								  subtree so that text, blends, quickshapes, etc are all in
								  path form
								* It should be attribute complete, or close thereto

	Purpose:	Constructor

	SeeAlso:	LineComponent::AddNewLine

********************************************************************************************/

LineDefinition::LineDefinition(Node *pLineTree)
{
	ERROR3IF(pLineTree == NULL, "Illegal NULL param");
	ERROR3IF(!pLineTree->IsSpread(), "LineDefinitions must begin with a Spread (for now, at least)");
	ERROR3IF(pLineTree->FindParent() != NULL, "Line Definition looks like it's linked into a tree!");

	m_pTree	  = pLineTree;

	// Check the subtree to see if it contains any transparency
	m_bNeedsTrans = m_pTree->ChildrenNeedTransparency();

	IOStore = 0;

	m_Name = TEXT("Custom line");

	m_bOverridesFill  = FALSE;
	m_bOverridesTrans = FALSE;
}



/********************************************************************************************

>	LineDefinition::~LineDefinition()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/2/97

	Purpose:	Destructor

********************************************************************************************/

LineDefinition::~LineDefinition()
{
	if (m_pTree	!= NULL)
	{
		m_pTree->CascadeDelete();
		delete m_pTree;
	}
}



/********************************************************************************************

>	void LineDefinition::SetLineName(StringBase *pName)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97

	Purpose:	Sets the name text string for this Line

********************************************************************************************/

void LineDefinition::SetLineName(StringBase *pName)
{
	ERROR3IF(pName == NULL, "Illegal NULL param");

	pName->Left(&m_Name, 31);
}



/********************************************************************************************

>	virtual BOOL LineDefinition::NeedsTransparency() const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97

	Returns:	TRUE if this Line needs transparency in order to render

	Purpose:	Determine if this Line needs transparency in order to render.

********************************************************************************************/

BOOL LineDefinition::NeedsTransparency() const
{
	return(m_bNeedsTrans);
}


/********************************************************************************************

>	static void LineDefinition::ConvertIndexedColours(Node* pRoot) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/3/97
	Inputs :	pRoot - the root of the subtree that is about to passed in to create a linedef
				Note that this node must actually be a SPREAD, as that is what we expect
	Ouputs:     pSpread with all its indexed colours converted to local doccolours
	Returns:	-

	Purpose:	To convert all the colours in the subtree that we wish to use to make a 
				brush definition
********************************************************************************************/

void LineDefinition::ConvertIndexedColours(Spread* pRoot)
{
	PORTNOTETRACE("other","LineDefinition::ConvertIndexedColours - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (pRoot == NULL)
	{
		ERROR3("Spread is NULL");
		return;
	}
	// start at the layer
	Node* pNode = pRoot->FindFirstChild();
	pNode = pNode->FindFirstChild();

	// traverse across the layer
	while (pNode != NULL)
	{
		// search depth first as we may have a group
		Node *pCurNode = pNode->FindFirstDepthFirst();
		Node *pNextNode;

		while (pCurNode !=NULL)
		{
			// We may be about to chop this node out of the tree, so get the next node now
			pNextNode = pCurNode->FindNextDepthFirst(pNode);
	
			// Use to scan the colour fields of the attribute.
			if (pCurNode->IsAnAttribute())
			{
				UINT32 Context = 0;
				NodeAttribute *pNodeAttr = (NodeAttribute *) pCurNode;

				// Get the next colour field from the attribute
				DocColour *pColour = pNodeAttr->EnumerateColourFields(Context++);
				
				while (pColour != NULL)
				{
					// For each colour field, make sure the colour is a local DocColour so that
					// the sub-tree is entirely stand-alone
					IndexedColour* pIndexCol = pColour->FindParentIndexedColour();
					if (pIndexCol != NULL)
					{
						ColourGeneric ColDef;
						ColourContext *cc = ColourManager::GetColourContext(pColour->GetColourModel());
						ERROR3IF(cc == NULL, "Can't find colour context?!");

						// Get the IndexedColour definition as a standalone colour definition
						cc->ConvertColour(pIndexCol, &ColDef);

						// Make the DocColour into a simple standalone "lookalike" of the parent colour
						*pColour = DocColour(pColour->GetColourModel(), &ColDef);

						// if the parent colour was named then set the reserved flag
						if (pIndexCol->IsNamed())
							pColour->SetReservedFlag(COL_NAMED);
					}
						pColour = pNodeAttr->EnumerateColourFields(Context++);
				}
			}
			pCurNode = pNextNode;
		}
		
		pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
	}
#endif
}
