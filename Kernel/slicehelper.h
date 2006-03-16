// $Id: slicehelper.h 662 2006-03-14 21:31:49Z alex $
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
// All fully static functions to help with scanning and finding
// developed to help the slicing opperations
// but can be used elsewhere - sjk 30/9/99

#ifndef MAX_BUTTONS_IN_A_BAR

#define MAX_BUTTONS_IN_A_BAR	20

#define INCLUDE_CLICKED_STATE	1 // 1 to have a clicked state, 0 to not have it

#include "ngscan.h"

class TemplateAttribute;
//class SGNameItem;
class UndoableOperation;
class TextLine;
class TextStory;

class SliceHelper
{
public:
	// These stand for things you can do to the selection in SelectAllSetsInRect.
	enum SelStateAction { CLEAR, SET, TOGGLE };

	// find layers
	static Layer * FindLayerCalled(const StringBase & LayerName);

	// count buttons
	static INT32 CountButtonsInBar(const StringBase & BarName);
	static void CountButtonsInBarScan(Node * pNode, TemplateAttribute ** ppFoundButton, INT32 * pNumberOfButtons, const StringBase & BarName);

	// build lists of buttons in a bar
	static void BuildListOfNodesInBar(List * pList, Node * pNode, const StringBase & BarName);

	// find out about sets from the name gallery
	static SGNameItem* LookupNameGalleryItem(const StringBase & strName);
	static void GetNextFreeButtonName(INT32 &butno, StringBase * pStr = NULL);

	// nice very generic function
	static Node * FindNextOfClass(Node *pNode, Node * pLidNode, const class CCRuntimeClass * pClass, BOOL CheckThis = FALSE);
	// an inline version of the above code which looks purely for WixTemplateAttributes
	// made inline and without the IS_A's for speed purposes (sjk 1/8/00)
	static Node * FindNextNameNode(Node *pNode, Node * pLidNode, BOOL CheckThis = FALSE)
	{
		Node * pNextNode = NULL;
		BOOL tested = FALSE;
		Node * pPassedNode = CheckThis ? NULL : pNode;

		while (pNode != pLidNode || !tested)
		{
			tested = TRUE;

			// check this node as we move across
			if (!pNode->IsNodeHidden() && pNode != pPassedNode && pNode->IsAnObjectName())
				return pNode;

			// while can go down do so
			pNextNode = pNode->FindFirstChild();

			if (pNextNode && pNextNode->IsNodeHidden())
				pNextNode = pNextNode->FindNextNonHidden();
			
			while(pNextNode)
			{
				pNode = pNextNode;
				
				// check this node as we move down
				if (!pNode->IsNodeHidden() && pNode != pPassedNode && pNode->IsAnObjectName())
					return pNode;

				pNextNode = pNode->FindFirstChild();

				if (pNextNode && pNextNode->IsNodeHidden())
					pNextNode = pNextNode->FindNextNonHidden();
			}

			// cant go down and we are already at the top
			if (pNode == pLidNode)
				return NULL;

			// find next none hidden brother
			pNextNode = pNode->FindNextNonHidden();

			if (pNextNode)
				pNode = pNextNode;
			else // no brothers so find an uncle
			{
				BOOL FoundUncle = FALSE;

				while (!FoundUncle)
				{
					pNextNode = pNode->FindParent();
					if (pNextNode == pLidNode || !pNextNode)
						return NULL;

					pNode = pNextNode;
					pNextNode = pNode->FindNextNonHidden();

					// found uncle
					if (pNextNode)
					{
						pNode = pNextNode;
						FoundUncle = TRUE;
					}
				}
			}
		}

		return NULL;
	}

	// code for selecting all objects with the same name as a specific Node, or all objects in a named set.
	static void SelectAllSetsOfThisNode(NodeRenderableInk* pInk, String_256 & SetName, BOOL bShift);
	static void RecurseSelectAllSetsOfThisNode(Node * pAttrs, String_256 & SetName, BOOL bShift);
	static BOOL SelectObjectsInSet(const StringBase& strName, SelectScan::Change eNewState);

	// a custom function for selecting all nodes sharing names with those nodes within a given selection rectangle.
	// maybe this function should be moved into NodeRenderable?
	static void SelectAllSetsInRect(const DocRect, Spread*, SelStateAction st = SET);

	static String_256 GetBarName(TemplateAttribute * pTemplAttrib);
	static void MeshImportedLayersWithExistingButtonBars(Node * pImportedLayer[5], UndoableOperation * pUndoableOp, BOOL Imported);
	static BOOL IsUniqueName(const String_256 & Name, List * pList);

