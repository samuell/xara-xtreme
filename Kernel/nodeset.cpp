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

// Implementation of Node sets, i.e. arbitrary collections of nodes.

/*
*/

#include "camtypes.h"

#include "nodeset.h"


DECLARE_SOURCE("$Revision: 662 $");

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

CC_IMPLEMENT_MEMDUMP(NodeSet, CCObject)


/********************************************************************************************

>	NodeSet::NodeSet(BOOL Strict)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94
	Inputs:		Strict - TRUE => Strict set - a node may only be in the set 0 or 1 times.
						FALSE => Not strict - a node may be in the set more than once.
	Purpose:	Create a NodeSet.  It may or may not be a strict set - it depends on the
				"Strict" parameter.  If it is strict, then when nodes are added, the class
				checks that they don't already exist in the set - if they do, it just does
				nothing and returns success.
	SeeAlso:	NodeSet

********************************************************************************************/

NodeSet::NodeSet(BOOL Strict) : m_NodeSet( 50 )
{
	// Remember what kind of set this is.
	m_fStrictSet = Strict;
}


/********************************************************************************************

>	BOOL NodeSet::AddNode(Node *pNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94
	Inputs:		pNode - pointer to the Node to add to this NodeSet.
	Returns:	TRUE if the set now contains the specified Node;
				FALSE if out of memory => ERROR1
	Purpose:	Adds the specified node to this node set. If this is a strict set (see the
				NodeSet constructor), then the set is first scanned to see if it already
				contains this node.  If it does, then the set is unchanged and success is
				returned.
	Errors:		Out of memory => ERROR1
	SeeAlso:	NodeSet::NodeSet

********************************************************************************************/

BOOL NodeSet::AddNode(Node *pNode)
{
	ERROR2IF(pNode == NULL, FALSE, "NULL node pointer in NodeSet::AddNode()");

	if( m_fStrictSet )
	{
		// Check to see if we haven't already got this node in the set
		size_t			cNode = m_NodeSet.size();
		for( size_t i = 0; i < cNode; i++ )
		{
			if( m_NodeSet[i] == pNode )
				// Already have this node - return success
				return TRUE;
		}
	}

	// See if we can add another node to this set.
	m_NodeSet.push_back( pNode );

	// Return success to caller
	return TRUE;
}

/********************************************************************************************

>	void NodeSet::MakeEmpty()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94
	Purpose:	Remove all nodes from this NodeSet.
	SeeAlso:	DynamicArray::EmptyArray

********************************************************************************************/

void NodeSet::MakeEmpty()
{
	// Delete all the array elements, and create a new set that are all NULL.
	m_NodeSet.clear();
}


/********************************************************************************************

>	void NodeSet::MarkNodes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94
	Purpose:	Set the MARKED flag on all Nodes in this NodeSet.
				This is usually done before doing a selective rendering pass on the 
				document tree.
	SeeAlso:	NodeSet::UnmarkNodes

********************************************************************************************/

void NodeSet::MarkNodes()
{

}

/********************************************************************************************

>	void NodeSet::UnmarkNodes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94
	Purpose:	Clear the MARKED flag on all Nodes in this NodeSet.
				This is usually done after doing a selective rendering pass on the 
				document tree.
	SeeAlso:	NodeSet::MarkNodes

********************************************************************************************/

void NodeSet::UnmarkNodes()
{

}


