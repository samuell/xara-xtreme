// $Id: opliveeffects.h 264 2006-01-19 12:28:28Z alex $
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
// Declaration of Bitmap Effects handling classes.
//

#ifndef INC_EFFECTS_STACK
#define INC_EFFECTS_STACK

class Range;
class ListRange;
class NodeEffect;
class EffectsStack;
class NodeLiveEffect;
class NodeFeatherEffect;
class NodeBitmapEffect;

// Define this if you want to pick up the development of the Feather Effect
#define FEATHER_EFFECT

/********************************************************************************************

>	class PPStackLevel : public ListItem

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2004
	Purpose:	
	SeeAlso:	

********************************************************************************************/

class PPStackLevel : public ListItem
{
public:
	CC_DECLARE_DYNAMIC( PPStackLevel )
	PPStackLevel() {pPPNode=NULL; strPostProID=""; bConsistent=TRUE;}
	~PPStackLevel() {}					// Nothing to destruct

// Properties
	NodeEffect*	pPPNode;		// pointer to sample LE node for this stack level
//	_bstr_t				bstrEditDoc;	// sample edit document text for this stack level
	String_256			strPostProID;	// Display name of this LiveEffect level
	ListRange			listSelNodes;	// List of nodes in this stack level
	BOOL				bConsistent;	// TRUE if this level is consistent
										// (all nodes have similar params according to CompareState)
};




/********************************************************************************************

>	class EffectsStack : public List

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/2004
	Purpose:	Find and represent a consistent arrangement of LiveEffects above a
				specified range (usually the SelRange)
				This is complicated because the SelRange can have an arbitary collection of
				LiveEffects applied to it which may or may not be wholly or partially
				consisent.
				This class currently holds info about LiveEffects that are applied to all
				nodes in the SelRange, contiguously, in the same order.

********************************************************************************************/

#define STACKPOS_BOTTOM 0
#define STACKPOS_TOP 5000
#define STACKPOS_INSERT_TOP 6000
#define STACKPOS_INVALID -1

class EffectsStack: public List
{
	CC_DECLARE_DYNAMIC(EffectsStack)

public:
	static ListRange*			GetEffectsStackFromNode(Node* pNode, BOOL bNodeMaybeMidStack = FALSE, BOOL bEscapeDerived = TRUE, BOOL bIncludeLocked = FALSE);
	static EffectsStack*		GetEffectsStackFromSelection(Range* pRange = NULL, BOOL bEscapeDerived = TRUE, BOOL bIncludeLocked = FALSE);
	static BOOL					NodesSharePPStack(Node* pNode, Node* pLastNode, BOOL bNodeMaybeMidStack = FALSE, BOOL bIncludeLocked = FALSE);
	static Node*				FindStackBaseFrom(Node* pNode, BOOL bIncludeLocked);
	static Node*				EscapeOldControllers(Node* pUserNode);
	static ListRange*			GetNewLevelRange(Range* pRange = NULL, BOOL bEscapeOldControllers = TRUE);
	static ListRange*			GetTopClassRange(CCRuntimeClass* pClass,
												 BOOL bClassOnly = TRUE,
												 BOOL bIgnorePassThroughEffects = FALSE,
												 Node** ppMasterNode = NULL,
											     INT32* piStackPos = NULL,
											     Range* pRange = NULL,
											     BOOL bEscapeOldControllers = TRUE);

public:
	EffectsStack();
	~EffectsStack();

	BOOL					FindBestProcessor(String_256* pstrEffectID, INT32* piStackPos);
	BOOL					GetLevelInfo(String_256* pstrEffectID, INT32* piStackPos);
	BOOL					LockableNonEmpty();
	void					Clear();
	ListRange*				GetLevelRange(INT32* piStackPos, BOOL bEscapeOldControllers = TRUE);
	ListRange*				GetBaseLevelRange();
	PPStackLevel*			GetLevel(INT32 iStackPos) {return (PPStackLevel*)FindItem(iStackPos);}
	ListRange*				FindLevelContaining(NodeEffect* pNode, INT32* piStackPos);
	BOOL					NodeInTopLevel(NodeEffect* pNode);
	INT32						FindSelectionPos();
	BOOL					BuildEffectMenu(ContextMenu* pMenu);
	void					AttrsHaveChanged();

protected:
	BOOL					Initialise(ListRange* pRange);
	BOOL					Intersect(ListRange* pRange);
	PPStackLevel*			FindMatch(PPStackLevel* pFirstItem, ListRange* pRange, NodeEffect* pPP);

// Properties
public:
	BOOL					bConsistent;

private:
	ListRange*				m_pNewLevelRange;			// This is a ListRange we made to cover the top of the selection
														// but which is not yet in the stack
};




#endif	// INC_EFFECTS_STACK