	static Node * ReplaceAttrsInTree(UndoableOperation * pOp,
									   List * pImportedAttrList,
									   const String_256 & OldButtonName,
									   const String_256 & OldBarName,
									   const String_256 & NewButtonName,
									   const String_256 & NewBarName,
									   Node ** ppNodeFound = NULL);

	static BOOL BarNameExists(Node * pNode, String_256 &BarName, Node * pLid = NULL);
	static INT32 FindTargetAndExtender(Node * pStartNode,
						String_256 & Target,
						String_256 & Extender, 
						INT32 RequiredLevel, 
						BYTE *pExtenderFlags = NULL,
						DocRect *pTargetRect = NULL,
						DocRect *pExtenderRect = NULL);


	static BOOL CreatePropertiesForSet(	const String_256& SetName,
										const String_256& BarName,
										BOOL isSlice,
										BOOL Stretches,
										BOOL IsBackBar,
										BYTE StretchFlags,
										const String_256& StretchedBy,
										BOOL DontOverWrite = FALSE,
										DocRect *pTargetRect = NULL,
										DocRect *pExtenderRect = NULL,
										UndoableOperation * pOp = NULL,
										NodeSetProperty * pExampleProp = NULL);

	static BOOL PurgeUseOfSetName(const StringBase & SetName, UndoableOperation* pOp, const String_256 * pReplacementName = NULL);


	static BOOL MakeTriggerLikeExample(const String_256 & NewButtonName, const String_256 & ButtonName, const String_256 * pExclude = NULL);

	static DocRect BoundingNodeSize(Node * pNode);

	static INT32 GetBarNumberFromBarName(const String_256 & BarName);

	static TemplateAttribute * FindFirstSetNodeBelongsTo(Node * pNode);
	static String_256 GetSetNameFromAttrib(Node * pNode);
	static TextStory * FindNextTextStoryToSync( TextStory * pLastStory,
												Node * pLid,
												TextStory * pMasterStory,
												const String_256 & ButtonName,
												const String_256 & MasterText,
												BOOL AnyText = FALSE);

	static BOOL TextStoriesHaveSameText (TextStory * pStory1, TextStory * pStory2);
	static BOOL TextLinesHaveSameText (TextLine * pLine1, TextLine * pLine2);
	static BOOL SyncTextLines (TextLine * pLine1, TextLine * pLine2, UndoableOperation * pOp);
	static BOOL SyncTextStories (TextStory * pStory, TextStory * pMaster, UndoableOperation * pOp);

	static BOOL OnTextStoryChanged(	TextStory * pMasterStory,
									UndoableOperation * pOp,
									ObjChangeParam * pObjChange,
									const String_256 & MasterText);

	static DocRect ScanForSetSizeExcluding (const String_256 & IncludeSet, const String_256 & ExcludeSet);
	static BOOL BarExistsOnLayer(const String_256 &BarName, const String_256 &Layer);
	static void BarNameInRect(DocRect r, String_256 *pBarName);
	static void ShowLayer(BOOL Visible, Layer * pLayer, Spread * pSpread, UndoableOperation * pUndoOp);
	static INT32 DoesSelectionOnlyContainCompleteSets();

	static BOOL SetUsedInTree(Node * pNode, const String_256 &SetName, Node * pLid = NULL);
	static void DeleteUnusedReferences(const String_256 &SetName, UndoableOperation * pOp);

	static BOOL AddNamesToController (UndoableOperation * pOp, Node * pCtrlr);
	static BOOL RemoveNamesFromController (UndoableOperation * pOp, Node * pCtrlr);



	// Matt 6/11/2000
	// Given the current selection, auto-select all button elements which are fully selected on at least one layer
	static BOOL ModifySelectionToContainWholeButtonElements();

	// Matt 6/11/2000
	// Saves the current selection in m_pSelNodeList
	static void SaveSelection();

	// Matt 6/11/2000
	// Restores the current selection from that stored in m_pSelNodeList
	static void RestoreSelection();

	// Matt 18/12/2000
	// Validates the NodeSetSentinel and corrects it if it is corrupted
	static void ValidateNodeSetSentinel();

	// Matt 20/12/2000
	// Builds a list of all the nodes in a particular button on a given layer
	static void BuildListOfNodesInButton(List * pList, Layer * pLayer, const StringBase & ButtonName);

	// Matt 22/01/2001
	// Ensure that the Trigger Information is correct in the name gallery - it sometimes is not initialised until an
	// operation has called FastApplyStretchScan() - not very handy...
	static void EnsureTriggerInfo();

private:
	static List* m_pSelNodeList;

};

#endif
